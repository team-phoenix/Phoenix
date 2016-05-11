#pragma once

#include <QString>
#include <QtGlobal>

FILE *logFP = nullptr;

const QString phoenixCustomDebugOutputFormat = QStringLiteral(
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

// This is used to get the stack trace behind whatever debug message you want to diagnose
// Simply change the message string below to whatever you want (partial string matching) and set the breakpoint
void phoenixDebugMessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg ) {
    // Change this QString to reflect the message you want to get a stack trace for
    if( QString( msg ).contains( QStringLiteral( "Your message here!" ) ) ) {

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

// Alternate version, writes to file
void phoenixDebugMessageLog( QtMsgType type, const QMessageLogContext &context, const QString &msg ) {
    // Adapted from Qt 5.6.0 source
    QString logMessage = qFormatLogMessage( type, context, msg );

    // print nothing if message pattern didn't apply / was empty.
    // (still print empty lines, e.g. because message itself was empty)
    if( logMessage.isNull() ) {
        return;
    }

    if( msg.isEmpty() ) {
        fprintf( logFP, "\n" );
    } else {
        fprintf( logFP, "%s\n", logMessage.toLocal8Bit().constData() );
    }

    fflush( logFP );

    // Print to console too, just in case
    phoenixDebugMessageHandler( type, context, msg );
}
