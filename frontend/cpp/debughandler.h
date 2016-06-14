#pragma once

#pragma once

#include <QString>
#include <QtGlobal>
#include <QMutex>
#include <QMutexLocker>
#include <QFileInfo>

#include <memory>

#include <quazip.h>
#include <quazipfile.h>

static QMutex mutex;
static std::unique_ptr<QuaZip> zipArchive;
static std::unique_ptr<QuaZipFile> logZipFile;

namespace DebugHandler {

    QString messageFormat() {
        return QStringLiteral(
                    "%{if-debug}D %{endif}"
                    "%{if-info}I %{endif}"
                    "%{if-warning}W %{endif}"
                    "%{if-critical}C %{endif}"
                    "%{if-fatal}F %{endif}"
                    "%{time hh:mm:ss:zzz} %{function}():%{line}: %{message} "
                    //"%{if-debug}\n               [%{file}:%{line} %{function}()]"/*\n%{backtrace depth=20 separator=\"\n\"}*/"%{endif}"
                    //"%{if-info}\n               [%{file}:%{line} %{function}()]"/*\n%{backtrace depth=20 separator=\"\n\"}*/"%{endif}"
                    "%{if-warning}\n               [%{file}:%{line} %{function}()]"/*\n%{backtrace depth=20 separator=\"\n\"}*/"%{endif}"
                                                                                                                               "%{if-critical}\n               [%{file}:%{line} %{function}()]"/*\n%{backtrace depth=20 separator=\"\n\"}*/"%{endif}"
                                                                                                                                                                                                                                           "%{if-fatal}\n               [%{file}:%{line} %{function}()]"/*\n%{backtrace depth=20 separator=\"\n\"}*/"%{endif}"
        );
    }

    void install(QtMessageHandler handler) {
        qInstallMessageHandler( handler );
    }

    void install(const QString &logFile, QtMessageHandler handler) {
        if ( !zipArchive ) {
            zipArchive = std::make_unique<QuaZip>( logFile );
            zipArchive->open( QuaZip::Mode::mdCreate );


            logZipFile = std::make_unique<QuaZipFile>( zipArchive.get() );
            logZipFile->open( QuaZipFile::WriteOnly, QuaZipNewInfo( QFileInfo( logFile ).baseName() + ".log" ) );
        }

        install( handler );
    }

    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QMutexLocker locker( &mutex );
        Q_UNUSED( locker );

        // Change this QString to reflect the message you want to get a stack trace for
        if( QString( msg ).contains( QStringLiteral( "No file name specified" ) ) ) {

            // Put a breakpoint over this line...
            int breakPointOnThisLine( 0 );

            // ...and not this one
            Q_UNUSED( breakPointOnThisLine );

        }

        // Adapted from Qt 5.6.0 source
        QString logMessage = qFormatLogMessage( type, context, msg );

        // print nothing if message pattern didn't apply / was empty.
        // (still print empty lines, e.g. because message itself was empty)
        if( logMessage.isNull() ) {
            return;
        }

        if( msg.isEmpty() ) {
            fprintf( stdout, "\n" );
        } else {
            fprintf( stdout, "%s\n", logMessage.toLocal8Bit().constData() );
        }

        fflush( stdout );
    }

    void messageLog(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        QMutexLocker locker( &mutex );

        // Adapted from Qt 5.6.0 source
        QString logMessage = qFormatLogMessage( type, context, msg );

        // print nothing if message pattern didn't apply / was empty.
        // (still print empty lines, e.g. because message itself was empty)
        if( logMessage.isNull() ) {
            return;
        }

        if( msg.isEmpty() ) {
            logZipFile->write( QByteArray( "\n" ) );
        } else {
            logZipFile->write( logMessage.toLocal8Bit() );
            logZipFile->write( QByteArray( "\n" ) );
        }

        // We need to manually unlock, because 'messageHandler' relocks the mutex.
        locker.unlock();

        // Print to console too, just in case
        messageHandler( type, context, msg );
    }
}


