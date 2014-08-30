
#ifndef PHOENIXLIBRARY_H
#define PHOENIXLIBRARY_H

#include <QObject>
#include <QDirIterator>
#include <QStringList>
#include <QCryptographicHash>
#include <QFile>
#include <QThread>
#include <QSqlQuery>
#include <QResource>
#include <QXmlStreamReader>

#include "thegamesdb.h"
#include "gamelibrarymodel.h"
#include "librarydbmanager.h"


class PhoenixLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)

public:
    PhoenixLibrary();

    ~PhoenixLibrary();

    void setFolderPath(QUrl path);
    void setProgress(qreal progress);
    void setLabel(QString label);

    QString label() const
    {
        return m_label;
    }

    qreal progress() const
    {
        return m_progress;
    }

public slots:
    void scanFolder(QUrl folder_path);
    void scrapeInfo();
    GameLibraryModel *model() { return m_model; }

signals:
    void labelChanged();
    void progressChanged();

private:
    LibraryDbManager dbm;
    QThread *import_thread;
    TheGamesDB *scraper;
    GameLibraryModel *m_model;
    QString m_label;
    int m_progress;

    QString getSystem(QString suffix);
    void loadXml(QString file_path);
    void addFilters(QStringList &filter_list);
    QRegularExpressionMatch parseFilename(QString filename);

};

#endif // PHOENIXLIBRARY_H
