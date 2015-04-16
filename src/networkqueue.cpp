#include <QSqlDatabase>
#include <QSqlQuery>

#include "networkqueue.h"
#include "logging.h"
#include "thegamesdb.h"

NetworkQueue::NetworkQueue()
    : counter( 0.0 ),
      m_request_count( 0 ) {

    setScraper( new TheGamesDB() );

    //this->moveToThread( &network_thread );

    m_scraper->moveToThread( &network_thread );

    m_game_model = nullptr;
    image_cacher = new CachedImage();
    image_cacher->moveToThread( &network_thread );

    connect( this, &NetworkQueue::finished, &network_thread, &QThread::quit );
    connect( &network_thread, &QThread::finished, image_cacher, &CachedImage::deleteLater );
    connect( &network_thread, &QThread::started, this, &NetworkQueue::progressRequests, Qt::DirectConnection );
    connect( m_scraper, &Scraper::progress, this, &NetworkQueue::setProgress, Qt::DirectConnection );
    connect( m_scraper, &Scraper::label, this, &NetworkQueue::setLabel, Qt::DirectConnection );
    connect( m_scraper, &Scraper::dataReady, this, &NetworkQueue::appendToLibrary );
    connect( this, &NetworkQueue::requestArtwork, image_cacher, &CachedImage::cacheImage );
    connect( image_cacher, &CachedImage::finished, this, &NetworkQueue::updateUrl );

}

NetworkQueue::~NetworkQueue() {
}

void NetworkQueue::setProgress( int value ) {
    emit progress( value );
}

void NetworkQueue::setLabel( QString current_label ) {
    qDebug() << "current label: " << current_label;
    emit label( current_label );
}

void NetworkQueue::enqueueData( int id, QString title, QString system ) {
    Scraper::ScraperContext context;
    context.id = id;
    context.title = title;
    context.system = system;

    internal_queue.enqueue( context );

}

void NetworkQueue::setScraper( Scraper *scraper ) {
    m_scraper = scraper;
}

void NetworkQueue::enqueueContext( Scraper::ScraperContext context ) {
    internal_queue.enqueue( context );
}

void NetworkQueue::progressRequests() {
    m_request_count = internal_queue.length();

    emit label( "Dispatching Requests" );
    emit progress( 0.0 );
    int temp_count = 0.0;

    m_scraper->setRequestCount( m_request_count );

    while( !internal_queue.isEmpty() ) {
        temp_count++;
        m_scraper->getGameData( internal_queue.dequeue() );
        emit progress( temp_count / m_request_count * 100 );

    }
}

int NetworkQueue::requestCount() {
    return m_request_count;
}

void NetworkQueue::setGameModel( GameLibraryModel *model ) {
    m_game_model = model;
}

void NetworkQueue::start() {
    network_thread.start( QThread::HighestPriority );
}

void NetworkQueue::appendToLibrary( Scraper::ScraperData *data ) {
    counter++;

    if( counter.load( std::memory_order_relaxed ) == 1 ) {
        emit label( "Scraping Artwork" );
        emit progress( 0.0 );
    }


    QSqlDatabase database = m_game_model->database();
    database.transaction();
    QSqlQuery q( database );

    q.prepare( "UPDATE " + LibraryDbManager::table_games + " SET artwork = ? WHERE id = ?" );

    q.addBindValue( data->front_boxart );
    q.addBindValue( data->libraryId );
    q.exec();

    delete data;

    int prog = static_cast<int>( counter.load( std::memory_order_relaxed ) / m_request_count * 100.0 );
    emit progress( prog );

    if( counter == m_request_count ) {
        counter = 0.0;

        emit label( "Caching Artwork" );
        //emit finished();
        q.prepare( "SELECT artwork, id, sha1 FROM " + LibraryDbManager::table_games );

        if( !q.exec() ) {
            qDebug() << q.executedQuery() << "Error with query";
        }

        while( q.next() ) {
            QUrl url = q.value( 0 ).toString();
            int id = q.value( 1 ).toInt();
            QString sha1 = QString( q.value( 2 ).toByteArray() );
            //qDebug() << "Sha1: " << sha1;
            emit requestArtwork( url, sha1, id );

        }

    }
}

void NetworkQueue::setCacheDirectory( QString cache_dir ) {
    m_cache_directory = cache_dir;
    image_cacher->setCacheDirectory( m_cache_directory );
}

void NetworkQueue::updateUrl( QString cached_url, int library_id ) {
    counter++;

    if( counter.load( std::memory_order_relaxed ) == 1 ) {
        emit progress( 0 );
    }

    if( cached_url == "" && library_id == -1 ) {
        return;
    }


    QSqlDatabase database = m_game_model->database();
    database.transaction();
    QSqlQuery q( database );

    q.prepare( "UPDATE " + LibraryDbManager::table_games + " SET artwork = ? WHERE id = ?" );

    q.addBindValue( "file:///" + cached_url );
    q.addBindValue( library_id );

    if( !q.exec() ) {
        qDebug() << "Error with exec: " << q.lastQuery();
    }

    int prog = ( int )( counter.load( std::memory_order_relaxed ) / m_request_count * 100.0 );
    emit progress( prog );

    if( counter == m_request_count ) {
        database.commit();
        QMetaObject::invokeMethod( m_game_model, "submitAll", Qt::QueuedConnection );

        counter = 0.0;
        m_request_count = 0;
        emit label( "" );
        emit finished();
    }

}


