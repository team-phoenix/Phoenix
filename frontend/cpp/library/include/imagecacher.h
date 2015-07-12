#ifndef IMAGECACHER_H
#define IMAGECACHER_H

#include <QQuickImageProvider>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QApplication>

namespace  Library {

    static const QString cacheDirectory = QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
                                        + QDir::separator()
                                        + QStringLiteral( "Phoenix" )
                                        + QDir::separator()
                                        + QStringLiteral( "Artwork" );

    static const QString qmlFilePrefix = QStringLiteral( "file://" );


    class ImageCacher : public QObject {
            Q_OBJECT

            Q_PROPERTY( QUrl cachedUrl READ cachedUrl WRITE setCachedUrl NOTIFY cachedUrlChanged )
            Q_PROPERTY( QUrl imageUrl READ imageUrl WRITE setImageUrl NOTIFY imageUrlChanged )
            Q_PROPERTY( QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged )


        public:
            explicit ImageCacher( QObject *parent = 0 );

            QUrl cachedUrl() const;
            QUrl imageUrl() const;
            QString identifier() const;

            void setIdentifier( const QString id );

            void setImageUrl( const QUrl url );

            void setCachedUrl( const QUrl url );

        private:
            QNetworkAccessManager mNetworkManager;

            QUrl qmlCachedUrl;
            QUrl qmlImageUrl;
            QString qmlIdentifier;

            QString mImageType;

        public slots:
            void cache();

        private slots:

            void handleRequest( QNetworkReply *reply );
            void handleRequestError( QNetworkReply::NetworkError );
            void handleRequestProgress( qint64 bytesRecieved, qint64 bytesTotal );

        signals:
            void cachedUrlChanged();
            void imageUrlChanged();
            void identifierChanged();
    };
}

#endif // IMAGECACHER_H
