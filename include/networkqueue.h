#ifndef NETWORKQUEUE
#define NETWORKQUEUE

#include <QQueue>
#include <QThread>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QObject>

#include "librarydbmanager.h"
#include "gamelibrarymodel.h"
#include "cacheimage.h"
#include "scraper.h"

class NetworkQueue : public QObject {

        Q_OBJECT

    public:
        explicit NetworkQueue();
        ~NetworkQueue();

        void setScraper( Scraper *scraper );
        void setGameModel( GameLibraryModel *model );
        void setLibraryDatabase( LibraryDbManager &dbm );
        int requestCount();
        void setCacheDirectory(QString cache_dir);


    signals:
        void finished();
        void progress(int val);
        void label(QString text);
        void requestArtwork(QUrl url, QString file_name, int id);

    public slots:
        void enqueueContext( Scraper::ScraperContext context );
        void enqueueData( int id, QString title, QString system );
        void progressRequests();
        void start();
        void setProgress(int progress);
        void setLabel(QString label);

    private slots:
        void appendToLibrary( Scraper::ScraperData *data );
        void updateUrl(QString cached_url, int library_id);

    private:
        QThread network_thread;
        QQueue<Scraper::ScraperContext> internal_queue;
        Scraper *m_scraper;
        GameLibraryModel *m_game_model;
        CachedImage *image_cacher;

        QString m_cache_directory;

        std::atomic_int counter;
        int m_request_count;
        bool m_finished = false;


};

#endif // NETWORKQUEUE
