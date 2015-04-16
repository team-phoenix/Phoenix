#include "scraper.h"
#include "logging.h"
#include <QStringList>

Scraper::Scraper( QObject *parent )
    : QObject( parent ),
      current_count( 0 ) {
    hit_ratio = 3 / 4.0;
    request_count = 0;
    m_expressions = ( QStringList() << "-"
                      << "\\"
                      << "/"
                      << "!"
                      << "."
                      << "?"
                      << ":"
                    );
    network_manager = new QNetworkAccessManager( this );
    connect( network_manager, &QNetworkAccessManager::finished, this, &Scraper::processRequest );
    connect( network_manager, &QNetworkAccessManager::networkAccessibleChanged, this, &Scraper::handleNetworkChanged );

}

Scraper::~Scraper() {
    delete network_manager;
}

void Scraper::setHitRatio( qreal ratio ) {
    hit_ratio = ratio;
}

void Scraper::setRequestCount( int count ) {
    request_count = count;
}

QStringList Scraper::expressions() {
    return m_expressions;
}

QNetworkAccessManager *Scraper::networkManager() {
    return network_manager;
}

void Scraper::setBaseUrl( QString base_url ) {
    m_base_url = base_url;
}

QString Scraper::baseUrl() {
    return m_base_url;
}

QString Scraper::cleanString( QString string ) {
    QStringList str_list = string.split( " " );

    for( int i = 0; i < str_list.length(); ++i ) {
        if( str_list.at( i ).contains( "(" ) ) {
            if( str_list.at( i ).contains( ")" ) ) {
                str_list.removeAt( i );
            } else {
                str_list.removeAll( "(" );
            }
        }
    }

    QString new_str = str_list.join( " " );

    for( int i = 0; i < expressions().length(); i++ ) {
        new_str.remove( expressions().at( i ) );
    }

    QString stringNormalized = new_str.normalized( QString::NormalizationForm_KD );
    stringNormalized.remove( QRegExp( "[^a-zA-Z\\s]" ) );

    return stringNormalized.toLower();
}

void Scraper::processRequest( QNetworkReply *reply ) {
    Q_UNUSED( reply );
}

void Scraper::getGameData( Scraper::ScraperContext context ) {
    Q_UNUSED( context );
}

void Scraper::handleNetworkChanged( QNetworkAccessManager::NetworkAccessibility accessible ) {
    QString result;

    if( accessible == QNetworkAccessManager::NotAccessible ) {
        result = "The Network isn't working";
    }

    else if( accessible == QNetworkAccessManager::Accessible ) {
        result = "The Network is running";
    }

    else {
        result = "The Network is unknown";
    }

    qDebug() << result;
    emit label( result );


}

