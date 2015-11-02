#include "librarymodel.h"

using namespace Library;

LibraryModel::LibraryModel( QObject *parent )
    : LibraryModel( UserDatabase::instance(), parent ) {

}

LibraryModel::LibraryModel( UserDatabase *db, QObject *parent )
    : LibraryModel( *db, parent ) {

}

LibraryModel::LibraryModel( UserDatabase &db, QObject *parent )
    : QSqlTableModel( parent, db.database() ),
      mGameScannerThread( this ),
      mFilterCollection( false ),
      mTransaction( false ),
      qmlInsertPaused( false ),
      qmlInsertCancelled( false ),
      qmlCount( 0 ),
      qmlRecursiveScan( true ),
      qmlProgress( 0.0 ) {

    mGameScanner.moveToThread( &mGameScannerThread );

    mRoleNames = QSqlTableModel::roleNames();
    mRoleNames.insert( TitleRole, QByteArrayLiteral( "title" ) );
    mRoleNames.insert( SystemRole, QByteArrayLiteral( "system" ) );
    mRoleNames.insert( TimePlayedRole, QByteArrayLiteral( "timePlayed" ) );
    mRoleNames.insert( ArtworkRole, QByteArrayLiteral( "artworkUrl" ) );
    mRoleNames.insert( FileNameRole, QByteArrayLiteral( "absoluteFilePath" ) );
    mRoleNames.insert( SystemPathRole, QByteArrayLiteral( "systemPath" ) );
    mRoleNames.insert( RowIDRole, QByteArrayLiteral( "rowIndex" ) );
    mRoleNames.insert( Crc32Role, QByteArrayLiteral( "crc32Checksum" ) );
    mRoleNames.insert( CoreFilePathRole, QByteArrayLiteral( "coreFilePath" ) );

    setEditStrategy( QSqlTableModel::OnManualSubmit );
    setTable( UserDatabase::tableName );
    select();

    // Connect Model to Worker.
    connect( this, &LibraryModel::droppedUrls, &mGameScanner, &GameScanner::handleDroppedUrls );
    connect( this, &LibraryModel::containsDrag, &mGameScanner, &GameScanner::handleContainsDrag );
    connect( this, &LibraryModel::draggedUrls, &mGameScanner, &GameScanner::handleDraggedUrls );
    connect( this, &LibraryModel::insertGames, &mGameScanner, &GameScanner::scanFolder );
    connect( this, &LibraryModel::signalInsertCancelled, &mGameScanner, &GameScanner::setInsertCancelled );
    connect( this, &LibraryModel::signalInsertPaused, &mGameScanner, &GameScanner::setInsertPaused );

    // Connect Worker to Model.

    // Do not change this from a blocking queued connection. This is to simplify the threaded code.
    connect( &mGameScanner, &GameScanner::insertGameData, this, &LibraryModel::handleInsertGame );
    connect( &mGameScanner, &GameScanner::started, this, [] {
        qCDebug( phxLibrary ) << "Worker started...";
    } );

    // Do some thread cleanup.
    connect( &mGameScanner, &GameScanner::finished, this, [ this ] {
        qCDebug( phxLibrary ) << "Worker finished...";
    } );

    // Listen to the Worker Thread.
    connect( &mGameScannerThread, &QThread::started, &mGameScanner, &GameScanner::eventLoopStarted );
    connect( &mGameScannerThread, &QThread::started, this, [ this ] {
        qCDebug( phxLibrary ) << "Worker thread started...";
    } );

    connect( &mGameScannerThread, &QThread::finished, this, [ this ] {
        qCDebug( phxLibrary ) << "Worker thread stopped...";
    } );

}

LibraryModel::~LibraryModel() {

    stopGameScannerThread();

    if( submitAll() ) {
        // database().commit();
    } else {
        database().rollback();
    }
}

QVariant LibraryModel::data( const QModelIndex &index, int role ) const {
    QVariant value = QSqlTableModel::data( index, role );

    if( role < Qt::UserRole ) {
        return value;
    } else {
        if( !mRoleNames.contains( role ) ) {
            return value;
        }

        // role name need to be the same as column name.
        int columnIdx = record().indexOf( mRoleNames.value( role ) );
        return QSqlTableModel::data( this->index( index.row(), columnIdx ), Qt::DisplayRole );
    }
}

QHash<int, QByteArray> LibraryModel::roleNames() const {
    return mRoleNames;
}

bool LibraryModel::select() {

    const QString query = selectStatement();

    if( query.isEmpty() ) {
        return false;
    }

    beginResetModel();

    //    d->clearCache();

    QSqlQuery qu( database() );

    qu.prepare( query );

    for( auto &role : filterParameterMap.keys() ) {

        qu.addBindValue( filterParameterMap.value( role ) );
    }

    if( !qu.exec() ) {
        qCWarning( phxLibrary ) << "LibraryModel::select() error: " << qu.lastError().text();
    }

    setQuery( qu );

    mFilterCollection = false;

    if( !qu.isActive() || lastError().isValid() ) {
        setTable( tableName() ); // resets the record & index
        endResetModel();
        return false;
    }

    endResetModel();
    return true;
}

bool LibraryModel::transaction() {
    if( !mTransaction ) {
        return database().transaction();
    }

    return true;
}

void LibraryModel::updateCount() {
    transaction();

    QSqlQuery query( database() );

    query.exec( QStringLiteral( "SELECT Count(*) FROM " ) + UserDatabase::tableName );

    while( query.next() ) {
        qmlCount = query.value( 0 ).toInt();
    }

    sync();

    emit countChanged();
}

void LibraryModel::setFilter( const QString table, const QString row, const QVariant value ) {

    auto tableRow = table + QStringLiteral( "." ) + row;

    filterParameterMap.insert( tableRow, value.toString() );

    QSqlTableModel::setFilter( createFilter() );
}

void LibraryModel::clearFilter( const QString table, const QString row ) {
    auto tableRow = table + QStringLiteral( "." ) + row;

    if( filterParameterMap.remove( tableRow ) == 0 ) {
        return;
    }

    setFilter( createFilter() );

}

void LibraryModel::sync() {
    if( !submitAll() ) {
        database().rollback();
    } else {
        database().commit();
    }

    mTransaction = false;

}

void LibraryModel::handleDraggedUrls( QList<QUrl> urls ) {
    emit draggedUrls( std::move( urls ) );
}

void LibraryModel::handleContainsDrag( const bool contains ) {
    emit containsDrag( std::move( contains ) );
}

void LibraryModel::handleDroppedUrls() {
    emit droppedUrls();
}

void LibraryModel::resumeInsert() {
    if( mGameScannerThread.isRunning() ) {
        mGameScanner.setInsertPaused( false );
    }

}

void LibraryModel::pauseInsert() {
    if( mGameScannerThread.isRunning() ) {
        mGameScanner.setInsertPaused( true );
    }
}

void LibraryModel::cancelInsert() {
    if( mGameScannerThread.isRunning() ) {
        mGameScanner.setInsertCancelled( true );
    }
}

void LibraryModel::stopGameScannerThread() {
    if( mGameScannerThread.isRunning() ) {
        mGameScanner.setInsertCancelled( true );
        mGameScannerThread.quit();
        mGameScannerThread.wait();
    }
}

bool LibraryModel::insertCancelled() {
    return mGameScanner.insertCancelled();
}

bool LibraryModel::insertPaused() {
    return mGameScanner.insertPaused();
}

void LibraryModel::handleUpdateGame( const GameData metaData ) {
    // We need to be careful here. This function needs to only set data
    // when the startMetaDataScan() function has finished.

    static int previousProgress = 0;

    transaction();

    static const auto updateDataStatement = QStringLiteral( "UPDATE " )
                                            + UserDatabase::tableName
                                            + QStringLiteral( " SET artworkUrl = ?" )
                                            + QStringLiteral( " WHERE crc32Checksum = ? " );

    QSqlQuery query( database() );

    query.prepare( updateDataStatement );

    query.addBindValue( metaData.artworkUrl );
    query.addBindValue( metaData.crc32Checksum );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Sql Update Error: " << query.lastError().text();
    }

    auto roundedProgress = static_cast<int>( metaData.importProgress );

    if( roundedProgress != previousProgress ) {
        previousProgress = roundedProgress;
        setProgress( roundedProgress );

        if( roundedProgress == 100 ) {
            previousProgress = 0;

            setProgress( roundedProgress );

            sync();

        }
    }

}

QString LibraryModel::selectStatement() const {
    static const auto collectionFilterStatement = QStringLiteral( "SELECT " )
            + UserDatabase::tableName
            + QStringLiteral( ".* FROM " )
            + UserDatabase::tableName
            + QStringLiteral( " INNER JOIN " )
            + UserDatabase::tableCollectionMappings
            + QStringLiteral( " ON " )
            + UserDatabase::tableName
            + QStringLiteral( ".rowIndex = " )
            + UserDatabase::tableCollectionMappings
            + QStringLiteral( ".rowIndex JOIN " )
            + UserDatabase::tableCollections
            + QStringLiteral( " ON " )
            + UserDatabase::tableCollections
            + QStringLiteral( ".collectionID = " )
            + UserDatabase::tableCollectionMappings
            + QStringLiteral( ".collectionID" );

    auto select = collectionFilterStatement + " WHERE " + filter();

    if( !mFilterCollection ) {
        return QSqlTableModel::selectStatement();
    }

    return select;
}

void LibraryModel::handleInsertGame( const GameData importData ) {

    static const auto statement = QStringLiteral( "INSERT INTO " )
                                  + UserDatabase::tableName
                                  + QStringLiteral( " (title, system, absoluteFilePath, timePlayed, crc32Checksum, artworkUrl) " )
                                  + QStringLiteral( "VALUES (?,?,?,?,?,?)" );


    if( insertCancelled() ) {
        setProgress( 0.0 );
        setMessage( QStringLiteral( "" ) );
        database().rollback();
        return;
    }

    if( importData.fileID == 0 ) {
        //beginInsertRows( QModelIndex(), count(), count() );
        transaction();
        setMessage( QStringLiteral( "Importing Games..." ) );
    }

    /*
    if( importData.fileID % 50 == 0 ) {
        qDebug() << "force sync";
        sync();
        transaction();
    }
    */


    mGameScanner.setResumeInsertID( importData.filePath );

    QSqlQuery query( database() );

    query.prepare( statement );
    query.addBindValue( importData.title );
    query.addBindValue( importData.system );
    query.addBindValue( importData.filePath );
    query.addBindValue( importData.timePlayed );
    query.addBindValue( importData.crc32Checksum );
    query.addBindValue( importData.artworkUrl );

    if( !query.exec() ) {
        qDebug() << "SQL Insertion Error: " << query.lastError().text() << query.lastQuery();
    }

    // Limit how many times the progress is updated, to reduce strain on the render thread.
    auto importProgress = static_cast<int>( importData.importProgress );

    if( importProgress  != static_cast<int>( progress() ) ) {
        setProgress( importProgress );
    }

    if( static_cast<int>( progress() ) == 100 ) {

        mGameScanner.setResumeInsertID( "" );
        mGameScanner.setResumeDirectory( "" );

        setProgress( 0.0 );
        setMessage( QStringLiteral( "Import Synced..." ) );

        sync();

        updateCount();

        //endInsertRows();

    }
}

void LibraryModel::setProgress( const qreal progress ) {
    if( progress == qmlProgress ) {
        return;
    }

    qmlProgress = progress;
    emit progressChanged();
}

QString LibraryModel::createFilter() {
    QString newFilter;

    for( auto &key : filterParameterMap.keys() ) {
        mFilterCollection = key.contains( QStringLiteral( "collections" ) );
        QString comparison;

        if( key == UserDatabase::tableName + QStringLiteral( ".title" ) ) {
            comparison = QStringLiteral( "LIKE ?" );
        } else if( key == UserDatabase::tableCollections + QStringLiteral( ".collectionID" )
                   || key == UserDatabase::tableName + QStringLiteral( ".system" ) ) {
            comparison = QStringLiteral( "= ?" );
        }

        if( newFilter.isEmpty() ) {
            newFilter += key + QStringLiteral( " " ) + comparison;
        }

        else {
            newFilter += QStringLiteral( " AND " ) + key + QStringLiteral( " " ) + comparison;
        }
    }

    return std::move( newFilter );
}

int LibraryModel::count() const {
    return qmlCount;
}

qreal LibraryModel::progress() const {
    return qmlProgress;
}

QVariantMap LibraryModel::get( int inx ) {
    QVariantMap map;

    foreach( int i, roleNames().keys() ) {
        map[roleNames().value( i )] = data( index( inx, 0 ), i );
    }

    return map;
}

bool LibraryModel::recursiveScan() const {
    return qmlRecursiveScan;
}

QString LibraryModel::message() const {
    return qmlMessage;
}

void LibraryModel::setRecursiveScan( const bool scan ) {
    qmlRecursiveScan = scan;
    emit recursiveScanChanged();
}

void LibraryModel::setMessage( const QString message ) {
    qmlMessage = message;
    emit messageChanged();
}

void LibraryModel::scanFolder( const QUrl url ) {

    auto localUrl = url.toLocalFile();

    if( mGameScanner.isRunning() ) {
        qDebug() << "Scan in already running. returning...";
        return;
    }

    bool autoStart = true;
    emit insertGames( std::move( localUrl ), autoStart );

}

void LibraryModel::clearDatabase() {
    if( mGameScanner.isRunning() ) {
        qCWarning( phxLibrary ) << "Cannot remove entries when scan is running.";
        return;
    }

    transaction();
    QSqlQuery query( database() );

    if( !query.exec( QStringLiteral( "DELETE FROM " ) + UserDatabase::tableName ) ) {
        qDebug() << "SQLITE Deletion Error: " << query.lastError().text();
        return;
    }

    setMessage( QStringLiteral( "Library Synced..." ) );

    sync();

    updateCount();
}
