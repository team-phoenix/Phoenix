#include <QSqlDatabase>
#include <QSqlQuery>

#include "networkqueue.h"
#include "logging.h"
#include "thegamesdb.h"

NetworkQueue::NetworkQueue()
{
    this->moveToThread(&network_thread);
    m_scraper = nullptr;
    setScraper(new TheGamesDB());

    m_progress = 0.0;
    counter = 0;
    m_request_count = 0;

    m_game_model = nullptr;

    connect(m_scraper, &Scraper::dataReady, this, &NetworkQueue::appendToLibrary);
    connect(this, &NetworkQueue::finished, &network_thread, &QThread::quit);
    connect(&network_thread, &QThread::started, this, &NetworkQueue::progressRequests);
    connect(m_scraper, &Scraper::progress, this, &NetworkQueue::progress);
    connect(m_scraper, &Scraper::label, this, &NetworkQueue::label);

}

NetworkQueue::~NetworkQueue()
{
}

void NetworkQueue::enqueueData(int id, QString title, QString system)
{
    Scraper::ScraperContext context;
    context.id = id;
    context.title = title;
    context.system = system;

    m_progress++;

    internal_queue.enqueue(context);

}

void NetworkQueue::setScraper(Scraper *scraper)
{
    m_scraper = scraper;
    m_scraper->moveToThread(&network_thread);
}

void NetworkQueue::enqueueContext(Scraper::ScraperContext context)
{
    internal_queue.enqueue(context);
}

void NetworkQueue::progressRequests()
{
    m_request_count = internal_queue.length();

    emit label("Dispatching Requests");
    emit progress(0.0);
    int temp_count = 0;
    m_scraper->setRequestCount(m_request_count);
    while (!internal_queue.isEmpty()) {
        emit progress((qreal)temp_count / m_request_count * 100.0);
        m_scraper->getGameData(internal_queue.dequeue());
        temp_count++;
    }
}

int NetworkQueue::requestCount()
{
   return m_request_count;
}

void NetworkQueue::setGameModel(GameLibraryModel *model)
{
    m_game_model = model;
}

void NetworkQueue::setLibraryDatabase(LibraryDbManager &dbm)
{
    db_manager = dbm;
}

void NetworkQueue::start()
{
    network_thread.start(QThread::LowPriority);
}

void NetworkQueue::appendToLibrary(Scraper::ScraperData *data)
{
    counter++;
    if (counter == 1) {
        emit progress(0.0);
        emit label("Attaching Artwork");
    }


    QSqlDatabase database = m_game_model->database();
    database.transaction();
    QSqlQuery q(database);

    q.prepare("UPDATE " + LibraryDbManager::table_games + " SET artwork = ? WHERE id = ?");

    q.addBindValue(data->front_boxart);
    q.addBindValue(data->libraryId);

    q.exec();

    emit ((qreal)counter / m_request_count * 100.0);

    if (counter == m_request_count) {
        database.commit();
        QMetaObject::invokeMethod(m_game_model, "select");

        emit label("");
        emit finished();
        m_request_count = 0;
        counter = 0;
    }

    delete data;


}
