#include "phoenixlibrary.h"

PhoenixLibrary::PhoenixLibrary()

{
    m_start = false;
    import_thread = new QThread();
    import_thread->setObjectName("phoenix-scraper");
    import_thread->setPriority(QThread::NormalPriority);

    m_model = new GameLibraryModel();
    scraper = new TheGamesDB();
    scraper->moveToThread(import_thread);

    connect(import_thread, SIGNAL(started()), this, SLOT(scanFolder()));
    connect(this, SIGNAL(destroyed()), import_thread, SLOT(deleteLater()));
    connect(import_thread, SIGNAL(finished()), import_thread, SLOT(deleteLater()));
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

bool PhoenixLibrary::startImport(bool start)
{
    if (start)
        import_thread->start();
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

void PhoenixLibrary::setFolderPath(QUrl path)
{
    m_folder_path = path;
    emit folderPathChanged(path);
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

void PhoenixLibrary::scanFolder()
{
    QDirIterator dir_iter(m_folder_path.toLocalFile(), QDirIterator::Subdirectories);

    QVector<QFileInfo> files;

    QStringList filter;
    addFilters(filter);

    // FileInfo is added to a vector so the user can see how far along on the import progress
    // the library is.

    setLabel("Import Games");
    while (dir_iter.hasNext()) {
        dir_iter.next();
        QFileInfo info(dir_iter.fileInfo());
        if (info.isFile()) {
            for (int i=0; i < filter.size(); ++i) {
                if (info.suffix() == filter.at(i)) {
                    files.append(info);
                    break;
                }
            }
        }
    }

    QSqlDatabase database = m_model->manager().handle();

    database.transaction();


    bool data_changed = false;

    int m_file_count = files.size();
    qreal count = static_cast<qreal>(m_file_count);

    setLabel("Finding Artwork");
    query = QSqlQuery(m_model->manager().handle());

    for (qreal i=0; i < count; ++i) {

        QFileInfo file_info = files.at(i);


        QString system = getSystem(file_info.suffix());



        if (system != "") {

            GameData data = scraper->getAllData(file_info.baseName(), system);

            QCryptographicHash sha1_gen(QCryptographicHash::Md5);

            query.prepare("INSERT INTO games (title, console, time_played, artwork)"
            " VALUES (?, ?, ?, ?)");

             QString system = getSystem(file_info.suffix());
            //QString xml_file =  QString((":/databases/%1.xml")).arg(system);

            //QResource resource(xml_file);

             if (data.title != "")
                 query.bindValue(0, data.title);
             else
                 query.bindValue(0, file_info.baseName());
             if (data.front_boxart != "")
                 query.bindValue(3, data.front_boxart);
             else
                 query.bindValue(3, "qrc:/assets/No-Art.png");

             query.bindValue(1, system);
             query.bindValue(2, "0h 0m 0s");


            //QFile in_file(file_info.absoluteFilePath());
            /*if (in_file.open(QIODevice::ReadOnly)) {
                QByteArray file_data = in_file.readAll();
                sha1_gen.addData(file_data.data(), file_data.length());
                QString md5;
                md5 = sha1_gen.result().toHex();

                if (md5 != "") {
                    QString console = QString(":/databases/%1.xml").arg(system);
                    //loadXml(console);
                }
                qCDebug(phxLibrary) << md5;

                in_file.close();
            }
            else
                qCDebug(phxLibrary) <<  "db not opened";
            */
            m_model->update();


            setProgress((((i+1) / m_file_count) * 100.0));

        }

        query.exec();

    }

    database.commit();
    m_model->submit();

}


//have query be all loaded up
