
#include <QRegularExpression>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QResource>
#include <QXmlStreamReader>
#include <QFile>
#include <QtConcurrent>
#include <QSqlError>
#include <QApplication>
#include <QCryptographicHash>

#include "phoenixlibrary.h"
#include "librarydbmanager.h"
#include "libretro_cores_info.h"
#include "logging.h"


PhoenixLibrary::PhoenixLibrary()
    : core_for_console(QMap<PhoenixLibrary::Console,QVariantMap> {
        { Nintendo_NES,       libretro_cores_info[platform_manager.preferred_cores[platform_manager.nintendo]] },
        { Nintendo_SNES,      libretro_cores_info[platform_manager.preferred_cores[platform_manager.super_nintendo]] },
        { Nintendo_Game_Boy,  libretro_cores_info[platform_manager.preferred_cores[platform_manager.gameboy]] },
        { Nintendo_GBA,       libretro_cores_info[platform_manager.preferred_cores[platform_manager.gameboy_advance]] },
        { Sony_PlayStation,   libretro_cores_info[platform_manager.preferred_cores[platform_manager.playstation]] },
    }),

    icon_for_console(QMap<QString, QString>{
        { platform_manager.nintendo, "/assets/consoleicons/nes.png" },
        { platform_manager.super_nintendo, "/assets/consoleicons/snes.png" },
        { platform_manager.gameboy, "/assets/consoleicons/gbc.png" },
        { platform_manager.playstation, "/assets/consoleicons/ps1.png" },
        { platform_manager.gameboy_advance, "/assets/consoleicons/gba.png" },
        { platform_manager.dos, "/assets/consoleicons/dosbox.png" },
        { platform_manager._3do, "/assets/consoleicons/3do.png" },
        { platform_manager.atari_7800, "/assets/consoleicons/7800.png" },
        { platform_manager.gameboy, "/assets/consoleicons/gbc.png" },
        { platform_manager.sega_saturn, "/assets/consoleicons/saturn.png" },
        { platform_manager.nintendo_ds, "/assets/consoleicons/nds.png" },
        { platform_manager.atari_lynx, "/assets/consoleicons/lynx.png" },
        { platform_manager.video, "" }
    }),
    m_consoles(QMap<PhoenixLibrary::Console,QString>{
        { All, "All" },
        { Atari_Lynx, "Atari Lynx" },
        { IBM_PC, "DOS" },
        { Nintendo_NES, "Nintendo" },
        { Nintendo_SNES, "Super Nintendo" },
        { Nintendo_Game_Boy, "Game Boy" },
        { Nintendo_GBA, "Game Boy Advance" },
        { Nintendo_DS, "Nintendo DS" },
        { Sega_Master_System, "Sega Master System" },
        { Sega_Mega_Drive, "Sega Mega Drive" },
        { Sega_Game_Gear, "Sega Game Gear" },
        { Sega_CD, "Sega CD" },
        { Sega_32X, "Sega 32X" },
        { Sony_PlayStation, "Sony PlayStation" },
        { Arcade, "Arcade" },
        { FFMpeg, "Film" },
    })
    //model: ListModel {
    //    ListElement {title: "All"; icon: "qrc:/assets/more.png";}
    //    ListElement {title: "Atari Lynx"; icon: "qrc:/assets/consoleicons/lynx.png";}
     //   ListElement {title: "Nintendo"; icon: "/assets/consoleicons/nes.png";}
     //   ListElement {title: "Super Nintendo"; icon: "/assets/consoleicons/snes.png";}
     //   ListElement {title: "Sony PlayStation"; icon: "/assets/consoleicons/ps1.png";}
    //    ListElement {title: "Game Boy Advance"; icon: "/assets/consoleicons/gba.png";}
     //   ListElement {title: "Game Boy Color"; icon: "/assets/consoleicons/gbc.png";}
    //    ListElement {title: "Nintendo DS"; icon: "/assets/consoleicons/nds.png";}
    //    ListElement {title: "DOSBox"; icon: "/assets/consoleicons/dosbox.png";}
    //}
{
    m_import_urls = false;

    excluded_consoles = QStringList() << platform_manager.nintendo_ds << platform_manager.mupen64plus << platform_manager.ppsspp
                                      << platform_manager.desmume;

    m_model = new GameLibraryModel(&dbm, this);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);


    for (auto &core: libretro_cores_info) {
        QString exts = core["supported_extensions"].toString();
        if (exts.isEmpty())
            continue;

        if (core_for_console.key(core) == InvalidConsole)
            continue; // core not in core_for_console map

        for (QString &ext : exts.split("|")) {
            if (ext.isEmpty())
                continue;


            if (core_for_extension.contains(ext)) {
                QDebug dbg = qWarning(phxLibrary);
                dbg << "Multiple cores for extension" << ext << ":";
                for (auto &c : core_for_extension.values(ext)) {
                    dbg << c["display_name"].toString();
                    qCDebug(phxLibrary) << "EXTENSION: " << ext;

                }

                dbg << "and" << core["display_name"].toString();
            }

            core_for_extension.insertMulti(ext, core);
        }
    }

    for (auto &core : libretro_cores_info.keys()) {

        QString system = libretro_cores_info[core]["systemname"].toString();
        QString cleaned_name = platform_manager.cleaned_system_name.value(system, system);
        QString display_name = libretro_cores_info[core].value("corename", "").toString();

        cores_for_console[cleaned_name].append( new CoreModel(this, display_name, core));

    }


    /*connect(import_thread, SIGNAL(started()), this, SLOT(scanFolder()));
    connect(this, SIGNAL(destroyed()), import_thread, SLOT(deleteLater()));
    connect(import_thread, SIGNAL(finished()), import_thread, SLOT(deleteLater()));*/
}

PhoenixLibrary::~PhoenixLibrary()
{

    if (m_model)
        m_model->deleteLater();

}

void PhoenixLibrary::setLabel(QString label)
{
    m_label = label;
    emit labelChanged();
}

void PhoenixLibrary::setProgress(qreal progress)
{
    m_progress = progress;
    emit progressChanged();
}

void PhoenixLibrary::loadXml(QString file_path)
{
    QResource resource(file_path);
    QFile in_file(resource.absoluteFilePath());
    if (in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader reader;
        reader.setDevice(&in_file);
        while (!reader.isEndDocument()) {
            reader.readNext();
            QString element = reader.name().toString();
            qCDebug(phxLibrary) << element;
            if (element == "name")

                qCDebug(phxLibrary) << reader.readElementText();
        }

        if (reader.hasError()) {
            qCDebug(phxLibrary) << reader.errorString();
        }
        in_file.close();
    }
    else
        qCDebug(phxLibrary) << file_path << " was not opened";
}

QRegularExpressionMatch PhoenixLibrary::parseFilename(QString filename)
{
    static QRegularExpression re_title_region(
        R"(^(?<title>.*)( \((?<region>.+?)\)))",
        QRegularExpression::DontCaptureOption | // only capture named groups
        QRegularExpression::DotMatchesEverythingOption
    );

    static QRegularExpression re_goodtools(
        R"(\[(?<gt_code>.*?)\])",
        QRegularExpression::DontCaptureOption | // only capture named groups
        QRegularExpression::DotMatchesEverythingOption
    );

    QRegularExpressionMatch m_title = re_title_region.match(filename);
    if (m_title.hasMatch())
        return m_title;

    // else, just match the whole filename as title
    return QRegularExpression("^(?<title>.*)$").match(filename);

    if (m_title.hasMatch()) {
        qCDebug(phxLibrary) << m_title;

        auto match_iterator = re_goodtools.globalMatch(filename, m_title.capturedEnd());
        while (match_iterator.hasNext()) {
            QRegularExpressionMatch match = match_iterator.next();
            qCDebug(phxLibrary) << match;
        }
    }
}

void PhoenixLibrary::startAsyncScan(QUrl path)
{
    QFuture<void> fut = QtConcurrent::run(this, &PhoenixLibrary::scanFolder, path);
    Q_UNUSED(fut)
}

void PhoenixLibrary::scanFolder(QUrl folder_path)
{
    QDirIterator dir_iter(folder_path.toLocalFile(), QDirIterator::Subdirectories);

    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q(database);

    setLabel("Importing Games");
    setProgress(0.0);

    bool found_games = false;
    while (dir_iter.hasNext()) {
        if (!found_games)
            found_games = true;
        dir_iter.next();
        QFileInfo info(dir_iter.fileInfo());

        if (!info.isFile())
            continue;

        if (!core_for_extension.contains(info.suffix()))
            continue; // not a known rom extension

        QRegularExpressionMatch m = parseFilename(info.completeBaseName());

        QString system = m_consoles.value(core_for_console.key(core_for_extension[info.suffix().toLower()]), "Unknown");
        QByteArray hash = generateSha1Sum(info.absoluteFilePath());

        QString title = m.captured("title");

        scanSystemDatabase(hash, title, system);



        q.prepare("INSERT INTO " % LibraryDbManager::table_games % " (title, system, time_played, region, filename)"
                  " VALUES (?, ?, ?, ?, ?)");
        q.addBindValue(title);
        q.addBindValue(system);
        q.addBindValue("00:00");
        q.addBindValue(m.captured("region"));
        q.addBindValue(info.absoluteFilePath());
        q.exec();

    }

    if (found_games) {
        database.commit();
        QMetaObject::invokeMethod(m_model, "select");
    }

    setLabel("");

}

void PhoenixLibrary::deleteRow(QString title)
{
    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q(database);

    q.prepare(QStringLiteral("DELETE FROM ") % LibraryDbManager::table_games % QStringLiteral(" WHERE title = ?"));
    q.addBindValue(title);
    if (q.exec()) {
        database.commit();
        m_model->select();

    }


    else {
        qCDebug(phxLibrary) << "Error deleting entry: " << q.lastQuery();
    }

    /*
    if (m_model->removeRow(index)) {
        if (m_model->submitAll())
            qCDebug(phxLibrary) << "submitAll: true";
        else {
            qCDebug(phxLibrary) << "Warning: " << m_model->lastError().text();
        }
    }
    else {
        m_model->revert();
    }*/
}

void PhoenixLibrary::resetAll()
{
    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q(database);

    setLabel("Clearing Library");

    q.prepare(QStringLiteral("DELETE FROM ") % LibraryDbManager::table_games);

    if (q.exec()) {
        database.commit();
        m_model->select();
        setLabel("Library Cleared");
    }
    else {
        qCDebug(phxLibrary) << "Error clearing library";
        setLabel("Error Clearing Library");
    }
}

/*GameData *PhoenixLibrary::asyncScrapeInfo(QString name, QString system)
{
    QFuture<GameData *> fut = QtConcurrent::run(this, &PhoenixLibrary::scrapeInfo, name, system);
    if (fut.isFinished())
        name = fut.result()->
}

GameData *PhoenixLibrary::scrapeInfo(QString name, QString system)
{
    scraper.setData(new GameData);
    scraper.setGameName(name);
    scraper.setGamePlatform(system);
    scraper.start();
}*/

bool PhoenixLibrary::setPreferredCore(QString system, QString new_core)
{
    qCDebug(phxLibrary) << "Changing: " << system << " with new value: " << new_core;

    if (platform_manager.preferred_cores.contains(system)) {
        platform_manager.preferred_cores[system] = new_core;
        return true;
    }
    return false;
}

QString PhoenixLibrary::getSystem(QString system)
{
    QString core_path = "";
#ifdef Q_OS_WIN32
        QString temp = platform_manager.preferred_cores.value(system, "");
        if (temp != "")
            core_path = QApplication::applicationDirPath() + "/cores/" + temp + ".dll";
#endif
#ifdef Q_OS_LINUX
        QString temp = platform_manager.preferred_cores.value(system, "");
        if (temp != "")
            core_path = "/usr/lib/libretro/" + temp + ".so";
#endif

    return core_path;
}

QList<QObject *> PhoenixLibrary::coresModel(QString system)
{
    return cores_for_console[system];
}

QStringList PhoenixLibrary::systemsModel()
{
    QStringList systems_list;
    for (auto &system :  m_consoles)
        systems_list.append(system);
    qCDebug(phxLibrary) << systems_list;
    return systems_list;
}

QString PhoenixLibrary::systemIcon(QString system)
{
    return icon_for_console.value(system, "");
}

QString PhoenixLibrary::showPath(int index, QString system)
{
    if (index < cores_for_console[system].length()) {
        CoreModel *mod = static_cast<CoreModel *>(cores_for_console[system].at(index));
        return mod->corePath();
    }
    return QString("");
}

void PhoenixLibrary::cacheUrls(QList<QUrl> list) {
    file_urls = list;
}

void PhoenixLibrary::importDroppedFiles()
{
    int length = file_urls.length();
    if (!length)
        return;

    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q(database);
    for (int i=0; i < length; ++i) {

        QFileInfo file = QFileInfo(file_urls[i].toLocalFile());

        setLabel("Importing Games");

        if (!core_for_extension.contains(file.suffix()))
            continue; // not a known rom extension

        QRegularExpressionMatch m = parseFilename(file.completeBaseName());

        QString system = m_consoles.value(core_for_console.key(core_for_extension[file.suffix().toLower()]), "Unknown");
        QByteArray hash = generateSha1Sum(file.absoluteFilePath());

        QString title = m.captured("title");

        scanSystemDatabase(hash, title, system);

        q.prepare(QStringLiteral("INSERT INTO ") % LibraryDbManager::table_games % QStringLiteral(" (title, system, time_played, region, filename)") %
                  QStringLiteral(" VALUES (?, ?, ?, ?, ?)"));
        q.addBindValue(title);
        q.addBindValue(system);
        q.addBindValue("00:00");
        q.addBindValue(m.captured("region"));
        q.addBindValue(file.absoluteFilePath());
        q.exec();

    }

    database.commit();
    QMetaObject::invokeMethod(m_model, "select");
}

void PhoenixLibrary::setImportUrls(bool importUrls)
{
    m_import_urls = importUrls;
    emit importUrlsChanged();

    if (m_import_urls) {
        QFuture<void> fut = QtConcurrent::run(this, &PhoenixLibrary::importDroppedFiles);
        Q_UNUSED(fut)
    }

    m_import_urls = false;
}

QByteArray PhoenixLibrary::generateSha1Sum(QString file)
{
    QFile game_file(file);
    if (!game_file.open(QIODevice::ReadOnly))
        return QByteArray("");

    QByteArray game_data = game_file.readAll();

    QCryptographicHash sha1_hash(QCryptographicHash::Sha1);
    sha1_hash.addData(game_data);
    QByteArray result = sha1_hash.result().toHex();

    game_file.close();
    return result;
}

void PhoenixLibrary::scanSystemDatabase(QByteArray hash, QString &name, QString &system)
{

    Q_UNUSED(system)

    QSqlDatabase db = system_db.handle();
    db.transaction();

    QSqlQuery q(db);

    q.prepare("SELECT sha1, gamename FROM NOINTRO WHERE sha1 LIKE ?");
    q.addBindValue("%" + hash + "%");



    if (!q.exec())
        qCDebug(phxLibrary) << q.executedQuery() << q.lastError();

    QString game_name;
    while(q.next()) {
        game_name = q.value(1).toString();
    }

    name = (game_name == "") ? name : game_name;

}
