#include "qdebug.h"
#include "thegamesdb.h"

const QString BASE_URL = "http://thegamesdb.net/api/";
const QString ART_BASE_URL = "http://thegamesdb.net/banners/";
const QStringList EXPRESSIONS = (QStringList() << " "
                                               << "-"
                                               << "\\"
                                               << "/"
                                               << "("
                                               << ")"
                                               << "!"
                                               << "."
                                               << "?"
                                               << ":");


TheGamesDB::TheGamesDB (QObject *parent)
    : QObject(parent)
{
    reply = nullptr;
    manager = nullptr;

    connect(this, SIGNAL(finished()), this, SLOT(resetNetwork()));
}

TheGamesDB::~TheGamesDB()
{
    if (reply)
        reply->deleteLater();
    if (manager)
        manager->deleteLater();
}

void TheGamesDB::resetNetwork()
{
    if (reply)
        reply->deleteLater();
    if (manager)
        manager->deleteLater();
   // reply = new QNetworkReply;
    //manager = new QNetworkAccessManager;
}

void TheGamesDB::getNetworkReply(QString url)
{

    QEventLoop eventLoop;
    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31");

    reply = manager->get(request);
    eventLoop.exec();

}

QString TheGamesDB::cleanString(QString string)
{
    for (int i=0; i < EXPRESSIONS.length(); i++) {
        string.remove(EXPRESSIONS.at(i));
    }
    return string.toLower();
}

bool TheGamesDB::noErrors()
{
    if (reply->error() == QNetworkReply::NoError)
        return true;
    else {
        qDebug() << "Network error: " << reply->errorString();
        return false;
    }
}

GameData TheGamesDB::findXMLGame()
{
    QXmlStreamReader reader(reply);
    GameData game;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "id") {
                QString text = reader.readElementText();
                game.id = text;
            }
            else if (element == "GameTitle") {
                QString text = reader.readElementText();
                game.title= text;
            }
            else if (element == "PlatformID") {
                QString text = reader.readElementText();
                game.platform_id = text;
            }
            else if (element == "Platform") {
                QString text = reader.readElementText();
                game.platform = text;
            }
            else if (element == "ReleaseDate") {
                QString text = reader.readElementText();
                game.release_date = text;

            }
            else if (element == "Overview") {
                QString text = reader.readElementText();
                game.overview = text;
            }
            else if (element == "ESRB") {
                QString text = reader.readElementText();
                game.esrb = text;
            }
            else if (element == "genre") {
                QString text = reader.readElementText();
                game.genre = text;
            }
            else if (element == "Players") {
                QString text = reader.readElementText();
                game.players = text;
            }
            else if (element == "Co-op") {
                QString text = reader.readElementText();
                game.co_op = text;
            }
            else if (element == "Youtube") {
                QString text = reader.readElementText();
                game.youtube = text;
            }
            else if (element == "Publisher") {
                QString text = reader.readElementText();
                game.publisher = text;
            }
            else if (element == "Developer") {
                QString text = reader.readElementText();
                game.developer = text;
            }
            else if (element == "Rating") {
                QString text = reader.readElementText();
                game.rating = text;
            }
            else if (element == "boxart") {
                QStringRef side = reader.attributes().value("side");
                QString text = reader.readElementText();
                if (side == "back" || side == "l/ba")
                    game.back_boxart = ART_BASE_URL + text;
                else if (side == "front" || side == "l/fro")
                    game.front_boxart = ART_BASE_URL + text;
            }
            else if (element == "clearlogo") {
                QString text = reader.readElementText();
                game.clear_logo= text;
            }
        }
    }
    return game;
}

GameData TheGamesDB::findXMLArt()
{
    QXmlStreamReader reader(reply);
    GameData art_map;

    int counter = 0;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "boxart") {
                counter++;
                QStringRef side = reader.attributes().value("side");
                if (side == "back" || side == "l/ba")
                    art_map.back_boxart = ART_BASE_URL + reader.readElementText();
                else if (side == "front" || side == "l/fro")
                    art_map.front_boxart = ART_BASE_URL + reader.readElementText();
                if (counter == 2)
                    break;
            }
        }
    }
    return art_map;
}

GameData TheGamesDB::parseXMLforId(QString game_name)
{
    QXmlStreamReader reader(reply);
    GameData game;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "id") {
                QString game_id = reader.readElementText();
                game.id = game_id;
            }
            else if (element == "GameTitle") {
                QString text = reader.readElementText();
                QString cleaned_title = cleanString(text);
                if (cleaned_title.indexOf(cleanString(game_name)) != -1) {
                    break;
                }
            }
        }
    }
    return game;
}

QList< GameData> TheGamesDB::getXMLPlatformList()
{
    QXmlStreamReader reader(reply);
    QList< GameData > platforms_list;
    GameData platform;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "Platform") {
                platform = GameData();
            }
            else if (element == "id") {
                QString text = reader.readElementText();
                platform.id = text;
            }
            else if (element == "name") {
                QString text = reader.readElementText();
                platform.name= text;
            }
            else if (element == "alias") {
                QString text = reader.readElementText();
                platform.alias  = text;
                platforms_list.append(platform);
            }
        }
    }
    return platforms_list;
}

QList< GameData > TheGamesDB::getXMLGamesList()
{
    QXmlStreamReader reader(reply);
    QList< GameData > games_list;
    GameData game_map;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "Game") {
                game_map = GameData();
            }
            else if (element == "id") {
                QString text = reader.readElementText();
                game_map.id = text;
            }
            else if (element == "GameTitle") {
                QString text = reader.readElementText();
                game_map.title = text;
            }
            else if (element == "ReleaseDate") {
                QString text = reader.readElementText();
                game_map.release_date = text;
            }
            else if (element == "Platform") {
                QString text = reader.readElementText();
                game_map.platform = text;
                games_list.append(game_map);
            }
        }
    }
    return games_list;
}

QList< GameData > TheGamesDB::getXMLPlatformGames()
{
    QXmlStreamReader reader(reply);
    QList< GameData > platform_games_list;
    GameData platform_games;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "Game") {
                platform_games = GameData();
            }
            else if (element == "id") {
                QString text = reader.readElementText();
                platform_games.id = text;
            }
            else if (element == "GameTitle") {
                QString text = reader.readElementText();
                platform_games.title = text;
            }
            else if (element == "ReleaseDate") {
                QString text = reader.readElementText();
                platform_games.release_date = text;
                platform_games_list.append(platform_games);
            }
        }
    }
    return platform_games_list;
}

QList< GameData > TheGamesDB::getPlatformsList() {
    QString url = BASE_URL + "GetPlatformsList.php";
    getNetworkReply(url);
    QList< GameData > platform_list;

    if (noErrors()) {
        //success
        platform_list = getXMLPlatformList();
    }

    emit finished();
    return platform_list;
}

GameData TheGamesDB::getGame(QString game_id) {
    QString url = BASE_URL + "GetGame.php?id=" + game_id;
    getNetworkReply(url);
    GameData game;

    if (noErrors()) {
        game = findXMLGame();
    }

    emit finished();
    return game;
}

GameData TheGamesDB::getGameId(QString game_name, QString platform) {
    QString url = BASE_URL + "GetGamesList.php?name=" + game_name
                  + "&platform=" + platform;
    getNetworkReply(url);
    GameData game_id;

    if (noErrors()) {
        game_id = parseXMLforId(game_name);
    }

    emit finished();
    return game_id;
}

GameData TheGamesDB::getArt(QString game_id)
{
    QString url = BASE_URL + "GetArt.php?id=" + game_id;
    getNetworkReply(url);
    GameData art;

    if (noErrors()) {
        art = findXMLArt();
    }

    emit finished();
    return art;
}

QList< GameData > TheGamesDB::getPlatformGames(QString platform_id)
{
    QString url = BASE_URL + "GetPlatformGames.php?platform=" + platform_id;
    getNetworkReply(url);
    QList< GameData > platform_games_list;

    if (noErrors()) {
        platform_games_list = getXMLPlatformGames();
    }

    emit finished();
    return platform_games_list;
}

QList< GameData > TheGamesDB::getGamesList(QString game_name)
{
    QString url = BASE_URL + "GetGamesList.php?name=" + game_name;
    getNetworkReply(url);
    QList< GameData > games_list;

    if (noErrors()) {
        games_list = getXMLGamesList();
    }

    emit finished();
    return games_list;
}

GameData TheGamesDB::getAllData(QString game_name, QString platform)
{
    QString game_id;

    game_id = getGameId(game_name, platform).id;
    return getGame(game_id);
}

void TheGamesDB::printPlatformsList()
{
    QList< GameData > platform_list = getPlatformsList();
    GameData platform;

    for (int i=0; i < platform_list.length(); i++) {
        platform = platform_list.at(i);
        qDebug() << "Name: " << platform.name;
        qDebug() << "ID: " << platform.id;
        qDebug() << "Alias: " << platform.alias << '\n';
    }
}

void TheGamesDB::printGame(QString game_name, QString platform)
{
    GameData game = getAllData(game_name, platform);

    qDebug() << "##############################################";
    qDebug() << game.title;
    qDebug() << "##############################################\n";
    game.printAll();
    qDebug() << '\n';
}

void TheGamesDB::printGamesList(QString game_name)
{
    QList < GameData > game_list = getGamesList(game_name);

    for (int i=0; i < game_list.size(); ++i) {
        GameData g = game_list.at(i);
        qDebug() << "ID: " << g.id;
        qDebug() << "GameTitle: " << g.title;
        qDebug() << "Alias: " << g.release_date;
        qDebug() << "Platform: " <<  g.platform << '\n';
    }
}

void TheGamesDB::printPlatformGames(QString platform_id)
{
    // CAUTION: This could print out over 3,000 games.

    QList < GameData > platform_games_list = getPlatformGames(platform_id);

    for (int i=0; i < platform_games_list.size(); ++i) {
        GameData p = platform_games_list.at(i);
        qDebug() << "ID: " <<  p.id;
        qDebug() << "GameTitle: " <<  p.title;
        qDebug() << "ReleaseDate: " <<  p.release_date << '\n';
    }
}
