#include "gamescancontroller.h"
#include "gamefileinfo.h"

#include <QtCore>
#include <QtConcurrent>

using namespace Library;

GameScanController::GameScanController(QObject *parent) : QObject(parent)
{
    connect( this, &GameScanController::error, this, &GameScanController::handleError );
    connect( this, &GameScanController::threadSafeStartScan, this, &GameScanController::handleScanStarted );
}

void GameScanController::appendScanPath(const QString scanPath)
{
    mScanPathList.append( scanPath );
}

QStringList GameScanController::scanPath() const
{
    return mScanPathList;
}

void GameScanController::startScan()
{
    emit threadSafeStartScan();
}

void GameScanController::handleError(const GameScanController::ErrorCode error) {
    switch ( error ) {
        case ErrorCode::PathError:
            qCDebug( phxLibrary ) << "Import path does not exist";
            break;
        case ErrorCode::TypeError:
            qCDebug( phxLibrary ) << "Import path is a file.";
            break;
        default:
            Q_UNREACHABLE();
            break;
    }
}

void GameScanController::handleScanStarted()
{
    // Check that the directory exists before continuing
    QDir directory( path );

    QFileInfo dir( path );

    if( dir.isFile() ) {
        emit error( ErrorCode::TypeError );
        return;
    }

    if( !directory.exists() ) {
        emit error( ErrorCode::PathError );
        return;
    }

    QDirIterator dirIter( path, GameFileInfo::gameFilter(), QDir::Files, QDirIterator::NoIteratorFlags );
    QStringList fileList;

    while ( dirIter.hasNext() ) {
        QString filePath = dirIter.next();
        fileList.append( filePath );
    }

    QFuture<QStringList> future;
    QFutureWatcher<QStringList> watcher;
}

