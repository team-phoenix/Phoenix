
#ifndef THEGAMESDB_H
#define THEGAMESDB_H

#include <QObject>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>
#include <QDir>
#include <QStringList>
#include <QString>
#include <QList>
#include <QUrl>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include "logging.h"

class GameData {
public:
    GameData() {
    }

    int libraryId;
    QString libraryName;
    QString librarySystem;
    QString id;
    QString title;
    QString developer;
    QString platform_id;
    QString platform;
    QString release_date;
    QString overview;
    QString esrb;
    QString genre;
    QString players;
    QString co_op;
    QString youtube;
    QString publisher;
    QString rating;
    QString back_boxart;
    QString front_boxart;
    QString clear_logo;
    QString play_time;

};

class TheGamesDB : public QObject {
    Q_OBJECT
public:
    enum QueryState {
        None,
        RequestingId,
        RequestingData,
        Finishing
    };

    TheGamesDB();
    ~TheGamesDB();

    Q_INVOKABLE void getGameData(int id, QString title, QString system);

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

signals:
    void dataReady(GameData*);

public slots:
    void processRequest(QNetworkReply*);

private:
    QNetworkAccessManager *manager;

    QString cleanString(QString string);
    GameData* findXMLGame(QString id, QNetworkReply* reply);
    QString parseXMLforId(QString game_name, QNetworkReply* reply);
    float hit_ratio;
};

#endif // THEGAMESDB_H
