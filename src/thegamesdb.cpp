#include "qdebug.h"
#include "thegamesdb.h"

const QString BASE_URL = "http://thegamesdb.net/api/";
const QString ART_BASE_URL = "http://thegamesdb.net/banners/";
const QStringList EXPRESSIONS = (QStringList() << "-"
                                               << "\\"
                                               << "/"
                                               << "!"
                                               << "."
                                               << "?"
                                               << ":"
                                               );

TheGamesDB::TheGamesDB()
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processRequest(QNetworkReply*)));

}

TheGamesDB::~TheGamesDB()
{
    if (manager)
        manager->deleteLater();
}

void TheGamesDB::processRequest(QNetworkReply* reply)
{
    switch (reply->property("state").toInt()) {
        case RequestingId:
        {
            QString id = parseXMLforId(reply->property("gameName").toString(), reply);
            if (id == "") {
                return;
            }

            auto secondReply = manager->get(QNetworkRequest(QUrl(BASE_URL + "GetGame.php?id=" + id)));
            secondReply->setProperty("gameId", id);
            secondReply->setProperty("gameName", reply->property("gameName"));
            secondReply->setProperty("gameSystem", reply->property("gameSystem"));
            secondReply->setProperty("state", RequestingData);
            break;
        }
        case RequestingData:
        {
            qDebug() << "Parsing XML for game";
            GameData* game_data = findXMLGame(reply->property("gameId").toString(), reply);
            game_data->libraryName = reply->property("gameName").toString();
            game_data->librarySystem = reply->property("gameSystem").toString();
            emit dataReady(game_data);
            break;
        }
        default:
            break;
    }

    reply->deleteLater();
}

QString TheGamesDB::cleanString(QString string)
{
    QStringList str_list = string.split(" ");
    for (int i=0; i < str_list.length(); ++i) {
        if (str_list.at(i).contains("(")) {
            if (str_list.at(i).contains(")"))
                str_list.removeAt(i);
            else
                str_list.removeAll("(");
        }
    }

    QString new_str = str_list.join(" ");

    for (int i=0; i < EXPRESSIONS.length(); i++) {
        new_str.remove(EXPRESSIONS.at(i));
    }
    QString stringNormalized = new_str.normalized(QString::NormalizationForm_KD);
    stringNormalized.remove(QRegExp("[^a-zA-Z\\s]"));

    return stringNormalized.toLower();
}

GameData* TheGamesDB::findXMLGame(QString id, QNetworkReply* reply)
{
    GameData* game_data = new GameData();
    game_data->id = id;

    QXmlStreamReader reader(reply);

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "id") {
                QString text = reader.readElementText();
                id = text;
            }
            else if (element == "GameTitle") {
                QString text = reader.readElementText();
                game_data->title= text;
            }
            else if (element == "PlatformID") {
                QString text = reader.readElementText();
                game_data->platform_id = text;
            }
            else if (element == "Platform") {
                QString text = reader.readElementText();
                game_data->platform = text;
            }
            else if (element == "ReleaseDate") {
                QString text = reader.readElementText();
                game_data->release_date = text;

            }
            else if (element == "Overview") {
                QString text = reader.readElementText();
                game_data->overview = text;
            }
            else if (element == "ESRB") {
                QString text = reader.readElementText();
                game_data->esrb = text;
            }
            else if (element == "genre") {
                QString text = reader.readElementText();
                game_data->genre = text;
            }
            else if (element == "Players") {
                QString text = reader.readElementText();
                game_data->players = text;
            }
            else if (element == "Co-op") {
                QString text = reader.readElementText();
                game_data->co_op = text;
            }
            else if (element == "Youtube") {
                QString text = reader.readElementText();
                game_data->youtube = text;
            }
            else if (element == "Publisher") {
                QString text = reader.readElementText();
                game_data->publisher = text;
            }
            else if (element == "Developer") {
                QString text = reader.readElementText();
                game_data->developer = text;
            }
            else if (element == "Rating") {
                QString text = reader.readElementText();
                game_data->rating = text;
            }
            else if (element == "boxart") {
                QStringRef side = reader.attributes().value("side");
                QString text = reader.readElementText();
                if (side == "back" || side == "l/ba")
                    game_data->back_boxart = ART_BASE_URL + text;
                else if (side == "front" || side == "l/fro")
                    game_data->front_boxart = ART_BASE_URL + text;
            }
            else if (element == "clearlogo") {
                QString text = reader.readElementText();
                game_data->clear_logo= text;
            }
        }
    }

    return game_data;
}


QString TheGamesDB::parseXMLforId(QString game_name, QNetworkReply* reply)
{
    QXmlStreamReader reader(reply);

    QString id;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString element = reader.name().toString();
            if (element == "id") {
                QString game_id = reader.readElementText();
                id = game_id;
            }
            else if (element == "GameTitle") {
                QString text = reader.readElementText();
                QString cleaned_title = cleanString(text);

                if (cleaned_title.indexOf(cleanString(game_name)) != -1) {
                    break;
                }
                else
                    id = "";
            }
        }
    }

    return id;
}

void TheGamesDB::getGameData(QString title, QString system)
{
    // Grab the first data
    auto reply = manager->get(QNetworkRequest(QUrl(BASE_URL + "GetGamesList.php?name=" + title + "&platform=" + PlatformsMap[system])));
    reply->setProperty("gameName", title);
    reply->setProperty("gameSystem", system);
    reply->setProperty("state", RequestingId);
}
