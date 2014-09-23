
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

class PhoenixLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(Console);

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

    int count() const
    {
        return m_count;
    }

    enum Console {
        InvalidConsole = 0,
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
    void startAsyncScan(QUrl path);
    void scanFolder(QUrl folder_path);
    void scrapeInfo();
    void resetAll();
    GameLibraryModel *model() { return m_model; }
    void deleteRow(QString title);
    QString getSystem(QString system);
    QList<QObject *> coresModel(QString system);
    QStringList systemsModel();
    bool setPreferredCore(QString system, QString new_core);
    QString systemIcon(QString system);
    QString showPath(int index, QString system);



signals:
    void labelChanged();
    void progressChanged();
    void countChanged();

private:
    LibraryDbManager dbm;
    QThread *import_thread;
    TheGamesDB *scraper;
    GameLibraryModel *m_model;
    QString m_label;
    PlatformManager platform_manager;
    int m_progress;
    int m_count;

    QMap<QString, QList<QObject *>> cores_for_console;

    const QMap<Console, QString> m_consoles {
        { Atari_Lynx,         "Atari Lynx" },
        { IBM_PC,             "DOS" },
        { Nintendo_NES,       "Nintendo" },
        { Nintendo_SNES,      "Super Nintendo" },
        { Nintendo_Game_Boy,  "Game Boy Advance" },
        { Nintendo_GBA,       "Game Boy" },
        { Nintendo_DS,        "Nintendo DS" },
        { Sega_Master_System, "Sega Master System" },
        { Sega_Mega_Drive,    "Sega Mega Drive" },
        { Sega_Game_Gear,     "Sega Game Gear" },
        { Sega_CD,            "Sega CD" },
        { Sega_32X,           "Sega 32X" },
        { Sony_PlayStation,   "Sony PlayStation" },
        { Arcade,             "Arcade" },
        { FFMpeg,             "FIlm"},
    };



    const QMap<QString, QString> icon_for_console;

    QStringList excluded_consoles;

    QMap<Console, QVariantMap> core_for_console;

    QMap<QString, QVariantMap> core_for_extension;

    void loadXml(QString file_path);
    QRegularExpressionMatch parseFilename(QString filename);

};


#endif // PHOENIXLIBRARY_H
