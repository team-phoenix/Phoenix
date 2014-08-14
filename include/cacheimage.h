#ifndef CACHEIMAGE_H
#define CACHEIMAGE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQmlApplicationEngine>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QQuickItem>
#include <QImage>
#include <QDir>

class CImage : public QObject {
    Q_OBJECT
    //Remote url
    Q_PROPERTY(QString imgsrc READ imgsrc WRITE setImgsrc NOTIFY imgsrcChanged)
    //Local url after downloading and saving the image
    Q_PROPERTY(QString localsrc READ localsrc NOTIFY localsrcChanged)
    //Subfolder where the image is stored in
    Q_PROPERTY(QString folder READ folder WRITE setFolder NOTIFY folderChanged)
    QNetworkAccessManager manager;

public:
    explicit CImage(QObject *parent = 0);
    void doDownload(const QUrl &url);
    QStringList saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    void setImgsrc(const QString &imgsrc);
    QString imgsrc();
    QString localsrc();
    QString folder();
    void setFolder(const QString &folder);
    Q_INVOKABLE void start();

private:
    QString m_localPath;
    QString m_localUrl;
    QString m_src;
    QString m_folder;

public slots:
    void downloadFinished(QNetworkReply *reply);
    void cacheAndReturn();
signals:
    void imgsrcChanged();
    void localsrcChanged();
    void folderChanged();
};
#endif // CACHEIMAGE_H
