
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

    void getGameData(QString title, QString system);

    const QMap<QString, QString> PlatformsMap {
        { "Game Boy Advance", "Nintendo Game Boy Advance" }
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
};

#endif // THEGAMESDB_H
