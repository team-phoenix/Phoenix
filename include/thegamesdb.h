
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

    QString alias;
    QString name;
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
    explicit TheGamesDB(QObject *parent);

    ~TheGamesDB();

    void getGameData(QString title, QString system);
signals:
    void dataReady(GameData*);

public slots:
    void processRequest(QNetworkReply*);

private:
    QNetworkAccessManager *manager;

    QueryState state;
    GameData *game_data;
    QString m_game_name;
    QString m_game_platform;

    QString cleanString(QString string);
    void findXMLGame(QNetworkReply* reply);
    void parseXMLforId(QString game_name, QNetworkReply* reply);
};

#endif // THEGAMESDB_H
