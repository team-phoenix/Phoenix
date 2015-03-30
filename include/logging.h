
#ifndef LOGGING_H
#define LOGGING_H

#include <QLoggingCategory>

/* This is used for debugging Phoenix. Instead of using qDebug(), developers should use
 * qCDebug(%category%), such as qCDebug(phxLibrary). The category used for debugging should
 * be relevant to whatever class is being worked on.
 *
 * If none of the categories below accurately represent the type of class that you are working in,
 * please feel free to add a new category that fits the case.
 */

Q_DECLARE_LOGGING_CATEGORY( phxCore )
Q_DECLARE_LOGGING_CATEGORY( phxVideo )
Q_DECLARE_LOGGING_CATEGORY( phxAudio )
Q_DECLARE_LOGGING_CATEGORY( phxInput )
Q_DECLARE_LOGGING_CATEGORY( phxLibrary )

#endif
