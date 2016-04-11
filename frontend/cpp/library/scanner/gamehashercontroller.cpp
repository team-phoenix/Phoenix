#include "gamehashercontroller.h"

#include <QThread>

GameHasherController::GameHasherController( QObject *parent ) : QObject( parent ),
    gameHasher( new GameHasher() ),
    gameHasherThread( new QThread() ),
    mProgress( 0 ),
    mRunning( false ),
    mPaused( false )
{
    // Set up GameHasher
    gameHasher->setObjectName( "GameHasher" );
    gameHasher->moveToThread( gameHasherThread );

    connect( gameHasherThread, &QThread::finished, gameHasher, &QObject::deleteLater );
    connect( this, &GameHasherController::shutdownGameHasher, gameHasher, &GameHasher::shutdown );
    connect( gameHasher, &GameHasher::scanCompleted, this, &GameHasherController::scanCompleted );
    connect( gameHasher, &GameHasher::filesNeedAssignment, this, &GameHasherController::filesNeedAssignment );
    connect( gameHasher, &GameHasher::progressChanged, this, &GameHasherController::setProgress );

    // Used for testing the manual add mode.
    //connect( gameHasher, &GameHasher::scanCompleted, this, &GameHasherController::filesNeedAssignment );

    connect( gameHasher, &GameHasher::scanCompleted, this, [ = ] ( FileList results ) {
        qCDebug( phxLibrary ) << "scanCompleted from GameHasher," << results.size();
    });


    gameHasherThread->setObjectName( "Game hasher thread" );
    gameHasherThread->start();

    // Ensure gameHasher safely shuts down on user-invoked quit
    connect( QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [ = ]() {
        qDebug() << "";
        qCInfo( phxLibrary ) << ">>>>>>>> User requested app to close, shutting down GameHasher (waiting up to 30 seconds)...";
        qDebug() << "";

        // Shut down coreControl (calls gameHasherThread->exit() too)
        emit shutdownGameHasher();

        // Shut down thread, block until it finishes
        gameHasherThread->wait( 30 * 1000 );
        gameHasherThread->deleteLater();

        qDebug() << "";
        qCInfo( phxLibrary ) << ">>>>>>>> Fully unloaded!";
        qDebug() << "";
    } );
}

int GameHasherController::progress() const
{
    return mProgress;
}

bool GameHasherController::running() const
{
    return mRunning;
}

bool GameHasherController::paused() const
{
    return mPaused;
}

void GameHasherController::setRunning(const bool running) {
    mRunning = running;
    emit runningChanged();
}

void GameHasherController::setPaused(const bool paused) {
    mPaused = paused;
    emit pausedChanged();
}

void GameHasherController::scanForGames( QList<QUrl> urls ) {
    QStringList paths;

    for( QUrl url : urls ) {
        paths.append( url.toLocalFile() );
    }

    qCDebug( phxLibrary ) << "Sending" << urls.size() << "paths to GameHasher";
    setRunning( true );
    QMetaObject::invokeMethod( gameHasher, "addPaths", Qt::QueuedConnection, Q_ARG( QStringList, paths ) );
}

void GameHasherController::processResults( FileList results ) {
    Q_UNUSED( results )
    // FIXME: Remove once GUI is up and running

    // Database is assumed to have already been opened by main/qml thread by the SqlModel
    // used in BoxartGridView.qml (defined in ContentArea.qml)
    QSqlDatabase libretroDB = QSqlDatabase::database( "USERDATA" );

    static const QString statement = QStringLiteral( "INSERT INTO " )
                                     % UserDatabase::tableName
                                     % QStringLiteral( " (title, system, absoluteFilePath, timePlayed, crc32Checksum, artworkUrl) " )
                                     % QStringLiteral( "VALUES (?,?,?,?,?,?)" );

    FileList rejects;

    for( FileEntry result : results ) {
        // Only import exact matches directly
        if( !( result.scannerResult == GameScannerResult::GameUUIDByHash )
            || ( result.systemUUIDs.size() != 1 ) ) {
            rejects.append( result );
            continue;
        }

        QSqlQuery query( libretroDB );

        query.prepare( statement );
        query.addBindValue( result.gameMetadata.title );
        query.addBindValue( result.systemUUIDs.first() );
        query.addBindValue( result.filePath );
        query.addBindValue( "0:00" );
        query.addBindValue( result.crc32 );
        query.addBindValue( result.gameMetadata.frontArtwork );

        if( !query.exec() ) {
            qDebug( phxLibrary ) << "SQL Insertion Error: " << query.lastError().text() << query.lastQuery();
        }
    }
}

void GameHasherController::setProgress( const int progress ) {
    mProgress = progress;
    if ( progress == 500 ) {
        setRunning( false );
    }
    emit progressChanged();
}

void GameHasherController::pause() {
    QMetaObject::invokeMethod( gameHasher, "pause" );
    setPaused( true );
}

void GameHasherController::resume() {
    QMetaObject::invokeMethod( gameHasher, "resume" );
    setPaused( false );
}

void GameHasherController::cancel() {
    QMetaObject::invokeMethod( gameHasher, "cancel" );
    setProgress( 0 );
    setRunning( false );
}

QObject *GameHasherControllerSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine ) {
    Q_UNUSED( scriptEngine )

    GameHasherController *gameHasherController = new GameHasherController( engine );

    return gameHasherController;
}
