#ifndef GAMEFILEINFO_H
#define GAMEFILEINFO_H

#include <QList>
#include <QFileInfo>
#include <QSqlQuery>

namespace Library {

    class GameFileInfo : public QFileInfo {
    public:
        GameFileInfo( const QString &file );

        struct HeaderData {
            QString result;
            QString systemIndex;
            qint64 seekPosition;
            qint64 byteLength;
        };

        struct CueData {
            QString system;
            QString sha1;
        };

        QString system() const;
        QString sha1CheckSum() const;
        QString title() const;
        QString fullFilePath() const;
        QString timePlayed() const;

        enum FileType {
            UnsupportedFile = -1,
            GameFile = 0,
            ZipFile,
            BiosFile,
            CueFile,
        };

        FileType fileType() const;

        static QStringList gameFilter();
        QList<HeaderData> getPossibleHeaders( const QStringList &possibleSystems );
        QString getRealSystem( const QList<HeaderData> &possibleHeaders );

        void cache( const QString &location );

    protected:
        FileType mFileType;
        QString mSystem;
        QString mSha1Sum;
        QString mTitle;
        QString mFullFilePath;
        QSqlQuery mQuery;

        QStringList getAvailableSystems( const QString &extension );
        QString getCheckSum( const QString &filePath );

        void update( const QString &extension );

        bool isBios( QString &biosName );


    private:
        QString mTimePlayed;

    };


}

#endif // GAMEFILEINFO_H
