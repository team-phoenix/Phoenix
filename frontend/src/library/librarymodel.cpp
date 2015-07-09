#include "librarymodel.h"
#include "logging.h"

#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QMutexLocker>
#include <QCryptographicHash>
#include <QSettings>

using namespace Library;

LibraryModel::LibraryModel( LibraryInternalDatabase &db, QObject *parent )
    : QSqlTableModel( parent, db.database() ),
      mMetaDataEmitted( false ),
      mScanFilesThread( this ),
      mGetMetadataThread( this ),
      mCancelScan( false ),
      lastUpdatedRowID( -1 ),
      qmlCount( 0 ),
      qmlRecursiveScan( true ),
      qmlProgress( 0.0 ) {

    for( auto &extension : platformMap.keys() ) {
        mFileFilter.append( "*." + extension );
    }

    mMetaDataDatabse.moveToThread( &mGetMetadataThread );

    mRoleNames = QSqlTableModel::roleNames();
    mRoleNames.insert( TitleRole, QByteArrayLiteral( "title" ) );
    mRoleNames.insert( SystemRole, QByteArrayLiteral( "system" ) );
    mRoleNames.insert( TimePlayedRole, QByteArrayLiteral( "timePlayed" ) );
    mRoleNames.insert( ArtworkRole, QByteArrayLiteral( "artworkUrl" ) );
    mRoleNames.insert( FileNameRole, QByteArrayLiteral( "fileName" ) );
    mRoleNames.insert( SystemPathRole, QByteArrayLiteral(  "systemPath" ) );
    mRoleNames.insert( RowIDRole, QByteArrayLiteral( "rowIndex" ) );

    setEditStrategy( QSqlTableModel::OnManualSubmit );
    setTable( LibraryInternalDatabase::tableName );
    select();

    connect( this, &LibraryModel::fileFound, this, &LibraryModel::handleFilesFound );
    connect( &mScanFilesThread, &QThread::started, this, &LibraryModel::findFiles, Qt::DirectConnection );
    connect( &mScanFilesThread, &QThread::started, this, [ this ] {
        setMessage( QStringLiteral( "Importing Games..." ) );
    } );
    connect( &mScanFilesThread, &QThread::finished, this, [ this ] {

        qCDebug( phxLibrary ) << "mScanFilesThread Stopped...";
        setCancelScan( false );
        mImportUrl.clear();
        setProgress( 0.0 );
        setMessage( QStringLiteral( "Finished..." ) );

    } );

    connect( this, &LibraryModel::cancelScanChanged, &mMetaDataDatabse, &MetaDataDatabase::setCancel );

    connect( &mGetMetadataThread, &QThread::started, this, [ this ] {
        setMessage( QStringLiteral( "Scraping Artwork..." ) );
    } );
    connect( &mGetMetadataThread, &QThread::finished, this, [ this ] {

        qCDebug( phxLibrary ) << "mGetMetadataThread Stopped...";
        setProgress( 0.0 );
        setCancelScan( false );
        setMessage( QStringLiteral( "Finished..." ) );

    } );

    connect( &mMetaDataDatabse, &MetaDataDatabase::updateMetadata, this, &LibraryModel::setMetadata );
    connect( this, &LibraryModel::signalFetchMetaData, &mMetaDataDatabse, &MetaDataDatabase::getMetadata );

}

LibraryModel::~LibraryModel() {

    cancel();

    if( mGetMetadataThread.isRunning() ) {

        mGetMetadataThread.wait();
    }

    if( mScanFilesThread.isRunning() ) {

        cancel();

        mScanFilesThread.wait();
    }

    sync();

    QSettings settings;
    settings.beginGroup( QStringLiteral( "Library" ) );
    settings.setValue( QStringLiteral( "LastRowIndex" ), lastUpdatedRowID );

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

    for( auto &val : params ) {
        qu.addBindValue( val );
    }

    qu.exec();

    setQuery( qu );

    if( !qu.isActive() || lastError().isValid() ) {
        setTable( tableName() ); // resets the record & index
        endResetModel();
        return false;
    }

    endResetModel();
    return true;
}

bool LibraryModel::cancelScan() {
    scanMutex.lock();
    auto cancel = mCancelScan;
    scanMutex.unlock();
    return cancel;
}

void LibraryModel::updateCount() {
    database().transaction();

    QSqlQuery query( database() );

    query.exec( QStringLiteral( "SELECT Count(*) FROM " ) + LibraryInternalDatabase::tableName );

    while( query.next() ) {
        qmlCount = query.value( 0 ).toInt();
    }

    emit countChanged();
}

void LibraryModel::setFilter( QString filter, QVariantList params, bool preserveCurrentFilter ) {
    Q_UNUSED( preserveCurrentFilter );

    /*
        if( preserveCurrentFilter && !this->filter().isEmpty() ) {
            filter = this->filter() + " AND " + filter;
            this->params.append( params );
        }*/

    //else {
    this->params = params;
    //}

    QSqlTableModel::setFilter( filter );
}

void LibraryModel::cancel() {

    if( mGetMetadataThread.isRunning() ) {
        setCancelScan( true );
        mMetaDataDatabse.setCancel( true );
    }

    if( mScanFilesThread.isRunning() ) {
        setCancelScan( true );
    }

}

void LibraryModel::sync() {
    if( submitAll() ) {
        database().commit();
    }

    else {
        database().rollback();
    }

}

void LibraryModel::updateUknownMetadata() {
    if( !mGetMetadataThread.isRunning() ) {
        mGetMetadataThread.start( QThread::NormalPriority );
        qCDebug( phxLibrary ) << "mGetMetadataThread Starting...";
    }


    static const QString fetchCheckSumStatement = QStringLiteral( "SELECT filename, rowIndex FROM " )
                                                + LibraryInternalDatabase::tableName
                                                + QStringLiteral( " WHERE artworkUrl = NULL" );
    database().transaction();

    QSqlQuery query( database() );

    if( !query.exec( fetchCheckSumStatement ) ) {
        qCWarning( phxLibrary ) << "startMetaDataScan() error: "
                                << query.lastError().text();
        return;
    }

    int i = 1;

    while( query.next() ) {

        GameMetaData metaData;
        metaData.filePath = query.value( 0 ).toString();
        metaData.rowIndex = query.value( 1 ).toInt();
        metaData.updated = false;
        metaData.progress = ( i / static_cast<qreal>( count() ) ) * 100.0;
        emit signalFetchMetaData( std::move( metaData ) );

        ++i;
    }

    if( i == 1 ) {
        mGetMetadataThread.quit();
    }

}

void LibraryModel::resumeMetadataScan() {
    QSettings settings;
    settings.beginGroup( QStringLiteral( "Library" ) );
    auto startingRow = settings.value( QStringLiteral( "LastRowIndex" ) );

    if( !startingRow.isValid() || startingRow.toInt() == -1 ) {
        return;
    }

    static const QString countRows = QStringLiteral( "SELECT COUNT(*) FROM " )
                                   + LibraryInternalDatabase::tableName
                                   + QStringLiteral( " WHERE rowIndex > ?" );
    database().transaction();

    QSqlQuery query( database() );

    query.prepare( countRows );
    query.addBindValue( startingRow );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "startMetaDataScan() error: "
                                << query.lastError().text();
        return;
    }

    int rowCount = -1;

    if( query.first() ) {
        rowCount = query.value( 0 ).toInt();
    }

    if( rowCount == -1 ) {
        return;
    }

    query.clear();

    if( !mGetMetadataThread.isRunning() ) {
        mGetMetadataThread.start( QThread::NormalPriority );
        qCDebug( phxLibrary ) << "mGetMetadataThread Starting...";
    }

    static const QString resumeMetadataStatement = QStringLiteral( "SELECT filename, rowIndex FROM " )
                                                 + LibraryInternalDatabase::tableName
                                                 + QStringLiteral( " WHERE rowIndex > ?" );

    query.prepare( resumeMetadataStatement );
    query.addBindValue( startingRow );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "startMetaDataScan() error: "
                                << query.lastError().text();
        return;
    }

    int i = 1;

    while( query.next() ) {

        GameMetaData metaData;
        metaData.filePath = query.value( 0 ).toString();
        metaData.rowIndex = query.value( 1 ).toInt();
        metaData.updated = false;
        metaData.progress = ( i / static_cast<qreal>( rowCount ) ) * 100.0;

        emit signalFetchMetaData( std::move( metaData ) );
        ++i;

    }

    if( i == 1 ) {
        mGetMetadataThread.quit();
    }


}


void LibraryModel::startMetaDataScan() {
    if( !mGetMetadataThread.isRunning() ) {
        mGetMetadataThread.start( QThread::NormalPriority );
        qCDebug( phxLibrary ) << "mGetMetadataThread Starting...";
    }


    static const auto fetchCheckSumStatement = QStringLiteral( "SELECT filename, rowIndex FROM " )
                                                + LibraryInternalDatabase::tableName;
    database().transaction();

    QSqlQuery query( database() );

    if( !query.exec( fetchCheckSumStatement ) ) {
        qCWarning( phxLibrary ) << "startMetaDataScan() error: "
                                <<  query.lastError().text();
        return;
    }

    int i = 1;

    while( query.next() ) {

        GameMetaData metaData;
        metaData.filePath = query.value( 0 ).toString();
        metaData.rowIndex = query.value( 1 ).toInt();
        metaData.updated = false;
        metaData.progress = ( i / static_cast<qreal>( count() ) ) * 100.0;
        emit signalFetchMetaData( std::move( metaData ) );

        ++i;
    }

    if( i == 1 ) {
        mGetMetadataThread.quit();
    }
}

void LibraryModel::setMetadata( const GameMetaData metaData ) {
    // We need to be careful here. This function needs to only set data
    // when the startMetaDataScan() function has finished.

    static int i = 0;
    static int previousProgress = 0;

    if( i == 0 ) {
        database().transaction();
        i = 1;
    }

    if( cancelScan() ) {
        qDebug() << "cancel Scan";
    }

    static const auto updateDataStatement = QStringLiteral( "UPDATE " )
                                             + LibraryInternalDatabase::tableName
                                             + QStringLiteral( " SET artworkUrl = ?, sha1 = ?" )
                                             + QStringLiteral( " WHERE rowIndex = ? " );


    if( metaData.updated ) {

        QSqlQuery query( database() );

        query.prepare( updateDataStatement );

        //qDebug() << metaData.rowIndex;

        query.addBindValue( metaData.artworkUrl );
        query.addBindValue( metaData.sha1 );
        query.addBindValue( metaData.rowIndex );

        if( !query.exec() ) {
            qCWarning( phxLibrary ) << "Sql Update Error: " << query.lastError().text();
            return;
        }
    }

    auto roundedProgress = static_cast<int>( metaData.progress );

    if( roundedProgress != previousProgress ) {
        previousProgress = roundedProgress;
        setProgress( roundedProgress );

        lastUpdatedRowID = metaData.rowIndex;

        if( roundedProgress == 100 ) {

            lastUpdatedRowID = -1;
            setProgress( roundedProgress );

            sync();

            mGetMetadataThread.quit();

        }
    }

}

void LibraryModel::handleFilesFound( const GameImportData importData ) {

    static const auto statement = QStringLiteral( "INSERT INTO " )
                                + LibraryInternalDatabase::tableName
                                + QStringLiteral( " (title, system, fileName, timePlayed) " )
                                + QStringLiteral( "VALUES (?,?,?,?)" );

    if( cancelScan() ) {
        return;
    }

    static int count = 0;

    if( count == 0 ) {
        //beginInsertRows( QModelIndex(), count(), count() );

        database().transaction();
        count = 1;
    }

    QSqlQuery query( database() );

    query.prepare( statement );
    query.addBindValue( importData.title );
    query.addBindValue( importData.system );
    query.addBindValue( importData.filePath );
    query.addBindValue( importData.timePlayed );

    if( !query.exec() ) {
        qDebug() << query.lastError().text();
    }

    // Limit how many times the progress is updated, to reduce strain on the render thread.
    auto importProgress = static_cast<int>( importData.importProgress );

    if( importProgress  != static_cast<int>( progress() ) ) {
        setProgress( importProgress );
    }

    if( static_cast<int>( progress() ) == 100 ) {

        sync();

        updateCount();

        startMetaDataScan();

        //endInsertRows();

    }
}

bool LibraryModel::getCueFileInfo( QFileInfo &fileInfo ) {
    QFile file( fileInfo.canonicalFilePath() );

    if( !file.open( QIODevice::ReadOnly ) ) {
        return false;
    }

    while( !file.atEnd() ) {
        auto line = file.readLine();
        QList<QByteArray> splitLine = line.split( ' ' );

        if( splitLine.first().toUpper() == "FILE" ) {
            QString baseName;

            for( int i = 1; i < splitLine.size() - 1; ++i ) {

                auto bytes = splitLine.at( i );

                bytes = bytes.replace( "\"", "" );

                if( i == splitLine.size() - 2 ) {
                    baseName += bytes;
                } else {
                    baseName += bytes + ' ';
                }
            }

            if( baseName.isEmpty() ) {
                return false;
            }

            fileInfo.setFile( fileInfo.canonicalPath() + QDir::separator() + baseName );
            break;

        }
    }

    file.close();

    return true;
}

void LibraryModel::checkHeaderOffsets( const QFileInfo &fileInfo, QString &platform ) {
    QFile file( fileInfo.canonicalFilePath() );

    if( file.open( QIODevice::ReadOnly ) ) {

        auto headers = headerOffsets.value( fileInfo.suffix() );

        for( auto &header : headers ) {
            if( !file.seek( header.offset ) ) {
                continue;
            }

            auto bytes = file.read( header.length );

            platform = platformForHeaderString( bytes.simplified().toHex() );

            if( !platform.isEmpty() ) {
                break;
            }

        }

        file.close();
    }
}

void LibraryModel::findFiles() {
    auto localUrl = mImportUrl.toLocalFile();

    QDir urlDirectory( localUrl );

    if( !urlDirectory.exists() ) {
        qCWarning( phxLibrary ) << localUrl << " does not exist!";
        return;
    }

    auto fileInfoList = urlDirectory.entryInfoList( mFileFilter, QDir::Files, QDir::NoSort );

    if( fileInfoList.size() == 0 ) {
        qCWarning( phxLibrary ) << "No files were found";
    }

    int i = 0;

    for( auto &fileInfo : fileInfoList ) {

        if( cancelScan() ) {
            mScanFilesThread.exit();
            qCDebug( phxLibrary ) << "Canceled import";
            return;
        }

        auto extension = fileInfo.suffix();

        // QFileInfo::baseName() seems to split the absolutePath based on periods '.'
        // This causes issue with some game names that use periods.
        auto title =  fileInfo.absoluteFilePath().remove(
                          fileInfo.canonicalPath() ).remove( 0, 1 ).remove( "." + extension );
        auto absoluteFilePath = fileInfo.canonicalFilePath();

        // We need to check for .cue files specifically, since these files are text files.
        if( extension == "cue" ) {
            if( !getCueFileInfo( fileInfo ) ) {
                // Can't import a wonky cue file. Just skip it.
                // We should be telling the use that this cue file has an error.
                qCWarning( phxLibrary ) << fileInfo.canonicalFilePath()
                                        << " is isn't a valid cue file. Skipping...";
                continue;
            }
        }

        // ####################################################################
        //                             WARNING!!!
        // ####################################################################

        // Every call to fileInfo after this point isn't guaranteed to have the
        // same values.

        // Make copies of the fileInfo data before this point, unless you want the
        // cue file check to override them.

        auto system = platformMap.value( extension, "" );

        // System should only be empty on ambiguous files, such as ISO's and BINS.
        if( system.isEmpty() ) {

            checkHeaderOffsets( fileInfo, system );

            if( system.isEmpty() ) {
                qCWarning( phxLibrary ) << "The system is 'still' empty, for"
                                        << fileInfo.canonicalFilePath();

                qCWarning( phxLibrary ) << "this means we need to add"
                                        << "in better header and offset checking.";
            }
        }

        GameImportData importData;
        importData.timePlayed = QStringLiteral( "00:00" );
        importData.title = title;
        importData.filePath = absoluteFilePath;
        importData.importProgress = ( ( i + 1 )
                                      / static_cast<qreal>( fileInfoList.size() ) ) * 100.0;
        importData.system = system;

        emit fileFound( std::move( importData ) );

        ++i;

    }

    mScanFilesThread.quit();

}

void LibraryModel::setProgress( const qreal progress ) {
    if( progress == qmlProgress ) {
        return;
    }

    qmlProgress = progress;
    emit progressChanged();
}

void LibraryModel::setCancelScan( const bool scan ) {
    scanMutex.lock();
    mCancelScan = scan;
    scanMutex.unlock();

    emit cancelScanChanged( scan );
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


bool LibraryModel::remove( int row, int count ) {
    Q_UNUSED( count )

    if( mScanFilesThread.isRunning() || mGetMetadataThread.isRunning() ) {
        qCWarning( phxLibrary ) << "Cannot remove entries when scan is running.";
        return false;
    }

    //beginRemoveRows( QModelIndex(), row, row + count );

    database().transaction();
    QSqlQuery query( database() );

    query.prepare( QStringLiteral( "DELETE FROM " )
                   + LibraryInternalDatabase::tableName
                   + QStringLiteral( " WHERE systemID = ?" ) );

    query.addBindValue( row );

    if( !query.exec() ) {
        qDebug() << query.lastError().text();
        return false;
    }

    // If you want to cache changed, don't commit this.
    //

    sync();


    //endRemoveRows();

    updateCount();

    return true;

}

void LibraryModel::append( const QUrl url ) {

    if( mScanFilesThread.isRunning() || mGetMetadataThread.isRunning() ) {
        qDebug() << "Scan in already running. returning...";
        return;
    }

    mImportUrl = url;
    mScanFilesThread.start( QThread::HighPriority );

}

void LibraryModel::clear() {
    if( mScanFilesThread.isRunning() || mGetMetadataThread.isRunning() ) {
        qCWarning( phxLibrary ) << "Cannot remove entries when scan is running.";
        return;
    }

    database().transaction();

    QSqlQuery query( database() );

    if( !query.exec( QStringLiteral( "DELETE FROM " ) + LibraryInternalDatabase::tableName ) ) {
        qDebug() << "SQLITE Deletion Error: " << query.lastError().text();
        return;
    }

    setMessage( QStringLiteral( "Library Synced..." ) );

    sync();

    updateCount();
}
