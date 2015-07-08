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
#include "metadatadatabase.h"

namespace Library {

    class LibraryModel : public QSqlTableModel {
            Q_OBJECT
            Q_PROPERTY( int count READ count NOTIFY countChanged )
            Q_PROPERTY( bool recursiveScan READ recursiveScan WRITE setRecursiveScan
                        NOTIFY recursiveScanChanged )

            Q_PROPERTY( qreal progress READ progress NOTIFY progressChanged )
            Q_PROPERTY( QString message READ message WRITE setMessage NOTIFY messageChanged )

       public:

            // GameImportData is used to import game files into the SQL database.
            // This is a a simple data grouping to simplify signals and slots

            struct GameImportData {
                qreal importProgress;
                QString system;
                QString timePlayed;
                QString title;
                QString filePath;
            };

            using QSqlTableModel::setFilter;

            explicit LibraryModel( LibraryInternalDatabase &db, QObject *parent = 0 );

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
            };

            // Getters
            bool select();
            bool cancelScan();

            //  QML Getters
            int count() const;
            qreal progress() const;
            bool recursiveScan() const;
            QString message() const;

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
            bool remove( int row, int count = 1 );

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

            // Cancels the import progress if the mScanFilesThread is running.
            void cancel();

            void sync();

            void startMetaDataScan();

            void updateUknownMetadata();

            void resumeMetadataScan();

        private slots:

            // handleFilesFound runs on the main QML thread, and is
            // where the SQL query statement is created and executed.
            void handleFilesFound( const GameImportData importData );

            // This function is connected to the mScanFilesThread.
            // This iterates through the mImportUrl, if the url is
            // a folder, and emits the filesFound signal.

            // findFiles is needed to run directly on the mScanFilesThread
            // so make sure that this signal is connected with a Qt::DirectionConnection;
            // this is because the LibraryModel is not moved into the mScanFilesThread,
            // nor should it be.
            void findFiles();

            // This needs to be run on the main thread.
            void setMetadata( const GameMetaData metaData );

        signals:
            void countChanged();
            void messageChanged();
            void recursiveScanChanged();
            void progressChanged();
            void fileFound( const GameImportData importData );
            void cancelScanChanged( const bool cancel );
            void calculateCheckSum( GameMetaData metaData );

        private:
            // Normal Variables
            QStringList mFileFilter;
            QHash<int, QByteArray> mRoleNames;
            QVariantList params;
            QMutex scanMutex;
            // Used to find metadata for any game.
            MetaDataDatabase mMetaDataDatabse;
            int lastUpdatedRowID;

            // Is true when the startMetaDataScan() function has started and
            // is false when it's completeled. This is used to syncronize the
            // setMetadata() function.
            bool mMetaDataEmitted;

            // This thread is started when a user wants to import
            // a games folder. Currently, the thread quits whenever the
            // user cancels and import, or the import finishes.
            QThread mScanFilesThread;

            // This thread is started when metadata, such as artwork, is being found for a game.
            QThread mGetMetadataThread;

            // mImportUrl is the url of the folder or file that is
            // being scanned and imported.
            QUrl mImportUrl;
            bool mCancelScan;

            // QML Variables
            int qmlCount;
            bool qmlRecursiveScan;
            qreal qmlProgress;
            QString qmlMessage;

            // QML Setters
            void setProgress( const qreal progress );

            // Normal Setters
            void setCancelScan( const bool scan );


            // Helper Functions
            void checkHeaderOffsets( const QFileInfo &fileInfo, QString &platform );
            bool getCueFileInfo( QFileInfo &fileInfo );


    };


}

#endif // LIBRARYMODEL_H
