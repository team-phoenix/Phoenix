
#include <QRegularExpression>

#include "phoenixlibrary.h"
#include "librarydbmanager.h"
#include "logging.h"


PhoenixLibrary::PhoenixLibrary()
{
    /*import_thread = new QThread();
    import_thread->setObjectName("phoenix-scraper");
    import_thread->setPriority(QThread::NormalPriority);*/

    m_model = new GameLibraryModel(&dbm);
    /*scraper = new TheGamesDB();
    scraper->moveToThread(import_thread);*/

    /*connect(import_thread, SIGNAL(started()), this, SLOT(scanFolder()));
    connect(this, SIGNAL(destroyed()), import_thread, SLOT(deleteLater()));
    connect(import_thread, SIGNAL(finished()), import_thread, SLOT(deleteLater()));*/
}

PhoenixLibrary::~PhoenixLibrary()
{
    if (scraper) {
        scraper->deleteLater();
    }
    if (m_model)
        m_model->deleteLater();

    if (import_thread)
        import_thread->deleteLater();

}

void PhoenixLibrary::addFilters(QStringList &filter_list)
{
    filter_list << "n64"
                << "z64"
                << "nes"
                << "gba"
                << "gb"
                << "gbc"
                << "cue"
                << "sfc"
                << "smc";
}

QString PhoenixLibrary::getSystem(QString suffix)
{
    QString system;
    if (suffix == "nes")
        system = "Nintendo (NES)";
    else if (suffix == "sfc" || suffix == "smc")
        system = "Super Nintendo (SNES)";
    else if (suffix == "n64" || suffix == "z64")
        system = "Nintendo 64";
    else if (suffix == "gb" || suffix == "gbc")
        system = "Game Boy";
    else if (suffix == "gba")
        system = "Game Boy Advance";
    else {
        system = "Unknown";
        qCDebug(phxLibrary) << suffix << " was not handled";
    }
    return system;
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

void PhoenixLibrary::scanFolder(QUrl folder_path)
{
    QDirIterator dir_iter(folder_path.toLocalFile(), QDirIterator::Subdirectories);

    QVector<QFileInfo> files;

    QStringList filter;
    addFilters(filter);

    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q(database);

    setLabel("Importing Games");
    setProgress(0.0);

    while (dir_iter.hasNext()) {
        dir_iter.next();
        QFileInfo info(dir_iter.fileInfo());

        if (!info.isFile())
            continue;

        if (!filter.contains(info.suffix(), Qt::CaseInsensitive))
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

    database.commit();
    setLabel("");
}

void PhoenixLibrary::scrapeInfo()
{
}
