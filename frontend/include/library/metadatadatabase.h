#ifndef METADATADATABASE_H
#define METADATADATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QObject>
#include <QString>
#include <QList>
#include <QMutex>

namespace Library {

    // GameMetaData is used to set in metadata for any game during;
    // this usually used after the append process.
    struct GameMetaData {
        QString sha1;
        QString artworkUrl;
        QString goodToolsCode;
        QString region;
        QString developer;
        QString genre;
        QString description;
        QString title;
        QString filePath;
        int rowIndex;
        qreal progress;
        bool updated;
    };

    class MetaDataDatabase : public QObject {
            Q_OBJECT
        public:
            static const QString tableRoms;
            static const QString tableSystems;
            static const QString tableReleases;
            static const QString tableRegions;

            explicit MetaDataDatabase( QObject *parent = 0 );

            ~MetaDataDatabase();

            QSqlDatabase &database();

            void open();
            void setCancel( const bool cancel );

        public slots:
            void getMetadata( GameMetaData filePath );
            QString getCheckSum( const QString filePath );

        signals:
            void updateMetadata( const GameMetaData metaData );


        private:
            QSqlDatabase db;
            QList<int> mRomIDList;
            qreal mProgress;
            bool mCancel;
            QMutex mMutex;
            QMutex sqlMutex;
    };

}

#endif // METADATADATABASE_H
