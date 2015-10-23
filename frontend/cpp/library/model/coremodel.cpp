#include "coremodel.h"

using namespace Library;

CoreModel::CoreModel( QObject *parent )
    : QAbstractTableModel( parent ) {

    mRoleNames.insert( DefaultCoreRoles::CoresRole, QByteArrayLiteral( "cores" ) );
    mRoleNames.insert( DefaultCoreRoles::SystemRole, QByteArrayLiteral( "system" ) );
    mRoleNames.insert( DefaultCoreRoles::DefaultCoreIndexRole, QByteArrayLiteral( "defaultCoreIndex" ) );

    beginResetModel();

    auto systemDBQuery = QSqlQuery( LibretroDatabase::database() );
    auto execStatus = systemDBQuery.exec( QStringLiteral( "SELECT DISTINCT system, defaultCore FROM systems;" ) );
    Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( systemDBQuery.lastError().text() ) );

    // Grab the system list from the system DB along with the default core
    QMap<QString, QString> defaultCores;

    while( systemDBQuery.next() ) {
        auto system = systemDBQuery.value( 0 ).toString();
        auto defaultCore = systemDBQuery.value( 1 ).toString();

        if( !defaultCore.isNull() ) {
            defaultCores.insert( system, defaultCore );
        }

        systemList.append( system );
    }

    systemDBQuery.finish();

    systemList.sort( Qt::CaseInsensitive );

    // Grab the user's default core choices from the user DB
    auto userDB = UserDatabase::instance()->database();
    auto userDBQuery = QSqlQuery( userDB );
    execStatus = userDBQuery.exec( QStringLiteral( "SELECT DISTINCT system, defaultCore FROM " ) +
                                   UserDatabase::tableDefaultCores + QStringLiteral( ";" ) );
    Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( userDBQuery.lastError().text() ) );

    while( userDBQuery.next() ) {
        auto system = userDBQuery.value( 0 ).toString();
        auto defaultCore = userDBQuery.value( 1 ).toString();

        if( !defaultCore.isNull() ) {
            systemToCoresMap.insert( system, QStringList{ defaultCore } );
        }
    }

    userDBQuery.finish();

    auto transaction = userDB.transaction();
    Q_ASSERT_X( transaction, Q_FUNC_INFO, qPrintable( userDB.lastError().text() ) );

    // Copy the systems and their default core over to the user database (and model list) that aren't already there
    for( auto system : defaultCores.keys() ) {
        if( !systemToCoresMap.contains( system ) ) {
            qCDebug( phxLibrary ) << "Adding missing system" << system << "and default core" << defaultCores[ system ] << "to user database...";
            execStatus = userDBQuery.exec( QStringLiteral( "INSERT INTO " ) + UserDatabase::tableDefaultCores +
                                           QString( " (system, defaultCore) VALUES (\'%1\', \'%2');" ).arg( system, defaultCores[ system ] ) );
            Q_ASSERT_X( execStatus, Q_FUNC_INFO, qPrintable( userDBQuery.lastError().text() ) );
            systemToCoresMap.insert( system, QStringList{ defaultCores[ system ] } );

        }
    }

    auto commit = userDB.commit();
    Q_ASSERT_X( commit, Q_FUNC_INFO, qPrintable( userDB.lastError().text() ) );

    userDBQuery.finish();

    // Grab all the other available cores from the system DB
    execStatus = systemDBQuery.exec( QStringLiteral( "SELECT core, system FROM cores" ) );
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

    // Sort cores, store index of default core after the list is sorted
    for( auto system : systemList ) {
        auto defaultCore = systemToCoresMap[ system ][ 0 ];

        QStringList &list = systemToCoresMap[ system ];
        list.sort( Qt::CaseInsensitive );
        defaultCoreIndex[ system ] = list.indexOf( defaultCore );
    }

    // Print sorted list (debug)
    // for( auto system : systemList ) {
    //     qCDebug( phxLibrary ) << system << coreListMap[ system ];
    // }

    endResetModel();

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
            return systemToCoresMap[ system ];
        }

        case SystemRole:
            return systemList[ index.row() ];

        case DefaultCoreIndexRole: {
            auto &system = systemList.at( index.row() );
            return defaultCoreIndex[ system ];
        }

        default:
            Q_UNREACHABLE();
            break;
    }

}

QHash<int, QByteArray> CoreModel::roleNames() const {
    return mRoleNames;
}

void CoreModel::save( const QString system, const QString defaultCore ) {
    auto db = UserDatabase::instance()->database();
    auto transaction = db.transaction();
    Q_ASSERT_X( transaction, Q_FUNC_INFO, qPrintable( db.lastError().text() ) );

    auto query = QSqlQuery( db );
    auto statement = QString( "UPDATE defaultCores SET defaultCore = \'%1\' WHERE system = \'%2\'" ).arg( defaultCore, system );

    auto exec = query.exec( statement );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

    auto commit = db.commit();
    Q_ASSERT_X( commit, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

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
