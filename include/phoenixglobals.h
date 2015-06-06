#ifndef PHOENIXGLOBALS_H
#define PHOENIXGLOBALS_H

#include <QObject>
#include <QMap>
#include <QFileInfo>

#include "usernotifications.h"
#include "utilities.h"
#include "inputmanager.h"
#include "phoenixlibraryhelper.h"

/* The PhoenixGlobals class is a global singleton that is used to
 * provide folder locations, and do other various operations that
 * aren't specific to any class and need to be accessed from a multitude of places.
 */

class PhoenixGlobals : public QObject {
        Q_OBJECT
    public:
        explicit PhoenixGlobals( QObject *parent = 0 );
        ~PhoenixGlobals();

        void setBiosPath( const QString &path );
        void setArtworkPath( const QString &path );
        void setOfflineStoragePath( const QString &path );
        void setSavePath( const QString &path );
        void setConfigPath( const QString &path );
        void setSelectedGame( const QString &file );
        void setSelectedCore( const QString &file );

    public slots:
        QString offlineStoragePath() const;
        QString biosPath() const;
        QString artworkPath() const;
        QString savePath() const;
        QString configPath() const;
        QFileInfo selectedGame() const;
        QFileInfo selectedCore() const;

        bool validCore( QString core_path );
        bool validGame( QString game_path );

    private:
        QString m_offline_storage_path;
        QString m_bios_path;
        QString m_artwork_path;
        QString m_save_path;
        QString m_config_path;
        QFileInfo m_selected_game;
        QFileInfo m_selected_core;

};

extern Utilities utilities;
extern InputManager input_manager;
extern PhoenixGlobals phxGlobals;
extern PhoenixLibraryHelper phoenixLibraryHelper;
extern UserNotifications userNotifications;

#endif // PHOENIXGLOBALS_H
