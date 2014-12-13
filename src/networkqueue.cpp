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

    m_game_model = nullptr;

    connect(this, &NetworkQueue::finished, &network_thread, &QThread::quit);
    connect(&network_thread, &QThread::started, this, &NetworkQueue::progressRequests, Qt::DirectConnection);
    connect(&network_thread, &QThread::finished, this, &NetworkQueue::deleteLater);
}

NetworkQueue::~NetworkQueue()
{
    delete m_scraper;
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
    connect(m_scraper, &Scraper::dataReady, this, &NetworkQueue::appendToLibrary);

    qCDebug(phxLibrary) << QThread::currentThread() << network_thread.thread();

    while (!internal_queue.isEmpty()) {
        m_scraper->getGameData(internal_queue.dequeue());
    }
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
    qCDebug(phxLibrary) << "Game received: " << data->title << " On Platform: " << data->platform << " And artwork: " << data->front_boxart << " and: " << data->back_boxart;
    counter++;

    QSqlDatabase database = db_manager.handle();
    database.transaction();
    QSqlQuery q(database);

    q.prepare("UPDATE " + LibraryDbManager::table_games + " SET artwork = ? WHERE id = ?");

    q.addBindValue(data->front_boxart);
    q.addBindValue(data->libraryId);

    if (q.exec()) {
        database.commit();
        m_game_model->select();
    }

    delete data;


    if (internal_queue.length() > 0)
        emit ((qreal)counter / internal_queue.size() * 100.0);

    if (counter == internal_queue.length()) {
        emit finished();
    }

}
