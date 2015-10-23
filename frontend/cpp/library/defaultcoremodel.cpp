#include "defaultcoremodel.h"
#include "systemdatabase.h"
#include "frontendcommon.h"
#include "phxpaths.h"

using namespace Library;

DefaultCoreModel::DefaultCoreModel( QObject *parent )
    : QAbstractTableModel( parent ) {

    mRoleNames.insert( DefaultCoreRoles::CoresRole, QByteArrayLiteral( "cores" ) );
    mRoleNames.insert( DefaultCoreRoles::SystemRole, QByteArrayLiteral( "system" ) );
    mRoleNames.insert( DefaultCoreRoles::DefaultCoreIndexRole, QByteArrayLiteral( "defaultCoreIndex" ) );

    auto db = QSqlQuery( SystemDatabase::database() );
    auto exec = db.exec( QStringLiteral( "SELECT DISTINCT system, defaultCore FROM systems;" ) );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( db.lastError().text() ) );

    beginResetModel();

    while( db.next() ) {
        auto system = db.value( 0 ).toString();
        auto core = db.value( 1 ).toString();

        if( !core.isNull() ) {
            coreListMap.insert( system, QStringList{ core } );
        }

        systemList.append( system );
    }

    systemList.sort( Qt::CaseInsensitive );

    exec = db.exec( QStringLiteral( "SELECT core, system FROM cores" ) );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( db.lastError().text() ) );

    while( db.next() ) {
        auto core = db.value( 0 ).toString();
        auto system = db.value( 1 ).toString();

        auto &list = coreListMap[ system ];

        if( !list.contains( core ) ) {
            list.append( core );
        }
    }

    // Sort cores, store index of default core after the list is sorted
    for( auto system : systemList ) {
        auto defaultCore = coreListMap[ system ][ 0 ];

        QStringList &list = coreListMap[ system ];
        list.sort( Qt::CaseInsensitive );
        defaultCoreIndex[ system ] = list.indexOf( defaultCore );
    }

    // Print sorted list (debug)
    // for( auto system : systemList ) {
    //     qCDebug( phxLibrary ) << system << coreListMap[ system ];
    // }

    endResetModel();

}

int DefaultCoreModel::rowCount( const QModelIndex &parent ) const {
    if( parent.isValid() ) {
        return 0;
    }

    return systemList.size();
}

int DefaultCoreModel::columnCount( const QModelIndex &parent ) const {
    if( parent.isValid() ) {
        return 0;
    }

    return 2;
}

QVariant DefaultCoreModel::data( const QModelIndex &index, int role ) const {
    if( !index.isValid() || index.row() >= systemList.size() || index.row() < 0 ) {
        return QVariant();
    }

    switch( role ) {
        case CoresRole: {
            auto &system = systemList.at( index.row() );
            return coreListMap[ system ];
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

QHash<int, QByteArray> DefaultCoreModel::roleNames() const {
    return mRoleNames;
}

void DefaultCoreModel::save( const QString system, const QString defaultCore ) {
    auto db = SystemDatabase::database();
    auto transaction = db.transaction();
    Q_ASSERT( transaction );

    auto query = QSqlQuery( db );
    auto statement = QString( "UPDATE systems SET defaultCore = \'%1\' WHERE system = \'%2\'" ).arg( defaultCore, system );
    //db.prepare( statement );
    //db.addBindValue( defaultCore );
    //db.addBindValue( system );

    auto exec = query.exec( statement );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

    auto commit = db.commit();
    Q_ASSERT_X( commit, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

}

bool DefaultCoreModel::coreExists( QString core ) {
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

