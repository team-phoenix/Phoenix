
#ifndef CACHEIMAGE_H
#define CACHEIMAGE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QQuickItem>
#include <QImage>
#include <QDir>


class CachedImage : public QObject {
        Q_OBJECT
    public:
        CachedImage();
        ~CachedImage();
        void doDownload( const QUrl &url );
        bool saveToDisk( const QString &filename, QIODevice *data );

        QString folder();
        QString cacheDirectory();

        void setFolder( const QString &folder );
        void setCacheDirectory( const QString &cacheDirectory );

    private:
        QString m_cache_directory;
        QString m_folder;

        QNetworkAccessManager *manager;

    public slots:
        void downloadFinished( QNetworkReply *reply );
        void cacheImage( QUrl url,  QString file_alias, int library_id );
        void returnCached( QUrl &imgUrl,  QString &file_alias, int library_id );

    signals:

        void start();
        void finished( QString cached_url, int library_id );
};

#endif // CACHEIMAGE_H
