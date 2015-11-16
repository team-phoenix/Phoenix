#ifndef GAMESCANCONTROLLER_H
#define GAMESCANCONTROLLER_H

#include <QObject>
#include <QtConcurrent>

class GameScanController : public QObject
{
    Q_OBJECT
public:
    explicit GameScanController( QObject *parent = 0 );

    // Error code that could be emitted by "handleScanStarted()".
    // Used in the "error( const ErrorCode )" signal.
    enum ErrorCode {
        PathError = 0, // Path could not be found
        TypeError = 1, // Tried to use a file as a folder.
    };

    void appendScanPath( const QString scanPath );
    QStringList scanPath() const;

signals:
    void threadSafeStartScan();

    // emitted when the scanning process is completely done
    void scanFinished();

    // emitted when the scanning process has first started,
    // before any actual work has been completed.
    void scanStarted();

    // Is used to check whether any error has occured in the scanning process.
    void error( const ErrorCode errorCode );

    void progress( int progress );

public slots:
    // Starts the scanning process in a thread safe manner.
    // Calls "emit threadSafeStartScan()".
    virtual void startScan();

private slots:

    // Connects to the "error( const ErrorCode ) signal
    // and gives a read out of what each error means.
    void handleError( const ErrorCode error );

    // Connects to the signal "threadSafeStartScan()" in order to
    // do the actual scanning work.
    void handleScanStarted();

private:
    QStringList mScanPathList;

};

#endif // GAMESCANCONTROLLER_H
