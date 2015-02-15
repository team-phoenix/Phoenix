
#ifndef THEGAMESDB_H
#define THEGAMESDB_H

#include <QObject>
#include <QDir>
#include <QStringList>
#include <QString>
#include <QList>
#include <QUrl>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include "logging.h"
#include "scraper.h"


class TheGamesDB : public Scraper {
    public:

        TheGamesDB();
        ~TheGamesDB();

        Q_INVOKABLE void getGameData( Scraper::ScraperContext context );

        const QMap<QString, QString> PlatformsMap {
            { "Atari Lynx", "Atari Lynx" },
            { "DOS", "DOS" },
            { "Game Boy Advance", "Nintendo Game Boy Advance" },
            { "Nintendo", "Nintendo Entertainment System (NES)" },
            { "Super Nintendo", "Super Nintendo (SNES)" },
            { "Game Boy", "Nintendo Game Boy" },
            { "Nintendo DS", "Nintendo DS" },
            { "Sega Master System", "Sega Master System" },
            { "Sega Mega Drive", "Sega Mega Drive" },
            { "Sega Game Gear", "Sega Game Gear" },
            { "Sega CD", "Sega CD" },
            { "Sega 32X", "Sega 32X" },
            { "Sony PlayStation", "Sony Playstation" },
            { "Arcade", "Arcade" },
            { "Film", "Film" }
        };

    public slots:

        void processRequest( QNetworkReply *reply );

    private:

        Scraper::ScraperData *findXMLGame( QString id, QNetworkReply *reply );
        QString parseXMLforId( QString game_name, QNetworkReply *reply );
};

#endif // THEGAMESDB_H
