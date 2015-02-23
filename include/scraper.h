#ifndef SCRAPER
#define SCRAPER

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QString>

#include "QObject"
#include <atomic>


class Scraper : public QObject {
        Q_OBJECT
    public:
        explicit Scraper( QObject *parent = 0 );
        ~Scraper();

        enum QueryState {
            None,
            RequestingId,
            RequestingData,
            Finishing
        };

        struct ScraperData {

            int libraryId;
            QString libraryName;
            QString librarySystem;
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

        struct ScraperContext {
            int id = -1;
            QString title;
            QString system;
        };

        QNetworkAccessManager *networkManager();
        QString baseUrl();
        QStringList expressions();


        void setHitRatio( qreal ratio );
        void setBaseUrl( QString base_url );
        void setRequestCount( int count );
        QString cleanString( QString string );

        qreal hit_ratio;
        int request_count;
        std::atomic<int> current_count;


    signals:
        void dataReady(Scraper::ScraperData *data);
        void progress(int val);
        void label(QString text);

    public slots:
        virtual void processRequest( QNetworkReply *reply);
        virtual void getGameData( Scraper::ScraperContext context);
        virtual void handleNetworkChanged(QNetworkAccessManager::NetworkAccessibility accessible);

    private:
        QNetworkAccessManager *network_manager;
        QString m_base_url;
        QStringList m_expressions;

};

#endif
