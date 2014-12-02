#ifndef PHOENIXLIBRARY_H
#define PHOENIXLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QThread>

#include "thegamesdb.h"
#include "gamelibrarymodel.h"
#include "librarydbmanager.h"
#include "platformmanager.h"
#include "coremodel.h"
#include "systemdatabase.h"

class PhoenixLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(Console);
    Q_PROPERTY(bool importUrls READ importUrls WRITE setImportUrls NOTIFY importUrlsChanged)

public:
    PhoenixLibrary();

    ~PhoenixLibrary();

    void setFolderPath(QUrl path);
    void setProgress(qreal progress);
    void setLabel(QString label);
    void setImportUrls(bool importUrls);

    QString label() const
    {
        return m_label;
    }

    qreal progress() const
    {
        return m_progress;
    }

    int count() const
    {
        return m_count;
    }

    bool importUrls()
    {
        return m_import_urls;
    }

    enum Console {
        InvalidConsole = 0,
        All,
        Atari_Lynx,
        IBM_PC,
        Nintendo_NES,
        Nintendo_SNES,
        Nintendo_Game_Boy,
        Nintendo_GBA,
        Nintendo_DS,
        Sega_Master_System,
        Sega_Mega_Drive,
        Sega_Game_Gear,
        Sega_CD,
        Sega_32X,
        Sony_PlayStation,
        Arcade,
        FFMpeg,
        Last
    };

    // Can't use template parameters with Q_PROPERTY
    typedef QMap<Console, QString> ConsoleMap;

    Q_PROPERTY(ConsoleMap consoles READ consoles CONSTANT)
    ConsoleMap consoles() const
    {
        return {};
    }

public slots:
    void handleOnlineDatabaseResponse(GameData* data);
    void startAsyncScan(QUrl path);
    //GameData *scrapeInfo(QString name, QString system);
    //GameData *asyncScrapeInfo(QString name, QString system);
    void resetAll();
    GameLibraryModel *model() { return m_model; }
    void deleteRow(QString title);
    QString getSystem(QString system);
    QList<QObject *> coresModel(QString system);
    QStringList systemsModel();
    bool setPreferredCore(QString system, QString new_core);
    QString systemIcon(QString system);
    QString showPath(int index, QString system);
    void cacheUrls(QList<QUrl> list);
    void importDroppedFiles();



signals:
    void labelChanged();
    void progressChanged();
    void countChanged();
    void importUrlsChanged();

private:
    LibraryDbManager dbm;
    QThread *import_thread;
    //TheGamesDB scraper;
    GameLibraryModel *m_model;
    QString m_label;
    PlatformManager platform_manager;
    int m_progress;
    int m_count;
    QList<QUrl> file_urls;
    bool m_import_urls;

    SystemDatabase system_db;

    QMap<QString, QList<QObject *>> cores_for_console;

    QMap<Console, QVariantMap> core_for_console;

    const QMap<QString, QString> icon_for_console;

    const QMap<Console, QString> m_consoles;

    QStringList excluded_consoles;
    TheGamesDB* thegamesdb;

    QMap<QString, QVariantMap> core_for_extension;

    void loadXml(QString file_path);
    QRegularExpressionMatch parseFilename(QString filename);
    QByteArray generateSha1Sum(QString file);
    bool insertGame(QSqlQuery &q, QFileInfo path);
    QVector<int> scanFolder(QUrl folder_path);
    void importMetadata(QVector<int> games_id);
    void scanSystemDatabase(QByteArray hash, QString &name, QString &system);
};


#endif // PHOENIXLIBRARY_H
