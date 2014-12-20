#include "qdebug.h"
#include "thegamesdb.h"

TheGamesDB::TheGamesDB()
{
    setBaseUrl("http://thegamesdb.net/api/");
}

TheGamesDB::~TheGamesDB()
{

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


            auto secondReply = networkManager()->get(QNetworkRequest(QUrl(baseUrl() + "GetGame.php?id=" + id)));
            secondReply->setProperty("gameId", id);
            secondReply->setProperty("libraryId", reply->property("libraryId"));
            secondReply->setProperty("gameName", reply->property("gameName"));
            secondReply->setProperty("gameSystem", reply->property("gameSystem"));
            secondReply->setProperty("state", RequestingData);
            break;
        }
        case RequestingData:
        {
            ScraperData* game_data = findXMLGame(reply->property("gameId").toString(), reply);
            game_data->libraryId = reply->property("libraryId").toInt();
            game_data->libraryName = reply->property("gameName").toString();
            game_data->librarySystem = reply->property("gameSystem").toString();

            emit progress((qreal)current_count.load(std::memory_order_relaxed) / request_count * 100.0);

            if (current_count == request_count) {
                current_count = 0;
                request_count = 0;
            }
            emit dataReady(game_data);

            break;
        }
        default:
            break;
    }

    reply->deleteLater();
}


Scraper::ScraperData* TheGamesDB::findXMLGame(QString id, QNetworkReply* reply)
{
    ScraperData* game_data = new ScraperData();
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
                const QString ART_BASE_URL = "http://thegamesdb.net/banners/";

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

    reply->deleteLater();
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
                QString cleaned_game = cleanString(game_name);

                QStringList c = cleaned_title.split(" ");
                int found_count = 0;
                for (int i=0; i < c.length(); ++i) {
                    if (cleaned_game.indexOf(c.at(i)) != -1)
                        found_count++;
                }

                float found_ratio = static_cast<float>(found_count / (float)c.length());
                if (found_ratio >= hit_ratio)
                    break;
                else
                    id = "";
            }
        }
    }

    reply->deleteLater();

    return id;
}

void TheGamesDB::getGameData(Scraper::ScraperContext context)
{
    // Grab the first data
    current_count++;
    if (current_count == 1) {
        emit label("Fetching Artwork");
        emit progress(0.0);
    }

    auto reply = networkManager()->get(QNetworkRequest(QUrl(baseUrl() + "GetGamesList.php?name=" + context.title + "&platform=" + PlatformsMap[context.system])));
    if (context.id != -1)
        reply->setProperty("libraryId", context.id);

    reply->setProperty("gameName", context.title);
    reply->setProperty("gameSystem", context.system);
    reply->setProperty("state", RequestingId);
}
