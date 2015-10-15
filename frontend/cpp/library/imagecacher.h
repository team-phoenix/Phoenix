#ifndef IMAGECACHER_H
#define IMAGECACHER_H

#include "frontendcommon.h"

#include "logging.h"
#include "phxpaths.h"

namespace  Library {

#if defined(Q_OS_WIN)
    static const QString qmlFilePrefix = QStringLiteral( "file:///" );
#else
    static const QString qmlFilePrefix = QStringLiteral( "file://" );
#endif
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
            void handleSSLErrors( const QList<QSslError> &errors );

        signals:
            void cachedUrlChanged();
            void imageUrlChanged();
            void identifierChanged();
    };
}

#endif // IMAGECACHER_H
