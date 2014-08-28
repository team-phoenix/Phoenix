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
    TheGamesDB();
    explicit TheGamesDB(QObject *parent);

    ~TheGamesDB();

    void setGameName(QString name);
    void setGamePlatform(QString platform);
    void setData(GameData *data);

signals:
    void finished(GameData *);
    void finished();
    void started();
    void completedRequest();
    void outputData(GameData *);

public slots:
    void processRequest(QNetworkReply *m_reply);
    void processErrors(QNetworkReply::NetworkError);
    void populateData();
    void start();


private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    GameData *game_data;
    QString m_game_name;
    QString m_game_platform;
    QUrl m_url;

    QString cleanString(QString string);
    void getGame(QString game_id);
    void findXMLGame();
    void parseXMLforId(QString game_name);
    void getGameId();
    void getNetworkReply();
    void setUrl(QUrl url);

};

#endif // THEGAMESDB_H
