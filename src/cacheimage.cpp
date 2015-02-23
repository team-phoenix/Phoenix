
#include "cacheimage.h"
#include <QFileInfo>
#include <QThread>

CachedImage::CachedImage()
{
    // signal/slot when manager object emits finished signal execute downloadFinished method
    manager = new QNetworkAccessManager(this);
    connect( manager, &QNetworkAccessManager::finished, this, &CachedImage::downloadFinished);
    setFolder("Artwork");

}

CachedImage::~CachedImage()
{
    delete manager;
}

void CachedImage::returnCached(QUrl &imgUrl, QString &file_alias, int library_id) {

    //Store the qml app offline storage location
    //In android it's application path/files/QML/OfflineStorage/

    QDir dir( cacheDirectory() + folder());

    if( !dir.exists() ) {
        if( !dir.mkpath( cacheDirectory() + folder()) ) {
            qDebug() << "Couldn't create subdirectory";
            return;
        }
    }


    QFileInfo url_info(imgUrl.toString());

    QString local_file = cacheDirectory() + folder() + file_alias + "." + url_info.suffix();
    if( !QFile::exists(local_file) ) { // This check seems to be taking a long time to do for each file
        //Create the request to be sent with Network Access Maanager

        QNetworkRequest req( imgUrl );
        // Reply will contain header and data and emits signal finished()
        // that will be listened by constructor connector
        QNetworkReply *reply = manager->get(req);

        reply->setProperty("library_id", QVariant(library_id));
        reply->setProperty("fileAlias", local_file);

    }

    else {
        // Cache was already made, no reason to refetch.
        emit finished(local_file, library_id);
    }


}

void CachedImage::cacheImage(QUrl url,  QString file_alias, int library_id) {
    if ( url.url().startsWith( "http://" ) || url.url().startsWith( "https://" ) ) {
        returnCached(url, file_alias, library_id);
    }
}

void CachedImage::downloadFinished(QNetworkReply *reply) {

    if( !reply->error() ) {
        QString cache_file = reply->property("fileAlias").toString();
        if (!saveToDisk(cache_file, reply))
            qDebug() << "saving file to disk failed for: " << cache_file;
        else {
            emit finished(cache_file, reply->property("library_id").toInt());
            reply->deleteLater();
            return;
        }


    }

    qDebug() << "Network reply from cacher errored out: " << reply->errorString();
    reply->deleteLater();
    emit finished("", -1);

}

bool CachedImage::saveToDisk( const QString &filename, QIODevice *data ) {
    QFile file( filename );

    if( !file.open( QIODevice::WriteOnly ) ) {
        qDebug() << "Could not open %s for writing: " << filename << file.errorString();
        return false;
    }

    file.write( data->readAll() );
    file.close();

    if( !file.exists() ) {
        qDebug() << "For some reason saving the image wasn't successful." << filename;
    }

    return true;
}


QString CachedImage::cacheDirectory() {
    return m_cache_directory;
}


void CachedImage::setCacheDirectory( const QString &cacheDirectory ) {
    m_cache_directory = cacheDirectory;
}


void CachedImage::setFolder( const QString &folder ) {
    if( m_folder != folder && folder != "" ) {
        m_folder = folder;
    }
}

QString CachedImage::folder() {
    return m_folder + "/";
}
