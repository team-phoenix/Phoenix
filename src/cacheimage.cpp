#include "cacheimage.h"

CImage::CImage(QObject *parent)
    : QObject(parent)
{
    // signal/slot when manager object emits finished signal execute downloadFinished method
    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}


void CImage::returnCached(QUrl imgUrl)
{
    QQmlApplicationEngine eng;
    //Store the qml app offline storage location
    //In android it's application path/files/QML/OfflineStorage/
    m_localPath = QDir::fromNativeSeparators(eng.offlineStoragePath() + "\\");

    if(m_folder != "")
        m_localPath += m_folder + "/";

    QDir dir(m_localPath);
    if(!dir.exists()) {
        if(!dir.mkpath(m_localPath)) {
            qDebug() << "Couldn't create subdirectory";
            return;
        }
    }

    if(!QFile::exists(m_localPath + m_filename)) { // This check seems to be taking a long time to do for each file
        //Create the request to be sent with Network Access Maanager
        emit cacheLoaded(false);
        QNetworkRequest req(imgUrl);
        // Reply will contain header and data and emits signal finished()
        // that will be listened by constructor connector
        manager.get(req);
    }
    else {
        //qDebug() << "exists: " << m_localPath + m_filename;
        //The file is already downloaded, let's just emit the local path
        m_localUrl = "file:///" + m_localPath + m_filename;
        emit cacheLoaded(true);
        emit localsrcChanged();
    }
}

void CImage::cacheImage()
{
    if(m_src.isEmpty()) {
        m_localUrl = "";
        emit localsrcChanged();
        return;
    }

    QUrl imgUrl(m_src);
    //Do following when url
    if(imgUrl.url().startsWith("http://") || imgUrl.url().startsWith("https://")) {
        returnCached(imgUrl);
    }

    else {
        //The file url is local already.
        m_localUrl = m_src;
        emit localsrcChanged();
    }
}
void CImage::downloadFinished(QNetworkReply *reply)
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
        if(filename[0] == "true") {
            //Saves the file to disk
            if (saveToDisk(filename[1], reply)) {
                m_localUrl = "file:///" + filename[1];
                emit localsrcChanged();
            }
        }
        else {
            m_localUrl = "file:///" + filename[1];
            emit localsrcChanged();
        }
    }
}

bool CImage::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open %s for writing: " << filename << file.errorString();
    return false;
    }
    file.write(data->readAll());
    file.close();
    if(!file.exists())
        qDebug() << "For some reason saving the image wasn't successful." << filename;
    return true;
}

QStringList CImage::saveFileName()
{
    QString basename = m_localPath + m_filename;
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

QString CImage::fileName()
{
    return m_filename;
}

void CImage::setFileName(const QString &fileName)
{
    m_filename = fileName + ".jpg";
    emit fileNameChanged();
}

QString CImage::imgsrc()
{
    return m_src;
}

void CImage::setFolder(const QString &folder)
{
    if(m_folder != folder && folder != "") {
        m_folder = folder;
        emit folderChanged();
    }
}

QString CImage::folder()
{
    return m_folder;
}

void CImage::setImgsrc(const QString &imgsrc)
{
    if(m_src != imgsrc && imgsrc != "") {
        m_src = imgsrc;
        emit imgsrcChanged();
    }
}

QString CImage::localsrc()
{
    return m_localUrl;
}

void CImage::start()
{
    cacheImage();
}
