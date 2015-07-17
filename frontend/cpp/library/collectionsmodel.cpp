#include "collectionsmodel.h"
#include "logging.h"

#include <QSqlError>
#include <QSqlRecord>

using namespace Library;

CollectionsModel::CollectionsModel( QObject *parent )
    : CollectionsModel( LibraryInternalDatabase::instance(), parent ) {

}

CollectionsModel::CollectionsModel( LibraryInternalDatabase *db, QObject *parent )
    : CollectionsModel( *db, parent ) {

}

CollectionsModel::CollectionsModel( LibraryInternalDatabase &db, QObject *parent )
    : QSqlTableModel( parent, db.database() ) {

    mRoleNames =  QSqlTableModel::roleNames();
    mRoleNames.insert( CollectionIDRole, QByteArrayLiteral( "collectionID" ) );
    mRoleNames.insert( CollectionNameRole, QByteArrayLiteral( "collectionName" ) );

    setEditStrategy( QSqlTableModel::OnManualSubmit );
    setTable( LibraryInternalDatabase::tableCollections );
    select();
}

QVariant CollectionsModel::data( const QModelIndex &index, int role ) const {
    QVariant value = QSqlTableModel::data( index, role );

    if( role < Qt::UserRole ) {
        return value;
    } else {
        if( !mRoleNames.contains( role ) ) {
            return value;
        }

        // role name need to be the same as column name.
        int columnIdx = record().indexOf( mRoleNames.value( role ) );
        return QSqlTableModel::data( this->index( index.row(), columnIdx ), Qt::DisplayRole );
    }
}

void CollectionsModel::sort( int column, Qt::SortOrder order ) {
    QSqlTableModel::sort( column, order );
}

void CollectionsModel::append( const QVariantMap dict ) {

    static const auto insertCollectionStatement = QStringLiteral( "INSERT INTO " )
            + LibraryInternalDatabase::tableCollections
            + QStringLiteral( " (collectionID, collectionName) " )
            + QStringLiteral( "VALUES (?,?)" );

    if( dict.size() < 2 ) {
        qCWarning( phxLibrary ) << "CollectionsModel::append() doesn't have enough values in the map.";
        return;
    }

    database().transaction();

    QSqlQuery query( database() );
    query.prepare( insertCollectionStatement );
    query.addBindValue( dict.value( "collectionID" ).toString() );
    query.addBindValue( dict.value( "collectionName" ).toString() );


    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Sql Collection Insert Error: " << query.lastError().text();
    }

    if( submitAll() ) {
        database().commit();
    } else {
        database().rollback();
    }
}

void CollectionsModel::setFilter( const CollectionRoles role, const QVariant id ) {
    //SELECT games.title, games.system, collections.collectionName, collections.collectionID FROM collections INNER JOIN games ON collections.collectionID  = 1

    filterParameterMap.insert( role, id );

    select();
}

QHash<int, QByteArray> CollectionsModel::roleNames() const {
    return mRoleNames;
}

bool CollectionsModel::select() {
    const QString query = selectStatement().insert(7, "DISTINCT " );

    qDebug() << query;

    if( query.isEmpty() ) {
        return false;
    }

    beginResetModel();

    //    d->clearCache();

    QSqlQuery qu( database() );
    qu.prepare( query );

    for( auto &role : filterParameterMap.keys() ) {
        qu.addBindValue( filterParameterMap.value( role ) );
    }

    qu.exec();
    setQuery( qu );

    if( !qu.isActive() || lastError().isValid() ) {
        setTable( tableName() ); // resets the record & index
        endResetModel();
        return false;
    }

    endResetModel();
    return true;
}
