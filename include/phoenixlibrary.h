
#ifndef PHOENIXLIBRARY_H
#define PHOENIXLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QThread>

#include "thegamesdb.h"
#include "gamelibrarymodel.h"
#include "librarydbmanager.h"


class PhoenixLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
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
        Sony_PlayStation
    };
    // Can't use template parameters with Q_PROPERTY
    typedef QMap<Console, QString> ConsoleMap;

    Q_PROPERTY(ConsoleMap consoles READ consoles CONSTANT)
    ConsoleMap consoles() const
    {
        return {};
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

    const QMap<Console, QString> m_consoles {
        { Atari_Lynx,         "Atari Lynx" },
        { IBM_PC,             "DOSBox" },
        { Nintendo_NES,       "Nintendo" },
        { Nintendo_SNES,      "Super Nintendo" },
        { Nintendo_Game_Boy,  "Game Boy Advance" },
        { Nintendo_GBA,       "Game Boy Color" },
        { Nintendo_DS,        "Nintendo DS" },
        { Sega_Master_System, "Sega Master System" },
        { Sega_Mega_Drive,    "Sega Mega Drive" },
        { Sega_Game_Gear,     "Sega Game Gear" },
        { Sega_CD,            "Sega CD" },
        { Sega_32X,           "Sega 32X" },
        { Sony_PlayStation,   "Sony PlayStation" },
    };

    QMap<Console, QVariantMap> core_for_console;

    QMap<QString, QVariantMap> core_for_extension;

    QString getSystem(QString suffix);
    void loadXml(QString file_path);
    QRegularExpressionMatch parseFilename(QString filename);

};


#endif // PHOENIXLIBRARY_H
