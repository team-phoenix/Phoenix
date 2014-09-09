
#include <QRegularExpression>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QResource>
#include <QXmlStreamReader>
#include <QFile>
#include <QtConcurrent>

#include "phoenixlibrary.h"
#include "librarydbmanager.h"
#include "libretro_cores_info.h"
#include "logging.h"


PhoenixLibrary::PhoenixLibrary()
    : core_for_console {
        { Atari_Lynx,         libretro_cores_info["handy_libretro"] },
        { IBM_PC,             libretro_cores_info["dosbox_libretro"] },
        { Nintendo_NES,       libretro_cores_info["nestopia_libretro"] },
        { Nintendo_SNES,      libretro_cores_info["snes9x_libretro"] },
        { Nintendo_Game_Boy,  libretro_cores_info["gambatte_libretro"] },
        { Nintendo_GBA,       libretro_cores_info["vbam_libretro"] },
        { Nintendo_DS,        libretro_cores_info["desmume_libretro"] },
        { Sega_Master_System, libretro_cores_info["picodrive_libretro"] },
        { Sega_Mega_Drive,    libretro_cores_info["picodrive_libretro"] },
        { Sega_Game_Gear,     libretro_cores_info["picodrive_libretro"] },
        { Sega_CD,            libretro_cores_info["picodrive_libretro"] },
        { Sega_32X,           libretro_cores_info["picodrive_libretro"] },
        { Sony_PlayStation,   libretro_cores_info["pcsx_rearmed_libretro"] },
    }
{
    /*import_thread = new QThread();
    import_thread->setObjectName("phoenix-scraper");
    import_thread->setPriority(QThread::NormalPriority);*/

    m_model = new GameLibraryModel(&dbm, this);
    /*scraper = new TheGamesDB();
    scraper->moveToThread(import_thread);*/

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
                for (auto &c : core_for_extension.values(ext))
                    dbg << c["display_name"].toString();

                dbg << "and" << core["display_name"].toString();
            }

            core_for_extension.insertMulti(ext, core);
        }
    }

    /*connect(import_thread, SIGNAL(started()), this, SLOT(scanFolder()));
    connect(this, SIGNAL(destroyed()), import_thread, SLOT(deleteLater()));
    connect(import_thread, SIGNAL(finished()), import_thread, SLOT(deleteLater()));*/
}

PhoenixLibrary::~PhoenixLibrary()
{
    //if (scraper) {
     //   scraper->deleteLater();
    //}
    if (m_model)
        m_model->deleteLater();

    //if (import_thread)
       // import_thread->deleteLater();

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

        q.prepare("INSERT INTO " table_games " (title, console, time_played, region)"
                  " VALUES (?, ?, ?, ?)");
        q.addBindValue(m.captured("title"));
        q.addBindValue("whatever");
        q.addBindValue("00:00");
        q.addBindValue(m.captured("region"));
        q.exec();
    }

    if (found_games) {
        database.commit();
        QMetaObject::invokeMethod(m_model, "select");
    }

    setLabel("");

}

void PhoenixLibrary::scrapeInfo()
{
}
