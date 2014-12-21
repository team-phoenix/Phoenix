#include <QSqlDatabase>
#include <QSqlQuery>

#include "networkqueue.h"
#include "logging.h"
#include "thegamesdb.h"

NetworkQueue::NetworkQueue()
    : counter(0),
      m_request_count(0)
{

    setScraper(new TheGamesDB());

    this->moveToThread(&network_thread);

    m_scraper->moveToThread(&network_thread);

    m_game_model = nullptr;

    connect(this, &NetworkQueue::finished, &network_thread, &QThread::quit);
    connect(&network_thread, &QThread::started, this, &NetworkQueue::progressRequests);
    connect(m_scraper, &Scraper::progress, this, &NetworkQueue::progress);
    connect(m_scraper, &Scraper::label, this, &NetworkQueue::label);
    connect(m_scraper, &Scraper::dataReady, this, &NetworkQueue::appendToLibrary);
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

    internal_queue.enqueue(context);

}

void NetworkQueue::setScraper(Scraper *scraper)
{
    m_scraper = scraper;
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

    m_scraper->setRequestCount(m_request_count.load(std::memory_order_relaxed));
    while (!internal_queue.isEmpty()) {
        temp_count++;
        m_scraper->getGameData(internal_queue.dequeue());
        emit progress((qreal)temp_count / m_request_count * 100.0);

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

void NetworkQueue::start()
{
    network_thread.start(QThread::HighestPriority);
}

void NetworkQueue::appendToLibrary(Scraper::ScraperData *data)
{
    counter++;

    if (counter.load(std::memory_order_relaxed) == 1) {
        emit label("Attaching Artwork");
        emit progress(0.0);
    }


    QSqlDatabase database = m_game_model->database();
    database.transaction();
    QSqlQuery q(database);

    q.prepare("UPDATE " + LibraryDbManager::table_games + " SET artwork = ? WHERE id = ?");

    q.addBindValue(data->front_boxart);
    q.addBindValue(data->libraryId);
    q.exec();

    delete data;

    emit progress((qreal)counter.load(std::memory_order_relaxed) / m_request_count * 100.0);

    if (counter == m_request_count) {
        database.commit();
        QMetaObject::invokeMethod(m_game_model, "submitAll", Qt::QueuedConnection);
        counter = 0;
        m_request_count = 0;

        emit label("");
        emit finished();
    }
}
