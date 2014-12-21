#ifndef NETWORKQUEUE
#define NETWORKQUEUE

#include <QQueue>
#include <QThread>
#include <QNetworkReply>
#include <QUrl>
#include <QObject>
#include <atomic>

#include "librarydbmanager.h"
#include "gamelibrarymodel.h"
#include "scraper.h"

class NetworkQueue : public QObject {

    Q_OBJECT

public:
    explicit NetworkQueue();
    ~NetworkQueue();

    void setScraper(Scraper *scraper);
    void setGameModel(GameLibraryModel *model);
    void setLibraryDatabase(LibraryDbManager &dbm);
    int requestCount();


signals:
    void finished();
    void progress(qreal);
    void label(QString);

public slots:
    void enqueueContext(Scraper::ScraperContext context);
    void enqueueData(int id, QString title, QString system);
    void progressRequests();
    void start();

private slots:
    void appendToLibrary(Scraper::ScraperData *data);

private:
    QThread network_thread;
    QQueue<Scraper::ScraperContext> internal_queue;
    Scraper *m_scraper;
    GameLibraryModel *m_game_model;

    std::atomic<int> counter;
    std::atomic<int> m_request_count;
    bool m_finished = false;

};

#endif // NETWORKQUEUE
