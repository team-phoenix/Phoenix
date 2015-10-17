#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include "frontendcommon.h"

#include "libraryinternaldatabase.h"
#include "libraryworker.h"
#include "logging.h"

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
                CoreFilePathRole,
            };

            explicit LibraryModel( QObject *parent = 0 );

            ~LibraryModel();

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
            void clearDatabase();

            //  updateCount() reads the SQL database and updates the count()
            // function with how many entries are actually in the databaes.
            // This is important for creating our own scrollbar.
            void updateCount();

            // Filters the SQL model based on a SQL query.
            // This is used to filter games in the BoxartGrid
            void setFilter( const QString table, const QString row, const QVariant value );
            void clearFilter( const QString table, const QString row );



            void sync();

            //void startMetaDataScan();

            //void updateUknownMetadata();

            //void resumeMetadataScan();

            //These signals are used to accept external drag and drop events.
            void handleDraggedUrls( QList<QUrl> urls );
            void handleContainsDrag( const bool contains );
            void handleDroppedUrls();


            void pauseInsert();

            void resumeInsert();

            void cancelInsert();

            void closeWorkerThread();

            void startWorkerThread() {
                mWorkerThread.start( QThread::HighPriority );
            }


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

            void draggedUrls( QList<QUrl> urls );
            void containsDrag( const bool contains );
            void droppedUrls();

            void insertGames( const QString url, bool autoStart );
            void signalInsertCancelled( const bool cancel );
            void signalInsertPaused( const bool paused );

        protected:

            QString selectStatement() const;

        private:

            explicit LibraryModel( LibraryInternalDatabase &db, QObject *parent = 0 );
            explicit LibraryModel( LibraryInternalDatabase *db, QObject *parent = 0 );

            // Normal Variables
            QStringList mFileFilter;
            QHash<int, QByteArray> mRoleNames;
            QVariantList filterParameters;
            QMutex mMutex;
            QHash<QString, QVariant> filterParameterMap;


            // This thread is started when a user wants to import
            // a games folder. Currently, the thread quits whenever the
            // user cancels and import, or the import finishes.
            QThread mWorkerThread;

            bool mFilterCollection;
            bool mTransaction;
            bool qmlInsertPaused;
            bool qmlInsertCancelled;

            LibraryWorker mLibraryWorker;

            // QML Variables
            int qmlCount;
            bool qmlRecursiveScan;
            qreal qmlProgress;
            QString qmlMessage;


            // QML Setters
            void setProgress( const qreal progress );


            // Normal Setters

            QString createFilter();
    };

}

#endif // LIBRARYMODEL_H
