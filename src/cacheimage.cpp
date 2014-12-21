
#include "cacheimage.h"
#include <QFileInfo>

CachedImage::CachedImage(QObject *parent)
    : QObject(parent)
{
    // signal/slot when manager object emits finished signal execute downloadFinished method
    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));


}

void CachedImage::returnCached(QUrl imgUrl)
{

    //Store the qml app offline storage location
    //In android it's application path/files/QML/OfflineStorage/

    if(m_folder != "")
        setCacheDirectory(m_cache_directory + m_folder + "/");

    QDir dir(cacheDirectory());
    if (!dir.exists()) {
        if (!dir.mkpath(cacheDirectory())) {
            qDebug() << "Couldn't create subdirectory";
            return;
        }
    }

    if(!QFile::exists(cacheDirectory() + m_filename)) { // This check seems to be taking a long time to do for each file
        //Create the request to be sent with Network Access Maanager
        emit cacheLoaded(false);
        QNetworkRequest req(imgUrl);
        // Reply will contain header and data and emits signal finished()
        // that will be listened by constructor connector
        manager.get(req);
    } else {
        //qDebug() << "exists: " << cacheDirectory() + m_filename;
        //The file is already downloaded, let's just emit the local path
        m_localUrl = "file:///" + cacheDirectory() + m_filename;
        emit cacheLoaded(true);
        emit localsrcChanged();
    }
}

void CachedImage::cacheImage()
{
    if (m_src.isEmpty()) {
        m_localUrl = "";
        emit localsrcChanged();
        return;
    }

    QUrl imgUrl(m_src);
    QFileInfo info_src(m_src);
    //m_suffix = info_src.suffix();
    setFileName(m_filename + "." + info_src.suffix());

    //Do following when url
    if (imgUrl.url().startsWith("http://") || imgUrl.url().startsWith("https://")) {
        returnCached(imgUrl);
    } else {
        //The file url is local already.
        m_localUrl = m_src;
        emit localsrcChanged();
    }
}

void CachedImage::downloadFinished(QNetworkReply *reply)
{
    QStringList filename;
    if (reply->error()) {
        qDebug() << "Something went worng on download";
        m_localUrl = "";
        emit localsrcChanged();
    }
    else {
        //Create a filename off the url
        filename = saveFileName();
        if (filename[0] == "true") {
            //Saves the file to disk
            if (saveToDisk(filename[1], reply)) {
                m_localUrl = "file:///" + filename[1];
                emit localsrcChanged();
            }
        } else {
            m_localUrl = "file:///" + filename[1];
            emit localsrcChanged();
        }
    }
    reply->deleteLater();
}

bool CachedImage::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open %s for writing: " << filename << file.errorString();
        return false;
    }
    file.write(data->readAll());
    file.close();
    if (!file.exists())
        qDebug() << "For some reason saving the image wasn't successful." << filename;
    return true;
}

QStringList CachedImage::saveFileName()
{
    QString basename = cacheDirectory() + m_filename;
    QStringList result;
    /*
    * the result stringlist contains of 2 elements that need to be returned
    * 0: true/false if we need to download the picture
    * 1: local full image path
    */
    result << "true" << basename;
    if (basename.isEmpty()) {
        qDebug() << "File name is missing, abort!";
        exit(0);
    }
    if (QFile::exists(basename)) {
        result[0] = "false";
    }
    return result;
}

QString CachedImage::cacheDirectory()
{
    return m_cache_directory;
}

QString CachedImage::fileName()
{
    return m_filename;
}

QString CachedImage::suffix()
{
    return m_suffix;
}

void CachedImage::setCacheDirectory(const QString &cacheDirectory)
{
    m_cache_directory = cacheDirectory;
    emit cacheDirectoryChanged();
}

void CachedImage::setFileName(const QString &fileName)
{
    m_filename = fileName;
    emit fileNameChanged();
}

QString CachedImage::imgsrc()
{
    return m_src;
}

void CachedImage::setFolder(const QString &folder)
{
    if (m_folder != folder && folder != "") {
        m_folder = folder;
        emit folderChanged();
    }
}

QString CachedImage::folder()
{
    return m_folder;
}

void CachedImage::setImgsrc(const QString &imgsrc)
{
    if (m_src != imgsrc && imgsrc != "") {
        m_src = imgsrc;
        emit imgsrcChanged();
    }
}

QString CachedImage::localsrc()
{
    return m_localUrl;
}

void CachedImage::start()
{
    cacheImage();
}
