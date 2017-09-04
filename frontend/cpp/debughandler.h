#pragma once

#include "frontendcommon.h"

static QString logFilePath;
static QFile logFile;
static QMutex mutex;

namespace DebugHandler {

    void install( QtMessageHandler handler ) {
        qInstallMessageHandler( handler );
    }

    void install( const QString &filePath, QtMessageHandler handler ) {
        if ( !logFile.isOpen() ) {
            logFilePath = filePath;
            logFile.setFileName( filePath );
            logFile.open( QIODevice::ReadWrite | QIODevice::Text );
            install( handler );
        }
    }

    void archive() {

        // The file is positioned at the end, we must return it to the start to read from it.
        logFile.seek( 0 );
        QFileInfo info( logFilePath );

        KZip zip( info.canonicalPath() + "/" + info.baseName() + ".zip" );
        zip.open( QIODevice::WriteOnly );
        zip.writeFile( info.baseName() + ".log", logFile.readAll(), 0100644, QStringLiteral("owners"), QStringLiteral("users"));

        // Must remove the old file, or we are just wasting space.
        logFile.remove();
    }

    void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
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

    void messageLog(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        QMutexLocker locker( &mutex );

        // Adapted from Qt 5.6.0 source
        QString logMessage = qFormatLogMessage( type, context, msg );

        // print nothing if message pattern didn't apply / was empty.
        // (still print empty lines, e.g. because message itself was empty)
        if( logMessage.isNull() ) {
            return;
        }

        if( !msg.isEmpty() ) {
            logFile.write( logMessage.toLocal8Bit() );
        }

        logFile.write( "\n" );
        logFile.flush();

        // We need to manually unlock, because 'messageHandler' relocks the mutex.
        locker.unlock();

        // Print to console too, just in case
        messageHandler( type, context, msg );
    }

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

}
