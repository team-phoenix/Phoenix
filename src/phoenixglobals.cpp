#include "phoenixglobals.h"
#include "usernotifications.h"

#include <QLibrary>
#include <QDebug>

#include "utilities.h"

Utilities utilities;
InputManager input_manager;
PhoenixGlobals phxGlobals;
PhoenixLibraryHelper phoenixLibraryHelper;
UserNotifications userNotifications;

PhoenixGlobals::PhoenixGlobals(QObject *parent)
    : QObject(parent)
{

}

PhoenixGlobals::~PhoenixGlobals()
{

}

QString PhoenixGlobals::offlineStoragePath() const
{
    return m_offline_storage_path;
}

QString PhoenixGlobals::biosPath() const
{
    return m_bios_path;
}

QString PhoenixGlobals::artworkPath() const
{
    return m_artwork_path;
}

QString PhoenixGlobals::savePath() const
{
    return m_save_path;
}

QString PhoenixGlobals::configPath() const
{
    return m_config_path;
}

QFileInfo PhoenixGlobals::selectedGame() const
{
    return m_selected_game;
}

QFileInfo PhoenixGlobals::selectedCore() const
{
    return m_selected_core;
}

void PhoenixGlobals::setBiosPath(const QString &path)
{
    m_bios_path = path;
    utilities.createFolder(path);
}

void PhoenixGlobals::setArtworkPath(const QString &path)
{
    m_artwork_path = path;
    utilities.createFolder(path);
}

void PhoenixGlobals::setSavePath(const QString &path)
{
    m_save_path = path;
    utilities.createFolder(path);
}

void PhoenixGlobals::setConfigPath(const QString &path){
    m_config_path = path;
    utilities.createFolder(path);
}

void PhoenixGlobals::setSelectedGame(const QString &file)
{
    m_selected_game.setFile(file);
}

void PhoenixGlobals::setSelectedCore(const QString &file)
{
    m_selected_core.setFile(file);
}

void PhoenixGlobals::setOfflineStoragePath(const QString &path)
{
    m_offline_storage_path = path;

    setBiosPath(path + "Bios/");
    setArtworkPath(path + "Artwork/");
    setSavePath(path + "Saves/");
    setConfigPath(path + "Config/");
}

bool PhoenixGlobals::validCore( QString core_path )
{
    if (core_path == "")
        return false;

    QLibrary libretro_core( core_path );
    bool result = libretro_core.load();
    libretro_core.unload();
    if (result)
        setSelectedCore(QString(core_path));
    else
        userNotifications.setCoreNotification(core_path + " could not be loaded");
    qDebug() << "core loaded ?: " << result;

    return result;
}

bool PhoenixGlobals::validGame( QString game_path )
{
    qDebug() << "game loaded ?: " << game_path;

    if (game_path == "")
        return false;

    QFile in_file( game_path );
    bool result = in_file.open( QIODevice::ReadOnly );

    in_file.close();
    if (result)
        setSelectedGame(QString(game_path));
    else
        userNotifications.setGameNotification(game_path + " could not be loaded");

    qDebug() << "game loaded ?: " << result;
    return result;
}
