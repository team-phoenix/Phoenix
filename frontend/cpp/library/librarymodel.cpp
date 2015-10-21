#include "librarymodel.h"

using namespace Library;

LibraryModel::LibraryModel( QObject *parent )
    : LibraryModel( LibraryInternalDatabase::instance(), parent ) {

}

LibraryModel::LibraryModel( LibraryInternalDatabase *db, QObject *parent )
    : LibraryModel( *db, parent ) {

}

LibraryModel::LibraryModel( LibraryInternalDatabase &db, QObject *parent )
    : QSqlTableModel( parent, db.database() ),
      mWorkerThread( this ),
      mFilterCollection( false ),
      mTransaction( false ),
      qmlInsertPaused( false ),
      qmlInsertCancelled( false ),
      qmlCount( 0 ),
      qmlRecursiveScan( true ),
      qmlProgress( 0.0 ) {

    mLibraryWorker.moveToThread( &mWorkerThread );

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
    setTable( LibraryInternalDatabase::tableName );
    select();


    // Connect Model to Worker.
    connect( this, &LibraryModel::droppedUrls, &mLibraryWorker, &LibraryWorker::handleDroppedUrls );
    connect( this, &LibraryModel::containsDrag, &mLibraryWorker, &LibraryWorker::handleContainsDrag );
    connect( this, &LibraryModel::draggedUrls, &mLibraryWorker, &LibraryWorker::handleDraggedUrls );
    connect( this, &LibraryModel::insertGames, &mLibraryWorker, &LibraryWorker::findGameFiles );
    connect( this, &LibraryModel::signalInsertCancelled, &mLibraryWorker, &LibraryWorker::setInsertCancelled );
    connect( this, &LibraryModel::signalInsertPaused, &mLibraryWorker, &LibraryWorker::setInsertPaused );

    // Connect Worker to Model.

    // Do not change this from a blocking queued connection. This is to simplify the threaded code.
    connect( &mLibraryWorker, &LibraryWorker::insertGameData, this, &LibraryModel::handleInsertGame );
    connect( &mLibraryWorker, &LibraryWorker::started, this, [] {
        qCDebug( phxLibrary ) << "Worker Started...";
    } );

    // Do some thread cleanup.
    connect( &mLibraryWorker, &LibraryWorker::finished, this, [ this ] {
        qCDebug( phxLibrary ) << "Worker Finished...";
    } );


    // Listen to the Worker Thread.
    connect( &mWorkerThread, &QThread::started, &mLibraryWorker, &LibraryWorker::eventLoopStarted );
    connect( &mWorkerThread, &QThread::started, this, [ this ] {
        qCDebug( phxLibrary ) << "mWorkerThread Started...";
    } );


    connect( &mWorkerThread, &QThread::finished, this, [ this ] {
        qCDebug( phxLibrary ) << "mWorkerThread Stopped...";
    } );

}

LibraryModel::~LibraryModel() {

    closeWorkerThread();

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

    query.exec( QStringLiteral( "SELECT Count(*) FROM " ) + LibraryInternalDatabase::tableName );

    while( query.next() ) {
        qmlCount = query.value( 0 ).toInt();
    }

    mTransaction = false;

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
    if( mWorkerThread.isRunning() ) {
        mLibraryWorker.setInsertPaused( false );
    }

}

void LibraryModel::pauseInsert() {
    if( mWorkerThread.isRunning() ) {
        mLibraryWorker.setInsertPaused( true );
    }
}

void LibraryModel::cancelInsert() {
    if( mWorkerThread.isRunning() ) {
        mLibraryWorker.setInsertCancelled( true );
    }
}

void LibraryModel::closeWorkerThread() {
    if( mWorkerThread.isRunning() ) {
        mLibraryWorker.setInsertCancelled( true );
        mWorkerThread.quit();
        mWorkerThread.wait();
    }
}

bool LibraryModel::insertCancelled() {
    return mLibraryWorker.insertCancelled();
}

bool LibraryModel::insertPaused() {
    return mLibraryWorker.insertPaused();
}


void LibraryModel::handleUpdateGame( const GameData metaData ) {
    // We need to be careful here. This function needs to only set data
    // when the startMetaDataScan() function has finished.

    static int previousProgress = 0;

    transaction();

    static const auto updateDataStatement = QStringLiteral( "UPDATE " )
                                            + LibraryInternalDatabase::tableName
                                            + QStringLiteral( " SET artworkUrl = ?" )
                                            + QStringLiteral( " WHERE crc32Checksum = ? " );

    QSqlQuery query( database() );

    query.prepare( updateDataStatement );

    query.addBindValue( metaData.artworkUrl );
    query.addBindValue( metaData.crc32Checksum);

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
            + LibraryInternalDatabase::tableName
            + QStringLiteral( ".* FROM " )
            + LibraryInternalDatabase::tableName
            + QStringLiteral( " INNER JOIN " )
            + LibraryInternalDatabase::tableCollectionMappings
            + QStringLiteral( " ON " )
            + LibraryInternalDatabase::tableName
            + QStringLiteral( ".rowIndex = " )
            + LibraryInternalDatabase::tableCollectionMappings
            + QStringLiteral( ".rowIndex JOIN " )
            + LibraryInternalDatabase::tableCollections
            + QStringLiteral( " ON " )
            + LibraryInternalDatabase::tableCollections
            + QStringLiteral( ".collectionID = " )
            + LibraryInternalDatabase::tableCollectionMappings
            + QStringLiteral( ".collectionID" );

    auto select = collectionFilterStatement + " WHERE " + filter();

    if( !mFilterCollection ) {
        return QSqlTableModel::selectStatement();
    }

    return select;
}

void LibraryModel::handleInsertGame( const GameData importData ) {

    static const auto statement = QStringLiteral( "INSERT INTO " )
                                  + LibraryInternalDatabase::tableName
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


    mLibraryWorker.setResumeInsertID( importData.filePath );

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

        mLibraryWorker.setResumeInsertID( "" );
        mLibraryWorker.setResumeDirectory( "" );

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

        if( key == LibraryInternalDatabase::tableName + QStringLiteral( ".title" ) ) {
            comparison = QStringLiteral( "LIKE ?" );
        } else if( key == LibraryInternalDatabase::tableCollections + QStringLiteral( ".collectionID" )
                   || key == LibraryInternalDatabase::tableName + QStringLiteral( ".system" ) ) {
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

void LibraryModel::append( const QUrl url ) {

    auto localUrl = url.toLocalFile();

    if( mLibraryWorker.isRunning() ) {
        qDebug() << "Scan in already running. returning...";
        return;
    }

    bool autoStart = true;
    emit insertGames( std::move( localUrl ), autoStart );

}

void LibraryModel::clearDatabase() {
    if( mLibraryWorker.isRunning() ) {
        qCWarning( phxLibrary ) << "Cannot remove entries when scan is running.";
        return;
    }

    transaction();
    QSqlQuery query( database() );

    if( !query.exec( QStringLiteral( "DELETE FROM " ) + LibraryInternalDatabase::tableName ) ) {
        qDebug() << "SQLITE Deletion Error: " << query.lastError().text();
        return;
    }

    setMessage( QStringLiteral( "Library Synced..." ) );

    sync();

    updateCount();
}
