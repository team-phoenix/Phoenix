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

class GameData {
public:
    GameData() {
    }

    void printAll() {
        qDebug() << alias;
        qDebug() << name;
        qDebug() << id;
        qDebug() << title;
        qDebug() << developer;
        qDebug() << platform_id;
        qDebug() << platform;
        qDebug() << release_date;
        qDebug() << overview;
        qDebug() << esrb;
        qDebug() << genre;
        qDebug() << players;
        qDebug() << co_op;
        qDebug() << youtube;
        qDebug() << publisher;
        qDebug() << rating;
        qDebug() << back_boxart;
        qDebug() << front_boxart;
        qDebug() << clear_logo;
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

};

class TheGamesDB : public QObject {
    Q_OBJECT
public:
    TheGamesDB();
    ~TheGamesDB();

    GameData getArt(QString game_id);
    GameData getGame(QString game_id);
    GameData getGameId(QString game_name, QString platform);
    QList< GameData > getGamesList(QString game_name);
    GameData getAllData(QString game_name, QString platform);
    QList< GameData > getPlatformsList();
    QList< GameData > getPlatformGames(QString platform_id);

    void printPlatformGames(QString platform_id);
    void printPlatformsList();
    void printGame(QString game_name, QString platform);
    void printGamesList(QString game_name);

private:
    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    void getNetworkReply(QString url);
    bool noErrors();

    QString cleanString(QString string);
    GameData findXMLArt();
    GameData findXMLGame();
    QList< GameData > getXMLGamesList();
    QList< GameData > getXMLPlatformList();
    QList< GameData > getXMLPlatformGames();
    GameData parseXMLforId(QString game_name);
};

#endif // THEGAMESDB_H
