#ifndef PHOENIXLIBRARY_H
#define PHOENIXLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QThread>

#include <memory>

#include "thegamesdb.h"
#include "gamelibrarymodel.h"
#include "librarydbmanager.h"
#include "platformmanager.h"
#include "coremodel.h"
#include "systemdatabase.h"
#include "networkqueue.h"
#include "phoenixglobals.h"
#include "phoenixlibraryhelper.h"

/* The PhoenixLibrary class is a wrapper around the GameLibraryModel class.
 * This class is exposed to QML and is used to help import, remove, or
 * alter games in the GameLibraryModel.
 */

class PhoenixLibrary : public QObject {
        Q_OBJECT
        Q_PROPERTY( qreal progress READ progress WRITE setProgress NOTIFY progressChanged )
        Q_PROPERTY( QString label READ label WRITE setLabel NOTIFY labelChanged )
        Q_PROPERTY( bool importUrls READ importUrls WRITE setImportUrls NOTIFY importUrlsChanged )
        Q_ENUMS( Console )

    public:
        PhoenixLibrary();

        ~PhoenixLibrary();

        void setFolderPath( QUrl path );
        void setImportUrls( bool importUrls );

        QString label() const {
            return m_label;
        }

        qreal progress() const {
            return m_progress;
        }

        bool importUrls() {
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
            Last
        };

        // Can't use template parameters with Q_PROPERTY
        typedef QMap<Console, QString> ConsoleMap;

        Q_PROPERTY( ConsoleMap consoles READ consoles CONSTANT )
        ConsoleMap consoles() const {
            return {};
        }

    public slots:
        void setProgress( int progress );
        void setLabel( QString label );

        void startAsyncScan( QUrl path );
        void resetAll();
        GameLibraryModel *model() {
            return m_model.get();
        }
        void deleteRow( QString title );
        QString getSystem( QString system );
        QList<QObject *> coresModel( QString system );
        QStringList systemsModel();
        bool setPreferredCore( QString system, QString new_core );
        QString systemIcon( QString system );
        QString showPath( int index, QString system );
        void cacheUrls( QList<QUrl> list );
        QVector<int> importDroppedFiles( QList<QUrl> url_list );
        void setCacheDirectory(QString cache_dir);

        // Collection Operations
        bool addToCollection(QVariantList game_data);
        bool removeFromCollection(QVariant data);

    signals:
        void labelChanged();
        void progressChanged();
        void countChanged();
        void importUrlsChanged();
        void rowDeleted();
        void rowAdded();

    private:
        LibraryDbManager dbm;
        //TheGamesDB scraper;
        std::unique_ptr<GameLibraryModel> m_model;
        QString m_label;
        PlatformManager platform_manager;
        int m_progress;
        QList<QUrl> file_urls;
        bool m_import_urls;

        QString m_cache_directory;

        SystemDatabase system_db;

        QMap<QString, QList<QObject *>> cores_for_console;

        QMap<Console, QVariantMap> core_for_console;

        const QMap<QString, QString> icon_for_console;

        const QMap<Console, QString> m_consoles;

        QStringList excluded_consoles;
        TheGamesDB *thegamesdb;

        QMap<QString, QVariantMap> core_for_extension;

        QRegularExpressionMatch parseFilename( QString filename );
        bool insertGame( QSqlQuery &q, QFileInfo path );
        QVector<int> scanFolder( QUrl folder_path );
        void importMetadata( QVector<int> games_id );
        void scanSystemDatabase( QByteArray hash, QString &name, QString &system );
        void saveCollection(QVariantList collections);
};


#endif // PHOENIXLIBRARY_H
