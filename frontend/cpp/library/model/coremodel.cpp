#include "coremodel.h"

using namespace Library;

CoreModel::CoreModel( QObject *parent )
    : QAbstractTableModel( parent ) {

    mRoleNames.insert( CoreModelRoles::CoresRole, QByteArrayLiteral( "cores" ) );
    mRoleNames.insert( CoreModelRoles::SystemRole, QByteArrayLiteral( "system" ) );
    mRoleNames.insert( CoreModelRoles::SystemFriendlyNameRole, QByteArrayLiteral( "systemFriendlyName" ) );
    mRoleNames.insert( CoreModelRoles::NumSystemsRole, QByteArrayLiteral( "numSystems" ) );
    mRoleNames.insert( CoreModelRoles::CurrentCoreIndexRole, QByteArrayLiteral( "currentCoreIndex" ) );
    mRoleNames.insert( CoreModelRoles::DefaultCoreIndexRole, QByteArrayLiteral( "defaultCoreIndex" ) );
    mRoleNames.insert( CoreModelRoles::DefaultCoreRole, QByteArrayLiteral( "defaultCore" ) );

    beginResetModel();

    LibretroDatabase libretroDatabase;

    QSqlQuery systemDBQuery = QSqlQuery( libretroDatabase );
    auto execStatus = systemDBQuery.exec( QStringLiteral( "SELECT DISTINCT UUID, defaultCore, friendlyName, manufacturer FROM system "
                                          "WHERE enabled = 1" ) );
    Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( systemDBQuery.lastError().text() ) );

    // Grab the system list from the system DB along with the default core
    while( systemDBQuery.next() ) {
        auto system = systemDBQuery.value( 0 ).toString();
        auto defaultCore = systemDBQuery.value( 1 ).toString();
        auto friendlyName = systemDBQuery.value( 2 ).toString();
        auto manufacturer = systemDBQuery.value( 3 ).toString();

        if( !defaultCore.isNull() ) {
            defaultCoreList.insert( system, defaultCore );
        }

        systemList.append( system );
        systemFriendlyNameList.append( friendlyName.isEmpty() || friendlyName.isNull() ? QString( "" ) : QString( manufacturer % QStringLiteral( " - " ) % friendlyName ) );
    }

    systemList.sort( Qt::CaseInsensitive );

    // Grab the user's current core choices from the user DB
    auto userDB = UserDatabase::instance()->database();
    auto userDBQuery = QSqlQuery( userDB );
    execStatus = userDBQuery.exec( QStringLiteral( "SELECT DISTINCT system, defaultCore FROM " ) +
                                   UserDatabase::tableCurrentCores + QStringLiteral( ";" ) );
    Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( userDBQuery.lastError().text() ) );

    while( userDBQuery.next() ) {
        auto system = userDBQuery.value( 0 ).toString();
        auto currentCore = userDBQuery.value( 1 ).toString();

        if( !currentCore.isNull() ) {
            systemToCoresMap.insert( system, QStringList{ currentCore } );
        }
    }

    userDBQuery.finish();

    auto transaction = userDB.transaction();
    Q_ASSERT_X( transaction, Q_FUNC_INFO, qPrintable( userDB.lastError().text() ) );

    // Copy the systems and their default core over to the user database (and model list) that aren't already there
    for( auto system : defaultCoreList.keys() ) {
        if( !systemToCoresMap.contains( system ) ) {
            qCDebug( phxLibrary ) << "Adding missing system" << system << "and default core" << defaultCoreList[ system ] << "to user database...";
            execStatus = userDBQuery.exec( QStringLiteral( "INSERT INTO " ) + UserDatabase::tableCurrentCores +
                                           QString( " (system, defaultCore) VALUES (\'%1\', \'%2');" ).arg( system, defaultCoreList[ system ] ) );
            Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( userDBQuery.lastError().text() ) );
            systemToCoresMap.insert( system, QStringList{ defaultCoreList[ system ] } );

        }
    }

    auto commit = userDB.commit();
    Q_ASSERT_X( commit, Q_FUNC_INFO, qPrintable( userDB.lastError().text() ) );

    userDBQuery.finish();

    // Grab all the other available cores from the system DB
    execStatus = systemDBQuery.exec( QStringLiteral( "SELECT DISTINCT core, system FROM systemToCore "
                                     "INNER JOIN system ON system.UUID = systemToCore.system "
                                     "WHERE system.enabled = 1" ) );
    Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( systemDBQuery.lastError().text() ) );

    while( systemDBQuery.next() ) {
        auto core = systemDBQuery.value( 0 ).toString();
        auto system = systemDBQuery.value( 1 ).toString();

        auto &list = systemToCoresMap[ system ];

        if( !list.contains( core ) ) {
            list.append( core );
        }
    }

    systemDBQuery.finish();

    // Sort cores, store index of current core and default core after the list is sorted
    for( auto system : systemList ) {
        auto currentCore = systemToCoresMap[ system ][ 0 ];

        QStringList &list = systemToCoresMap[ system ];
        list.sort( Qt::CaseInsensitive );
        currentCoreIndex[ system ] = list.indexOf( currentCore );
        defaultCoreIndex[ system ] = list.indexOf( defaultCoreList[ system ] );
    }

    // Mark default cores with a tag
    for( auto system : systemList ) {
        // Copy the map in its entirety
        for( auto core : systemToCoresMap[ system ] ) {
            systemToCoreFriendlyNamesMap[ system ] << core;
        }

        // Give the default cores an indication that they're default
        QString renamedCore = defaultCoreList[ system ] % " (default)";
        systemToCoreFriendlyNamesMap[ system ][ defaultCoreIndex[ system ] ] = renamedCore;
    }

    // Print sorted list (debug)
    // for( auto system : systemList ) {
    //     qCDebug( phxLibrary ) << system << coreListMap[ system ];
    // }

    endResetModel();

}

QModelIndex CoreModel::index( int row, int column, const QModelIndex &parent ) const {
    return hasIndex( row, column, parent ) ? createIndex( row, column, nullptr ) : QModelIndex();
}

int CoreModel::rowCount( const QModelIndex &parent ) const {
    if( parent.isValid() ) {
        return 0;
    }

    return systemList.size();
}

int CoreModel::columnCount( const QModelIndex &parent ) const {
    if( parent.isValid() ) {
        return 0;
    }

    return 2;
}

QVariant CoreModel::data( const QModelIndex &index, int role ) const {
    if( !index.isValid() || index.row() >= systemList.size() || index.row() < 0 ) {
        return QVariant();
    }

    switch( role ) {
        case CoresRole: {
            auto &system = systemList.at( index.row() );
            return systemToCoreFriendlyNamesMap[ system ];
        }

        case SystemRole:
            return systemList[ index.row() ];

        case SystemFriendlyNameRole:
            return systemFriendlyNameList[ index.row() ];

        case NumSystemsRole:
            return systemList.size();

        case CurrentCoreIndexRole: {
            auto &system = systemList.at( index.row() );
            return currentCoreIndex[ system ];
        }

        case DefaultCoreIndexRole: {
            auto &system = systemList.at( index.row() );
            return defaultCoreIndex[ system ];
        }

        case DefaultCoreRole: {
            auto &system = systemList.at( index.row() );
            return defaultCoreList[ system ];
        }

        default:
            Q_UNREACHABLE();
            break;
    }

}

QHash<int, QByteArray> CoreModel::roleNames() const {
    return mRoleNames;
}

void CoreModel::save( const QString system, const QString newCurrentCore ) {
    auto db = UserDatabase::instance()->database();
    auto transaction = db.transaction();
    Q_ASSERT_X( transaction, Q_FUNC_INFO, qPrintable( db.lastError().text() ) );

    auto query = QSqlQuery( db );
    auto statement = QString( "UPDATE defaultCores SET defaultCore = \'%1\' WHERE system = \'%2\'" ).arg( newCurrentCore, system );

    auto exec = query.exec( statement );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

    auto commit = db.commit();
    Q_ASSERT_X( commit, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

}

void CoreModel::save( const QString system, int newCurrentCoreIndex ) {
    save( system, systemToCoresMap[ system ][ newCurrentCoreIndex ] );
}

bool CoreModel::coreExists( QString core ) {
    QString defaultCore;
#if defined( Q_OS_WIN )
    defaultCore = PhxPaths::coreLocation() % QStringLiteral( "/" ) % core % QStringLiteral( ".dll" );
#endif
#if defined( Q_OS_MAC )
    defaultCore = PhxPaths::coreLocation() % QStringLiteral( "/" ) % core % QStringLiteral( ".dylib" );
#endif
#if defined( Q_OS_LINUX )
    defaultCore = PhxPaths::coreLocation() % QStringLiteral( "/" ) % core % QStringLiteral( ".so" );
#endif
    // qCDebug( phxLibrary ) << defaultCore;
    return QFile::exists( defaultCore );
}