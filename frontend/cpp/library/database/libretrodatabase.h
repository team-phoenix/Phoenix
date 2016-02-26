#ifndef SYSTEMDATABASE_H
#define SYSTEMDATABASE_H

#include "frontendcommon.h"
#include "phxpaths.h"
#include "logging.h"

#include <QMutex>

/*
 *
 * All Database classes will folow this interface.
 * class Base
    {
    public:
        Base() = delete;

        enum ThreadMode {
            NeedsMutex,
            NoMutex,
        };

        static QSqlDatabase database() = 0;
        static void open(  ) = 0;
        static void close() = 0;
        static void addDatabase() = 0;
        static void removeDatabase() = 0;
    };
 *
 *
 */

namespace Library {

    class LibretroDatabase {
        public:

            enum ThreadMode {
                NeedsMutex,
                NoMutex,
            };

            LibretroDatabase();

            LibretroDatabase( const ThreadMode mode );

            ~LibretroDatabase();

            //operator QSqlDatabase() const
           // {
            //    return database();
           // }

            explicit operator QSqlQuery() {
                return QSqlQuery( database() );
            }

            QSqlDatabase database() const;

            void open( const ThreadMode mode = NoMutex );
            void close( const ThreadMode mode = NoMutex );

            static void addDatabase();
            static void removeDatabase();

            static void addConnection( const QString &name ) {
                if ( !QSqlDatabase::contains( name ) ) {
                    qDebug() << "Added connection";



                    QSqlDatabase db = QSqlDatabase::addDatabase( QStringLiteral( "QSQLITE" ), name );

                    QString dataPathStr = PhxPaths::metadataLocation();
                    Q_ASSERT_X( !dataPathStr.isEmpty(), Q_FUNC_INFO, "dataPathStr.isEmpty()" );

                    QDir dataPath( dataPathStr );

                    QString databaseName = QStringLiteral( "libretro.sqlite" );
                    QString filePath = dataPath.filePath( databaseName );

                    db.setDatabaseName( filePath );

                    //bool open = db.open();
                    //Q_ASSERT_X( open, Q_FUNC_INFO, "db.open() == false" ) ;

                    //qCDebug( phxLibrary, "Opening library database %s", qPrintable( db.databaseName() ) );
                }
            }

            static QSqlDatabase connection( const QString &name ) {
                return QSqlDatabase::database( name );
            }

            static QMutex mutex;

    private:
            ThreadMode mMode;
    };

}

#endif // SYSTEMDATABASE_H
