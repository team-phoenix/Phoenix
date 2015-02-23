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
#include <QApplication>

#include <memory>

#include "phoenixlibrary.h"
#include "librarydbmanager.h"
#include "libretro_cores_info.h"
#include "logging.h"

PhoenixLibrary::PhoenixLibrary()
    : core_for_console( QMap<PhoenixLibrary::Console, QVariantMap> {
    { Nintendo_NES,       libretro_cores_info[platform_manager.preferred_cores[platform_manager.nintendo]] },
    { Nintendo_SNES,      libretro_cores_info[platform_manager.preferred_cores[platform_manager.super_nintendo]] },
    { Nintendo_Game_Boy,  libretro_cores_info[platform_manager.preferred_cores[platform_manager.gameboy]] },
    { Nintendo_GBA,       libretro_cores_info[platform_manager.preferred_cores[platform_manager.gameboy_advance]] },
    { Sony_PlayStation,   libretro_cores_info[platform_manager.preferred_cores[platform_manager.playstation]] },
} ),
icon_for_console( QMap<QString, QString> {
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
} ),
m_consoles( QMap<PhoenixLibrary::Console, QString> {
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
} ) {
    m_import_urls = false;
    m_progress = 0;


    excluded_consoles = QStringList() << platform_manager.nintendo_ds << platform_manager.mupen64plus << platform_manager.ppsspp
    << platform_manager.desmume;


    m_model = new GameLibraryModel( &dbm, this );
    m_model->setEditStrategy( QSqlTableModel::OnManualSubmit );
    m_model->select();

    for( auto &core : libretro_cores_info ) {
        QString exts = core["supported_extensions"].toString();

        if( exts.isEmpty() ) {
            continue;
        }

        if( core_for_console.key( core ) == InvalidConsole ) {
            continue;    // core not in core_for_console map
        }

        for( QString &ext : exts.split( "|" ) ) {
            if( ext.isEmpty() ) {
                continue;
            }


            if( core_for_extension.contains( ext ) ) {
                QDebug dbg = qWarning( phxLibrary );
                dbg << "Multiple cores for extension" << ext << ":";

                for( auto &c : core_for_extension.values( ext ) ) {
                    dbg << c["display_name"].toString();
                    qCDebug( phxLibrary ) << "EXTENSION: " << ext;

                }

                dbg << "and" << core["display_name"].toString();
            }

            core_for_extension.insertMulti( ext, core );
        }
    }

    QString base_path = QApplication::applicationDirPath();

    for( auto &core : libretro_cores_info.keys() ) {

        QString system = libretro_cores_info[core]["systemname"].toString();
        QString cleaned_name = platform_manager.cleaned_system_name.value( system, system );
        QString display_name = libretro_cores_info[core].value( "corename", "" ).toString();
#ifdef Q_OS_WIN32
        QString full_path = base_path + "/cores/" + core + ".dll";
#endif
#ifdef Q_OS_LINUX
        QString full_path = "/usr/lib/libretro/" + core + ".so";
#endif
#ifdef Q_OS_MACX
        QString full_path = "/usr/local/lib/libretro/" + core + ".dylib";
#endif
        QFile in_file( full_path );

        if( in_file.exists() ) {
            cores_for_console[cleaned_name].append( new CoreModel( this, display_name, core ) );
        }

    }

    network_queue = new NetworkQueue();
    network_queue->setGameModel( m_model );

    connect( network_queue, &NetworkQueue::label, this, &PhoenixLibrary::setLabel, Qt::DirectConnection);
    connect( network_queue, &NetworkQueue::progress, this, &PhoenixLibrary::setProgress, Qt::DirectConnection);

}

PhoenixLibrary::~PhoenixLibrary() {
    if( m_model ) {
        m_model->deleteLater();
    }

    delete network_queue;
}


void PhoenixLibrary::setLabel( QString label ) {
    m_label = label;
    emit labelChanged();
}

void PhoenixLibrary::setProgress( qreal progress ) {
    m_progress = progress;
    emit progressChanged();
}

void PhoenixLibrary::loadXml( QString file_path ) {
    QResource resource( file_path );
    QFile in_file( resource.absoluteFilePath() );

    if( in_file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QXmlStreamReader reader;
        reader.setDevice( &in_file );

        while( !reader.isEndDocument() ) {
            reader.readNext();
            QString element = reader.name().toString();
            qCDebug( phxLibrary ) << element;

            if( element == "name" )

            {
                qCDebug( phxLibrary ) << reader.readElementText();
            }
        }

        if( reader.hasError() ) {
            qCDebug( phxLibrary ) << reader.errorString();
        }

        in_file.close();
    } else {
        qCDebug( phxLibrary ) << file_path << " was not opened";
    }
}

QRegularExpressionMatch PhoenixLibrary::parseFilename( QString filename ) {
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

    QRegularExpressionMatch m_title = re_title_region.match( filename );

    if( m_title.hasMatch() ) {
        return m_title;
    }

    // else, just match the whole filename as title
    return QRegularExpression( "^(?<title>.*)$" ).match( filename );

    // Currently Unfinished code. Only extracts title and region from filename
    // not the goodtools code.

    /*if( m_title.hasMatch() ) {
        qCDebug( phxLibrary ) << m_title;

        auto match_iterator = re_goodtools.globalMatch( filename, m_title.capturedEnd() );

        while( match_iterator.hasNext() ) {
            QRegularExpressionMatch match = match_iterator.next();
            qCDebug( phxLibrary ) << match;
        }
    }*/
}

void PhoenixLibrary::startAsyncScan( QUrl path ) {

    QFuture<QVector<int>> fut = QtConcurrent::run( this, &PhoenixLibrary::scanFolder, path );
    auto watcher = std::make_shared<QFutureWatcher<QVector<int>>>();
    auto checksum_watcher = std::make_shared<QFutureWatcher<void>>();


    connect( checksum_watcher.get(), &QFutureWatcher<void>::finished, this, [this] {
        network_queue->start();
    } );

    // Request and update the artworks when the folder scan is finished
    connect( watcher.get(), &QFutureWatcher<bool>::finished, this, [this, watcher, checksum_watcher]() {
        auto inserted_games = watcher->result();

        QFuture<void> f = QtConcurrent::run( this, &PhoenixLibrary::importMetadata,
                                             inserted_games );
        checksum_watcher->setFuture( f );

        //QMetaObject::invokeMethod(&network_queue, "start");
        //network_queue.start();
        //network_queue.waitForFinished();


    } );
    watcher->setFuture( fut );
}

void PhoenixLibrary::importMetadata( QVector<int> games_id ) {
    QString what_games;
    what_games.reserve( games_id.size() * 4 );

    setLabel( "Retrieving Metadata & Art" );
    setProgress( 0.0 );

    // build a comma separated string from roms id that were just inserted
    for( auto i = games_id.begin(); i != games_id.end(); ++i ) {
        what_games.append( QString::number( *i ) );

        if( std::next( i ) != games_id.end() ) {
            what_games.append( "," );
        }
    }

    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q( database );

    q.exec( QStringLiteral( "SELECT id, directory, filename, title, system FROM %1 WHERE id IN (%2)" )
            .arg( LibraryDbManager::table_games ).arg( what_games ) );

    if( !q.exec() ) {
        qCWarning( phxLibrary ) << "Error while trying to find metadata for imported games";
    }

    for( uint i = 0; q.next(); i++ ) {
        setProgress( ( qreal )i / games_id.size() * 100.0 );
        int id = q.value( 0 ).toInt();
        QString path( QDir( q.value( 1 ).toString() ).filePath( q.value( 2 ).toString() ) );
        QByteArray hash = generateSha1Sum( path ); // TODO: CRC32
        QString title = q.value( 3 ).toString();
        QString system = q.value( 4 ).toString();
        scanSystemDatabase( hash, title, system );
        QSqlQuery qupdate( database );
        qupdate.prepare( QStringLiteral( "UPDATE %1 SET title = ?, system = ? WHERE id = ?" )
                         .arg( LibraryDbManager::table_games ) );
        qupdate.addBindValue( title );
        qupdate.addBindValue( system );
        qupdate.addBindValue( id );
        qupdate.exec();

        // call enqueueContext in its thread
        Scraper::ScraperContext context;
        qCDebug( phxLibrary ) << title << system;
        context.id = id;
        context.title = title;
        context.system = system;
        network_queue->enqueueContext( context );

    }

    database.commit();
    QMetaObject::invokeMethod( m_model, "submitAll" );

    setLabel( "" );

}

bool PhoenixLibrary::insertGame( QSqlQuery &q, QFileInfo path ) {
    if( !core_for_extension.contains( path.suffix() ) ) {
        return false;    // not a known rom extension
    }

    QRegularExpressionMatch m = parseFilename( path.completeBaseName() );

    auto core = core_for_extension[path.suffix().toLower()];
    QString system = m_consoles.value( core_for_console.key( core ), "Unknown" );

    QString title = m.captured( "title" );


    q.prepare( "INSERT INTO " % LibraryDbManager::table_games %
               " (title, system, time_played, region, directory, filename)"
               " VALUES (?, ?, ?, ?, ?, ?)" );
    q.addBindValue( title );
    q.addBindValue( system );
    q.addBindValue( "00:00" );
    q.addBindValue( m.captured( "region" ) );
    q.addBindValue( path.dir().path() );
    q.addBindValue( path.absoluteFilePath() );

    bool result = q.exec();

    if( result ) {
        emit rowAdded();
    }

    return result;
}

QVector<int> PhoenixLibrary::scanFolder( QUrl folder_path ) {
    QDirIterator dir_iter( folder_path.toLocalFile(), QDirIterator::Subdirectories );

    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q( database );

    setLabel( "Importing Games" );
    setProgress( 0.0 );
    QVector<int> inserted_games;

    bool found_games = false;

    while( dir_iter.hasNext() ) {
        if( !found_games ) {
            found_games = true;
        }

        dir_iter.next();
        QFileInfo info( dir_iter.fileInfo() );

        if( !info.isFile() ) {
            continue;
        }

        if( insertGame( q, info ) && q.lastInsertId().isValid() ) {
            inserted_games.append( q.lastInsertId().toInt() );
        } else {
            qCWarning( phxLibrary ) << "Unable to import game" << info.fileName()
                                    << "; error:" << q.lastError();
        }
    }

    if( found_games ) {
        database.commit();
        QMetaObject::invokeMethod( m_model, "submitAll" );

        //QMetaObject::invokeMethod(m_model, "select");
    }

    setLabel( "" );
    return inserted_games;
}

void PhoenixLibrary::deleteRow( QString title ) {

    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q( database );

    q.prepare( QStringLiteral( "DELETE FROM " ) % LibraryDbManager::table_games % QStringLiteral( " WHERE title = ?" ) );
    q.addBindValue( title );

    if( q.exec() ) {
        database.commit();
        QMetaObject::invokeMethod( m_model, "submitAll" );
    }


    else {
        qCDebug( phxLibrary ) << "Error deleting entry: " << q.lastQuery();
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

void PhoenixLibrary::resetAll() {
    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q( database );

    setLabel( "Clearing Library" );

    q.prepare( QStringLiteral( "DELETE FROM " ) % LibraryDbManager::table_games );

    if( q.exec() ) {
        database.commit();
        QMetaObject::invokeMethod( m_model, "submitAll" );
        setLabel( "Library Cleared" );
    } else {
        qCDebug( phxLibrary ) << "Error clearing library";
        setLabel( "Error Clearing Library" );
    }

    setLabel( "" );

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

bool PhoenixLibrary::setPreferredCore( QString system, QString new_core ) {
    qCDebug( phxLibrary ) << "Changing: " << system << " with new value: " << new_core;

    if( platform_manager.preferred_cores.contains( system ) ) {
        platform_manager.preferred_cores[system] = new_core;
        return true;
    }

    return false;
}

QString PhoenixLibrary::getSystem( QString system ) {
    QString core_path = "";
#ifdef Q_OS_WIN32
    QString temp = platform_manager.preferred_cores.value( system, "" );

    if( temp != "" ) {
        core_path = QApplication::applicationDirPath() + "/cores/" + temp + ".dll";
    }

#endif
#ifdef Q_OS_LINUX
    QString temp = platform_manager.preferred_cores.value( system, "" );

    if( temp != "" ) {
        core_path = "/usr/lib/libretro/" + temp + ".so";
    }

#endif
#ifdef Q_OS_MACX
    QString temp = platform_manager.preferred_cores.value( system, "" );

    if( temp != "" ) {
        core_path = "/usr/local/lib/libretro/" + temp + ".dylib";
    }

#endif

    return core_path;
}

QList<QObject *> PhoenixLibrary::coresModel( QString system ) {
    return cores_for_console[system];
}

QStringList PhoenixLibrary::systemsModel() {
    QStringList systems_list;

    for( auto &system :  m_consoles ) {
        systems_list.append( system );
    }

    qCDebug( phxLibrary ) << systems_list;
    return systems_list;
}

QString PhoenixLibrary::systemIcon( QString system ) {
    return icon_for_console.value( system, "" );
}

QString PhoenixLibrary::showPath( int index, QString system ) {
    if( index < cores_for_console[system].length() ) {
        CoreModel *mod = static_cast<CoreModel *>( cores_for_console[system].at( index ) );
        return mod->corePath();
    }

    return QString( "" );
}

void PhoenixLibrary::cacheUrls( QList<QUrl> list ) {
    file_urls = list;
}

QVector<int> PhoenixLibrary::importDroppedFiles( QList<QUrl> url_list ) {
    QSqlDatabase database = dbm.handle();
    database.transaction();

    QSqlQuery q( database );

    setLabel( "Importing Games" );
    setProgress( 0.0 );
    QVector<int> inserted_games;


    for( int i = 0; i < url_list.length(); ++i ) {

        QFileInfo info = QFileInfo( url_list[i].toLocalFile() );

        if( insertGame( q, info )      && q.lastInsertId().isValid() ) {
            inserted_games.append( q.lastInsertId().toInt() );
        } else {
            qCWarning( phxLibrary ) << "Unable to import game" << info.fileName()
                                    << "; error:" << q.lastError();
        }
    }

    database.commit();
    QMetaObject::invokeMethod( m_model, "submitAll" );

    setLabel( "" );
    return inserted_games;
}

void PhoenixLibrary::setImportUrls( bool importUrls ) {
    m_import_urls = importUrls;
    emit importUrlsChanged();

    if( m_import_urls ) {
        if( file_urls.length() > 0 ) {
            QFuture<QVector<int>> fut = QtConcurrent::run( this, &PhoenixLibrary::importDroppedFiles, file_urls );
            auto watcher = std::make_shared<QFutureWatcher<QVector<int>>>();
            auto checksum_watcher = std::make_shared<QFutureWatcher<void>>();
            connect( checksum_watcher.get(), &QFutureWatcher<void>::finished, this, [this] {
                qCDebug( phxLibrary ) << "Running artwork fetch";
                network_queue->start();

            } );

            // Request and update the artworks when the folder scan is finished
            connect( watcher.get(), &QFutureWatcher<bool>::finished, this, [this, watcher, checksum_watcher]() {
                auto inserted_games = watcher->result();

                QFuture<void> f = QtConcurrent::run( this, &PhoenixLibrary::importMetadata,
                                                     inserted_games );
                checksum_watcher->setFuture( f );

            } );
            watcher->setFuture( fut );
        }
    }

    m_import_urls = false;
}

QByteArray PhoenixLibrary::generateSha1Sum( QString file ) {
    QFile game_file( file );

    if( !game_file.open( QIODevice::ReadOnly ) ) {
        return QByteArray( "" );
    }

    QCryptographicHash sha1_hash( QCryptographicHash::Sha1 );
    sha1_hash.addData( &game_file );
    QByteArray result = sha1_hash.result();

    game_file.close();
    return result;
}

void PhoenixLibrary::scanSystemDatabase( QByteArray hash, QString &name, QString &system ) {

    Q_UNUSED( system )

    QSqlDatabase db = system_db.handle();
    db.transaction();

    QSqlQuery q( db );

    // LIKE == case insensitive
    q.prepare( "SELECT sha1, gamename FROM NOINTRO WHERE sha1 LIKE ?" );
    q.addBindValue( hash.toHex() );


    if( !q.exec() ) {
        qCDebug( phxLibrary ) << q.executedQuery() << q.lastError();
    }

    QString game_name;

    while( q.next() ) {
        game_name = q.value( 1 ).toString();
    }

    name = ( game_name == "" ) ? name : game_name;

}
