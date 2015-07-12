#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QSqlTableModel>
#include <QVariant>
#include <QHash>
#include <QUrl>
#include <QMutex>
#include <QFileInfo>
#include <QThread>

#include "libraryinternaldatabase.h"
#include "libretro_cores_info_map.h"
#include "platforms.h"
#include "libraryworker.h"


/* Known Bugs:
 *      1. Scanning metadata doesn't start it's scan at the first included row form an import.
 *
 *
 *
 */

namespace Library {

    class LibraryModel : public QSqlTableModel {
            Q_OBJECT
            Q_PROPERTY( int count READ count NOTIFY countChanged )
            Q_PROPERTY( bool recursiveScan READ recursiveScan WRITE setRecursiveScan
                        NOTIFY recursiveScanChanged )

            Q_PROPERTY( qreal progress READ progress NOTIFY progressChanged )
            Q_PROPERTY( QString message READ message WRITE setMessage NOTIFY messageChanged )
            Q_PROPERTY( bool insertPaused READ insertPaused )
            Q_PROPERTY( bool insertCancelled READ insertCancelled )

        public:

            // GameImportData is used to import game files into the SQL database.
            // This is a a simple data grouping to simplify signals and slots

            using QSqlTableModel::setFilter;

            explicit LibraryModel( QObject *parent = 0 );

            ~LibraryModel();

            // Model Roles
            enum GameRoles {
                TitleRole = Qt::UserRole + 1,
                SystemRole,
                TimePlayedRole,
                ArtworkRole,
                FileNameRole,
                SystemPathRole,
                RowIDRole,
                SHA1Role,
            };

            // Getters
            bool select();
            bool transaction();

            //  QML Getters
            int count() const;
            qreal progress() const;
            bool recursiveScan() const;
            QString message() const;
            bool insertPaused();
            bool insertCancelled();


            // QML Setters
            void setRecursiveScan( const bool scan );
            void setMessage( const QString message );

            // Subclass Setters.
            QVariantMap get( int inx );
            QVariant data( const QModelIndex &index, int role ) const;

            // Subclass Getters
            QHash<int, QByteArray> roleNames() const;

        public slots:
            // Removes 1 row from the SQL model.
            // bool remove( int row, int count = 1 );

            // Starts the import progress. The url is a folder or
            // file that the user wants to import.
            void append( const QUrl url );

            // clear removes every SQL row  in the database permanently.
            void clear();

            //  updateCount() reads the SQL database and updates the count()
            // function with how many entries are actually in the databaes.
            // This is important for creating our own scrollbar.
            void updateCount();

            // Filters the SQL model based on a SQL query.
            // This is used to filter games in the BoxartGrid
            void setFilter( QString filter
                            , QVariantList params
                            , bool preserveCurrentFilter );


            void sync();

            //void startMetaDataScan();

            //void updateUknownMetadata();

            //void resumeMetadataScan();

            void pauseInsert();

            void resumeInsert();

            void cancelInsert();

            void closeWorkerThread();

        private slots:

            // handleInsertGame runs on the main QML thread, and is
            // where the SQL query statement is created and executed.
            void handleInsertGame( const GameData importData );

            // This function is connected to the mWorkerThread.
            // This iterates through the mImportUrl, if the url is
            // a folder, and emits the filesFound signal.

            // This needs to be run on the main thread.
            void handleUpdateGame( const GameData metaData );

        signals:
            void countChanged();
            void messageChanged();
            void recursiveScanChanged();
            void progressChanged();
            void insertCancelledChanged();
            void insertPausedChanged();

            void insertGames( const QUrl url );
            void signalInsertCancelled( const bool cancel );
            void signalInsertPaused( const bool paused );

        private:

            explicit LibraryModel( LibraryInternalDatabase &db, QObject *parent = 0 );
            explicit LibraryModel( LibraryInternalDatabase *db, QObject *parent = 0 );

            // Normal Variables
            QStringList mFileFilter;
            QHash<int, QByteArray> mRoleNames;
            QVariantList params;
            QMutex mMutex;
            LibraryInternalDatabase *libraryDb;

            // This thread is started when a user wants to import
            // a games folder. Currently, the thread quits whenever the
            // user cancels and import, or the import finishes.
            QThread mWorkerThread;

            QString mLastUpdatedIdentifier;

            bool mTransaction;
            bool qmlInsertPaused;
            bool qmlInsertCancelled;

            LibraryWorker mLibraryWorker;
            // Used to find metadata for any game.
            MetaDataDatabase mMetaDataDatabse;


            // QML Variables
            int qmlCount;
            bool qmlRecursiveScan;
            qreal qmlProgress;
            QString qmlMessage;


            // QML Setters
            void setProgress( const qreal progress );

            // Normal Setters
    };

}

#endif // LIBRARYMODEL_H
