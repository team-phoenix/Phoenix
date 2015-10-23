#include "collectionsmodel.h"

using namespace Library;

CollectionsModel::CollectionsModel( QObject *parent )
    : CollectionsModel( UserDatabase::instance(), parent ) {

}

CollectionsModel::CollectionsModel( UserDatabase *db, QObject *parent )
    : CollectionsModel( *db, parent ) {

}

CollectionsModel::CollectionsModel( UserDatabase &db, QObject *parent )
    : QSqlTableModel( parent, db.database() ) {

    mRoleNames = QSqlTableModel::roleNames();
    mRoleNames.insert( CollectionIDRole, QByteArrayLiteral( "collectionID" ) );
    mRoleNames.insert( CollectionNameRole, QByteArrayLiteral( "collectionName" ) );

    setEditStrategy( QSqlTableModel::OnManualSubmit );
    setTable( UserDatabase::tableCollections );
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
            + UserDatabase::tableCollections
            + QStringLiteral( " (collectionID, collectionName) " )
            + QStringLiteral( "VALUES (?,?)" );

    if( dict.size() < 2 ) {
        qCWarning( phxLibrary ) << "CollectionsModel::append() doesn't have enough values in the map.";
        return;
    }

    database().transaction();

    QSqlQuery query( database() );
    query.prepare( insertCollectionStatement );
    query.addBindValue( dict.value( "collectionID" ) );
    query.addBindValue( dict.value( "collectionName" ) );

    qDebug() << "BINDING " << query.boundValues();

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Sql Collection Insert Error: " << query.lastError().text();
    }

    sync();
}

void CollectionsModel::set( const QVariant id, const QString name ) {
    static const auto updateStatement = QStringLiteral( "UPDATE " )
                                        + UserDatabase::tableCollections
                                        + " SET collectionName = ? WHERE collectionID = ?";

    database().transaction();

    QSqlQuery query( database() );
    query.prepare( updateStatement );
    query.addBindValue( name );
    query.addBindValue( id );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Collection Update Error: " << query.lastError().text();
        return;
    }

    sync();

}

void CollectionsModel::remove( const QVariant id ) {
    static const auto removeStatement = QStringLiteral( "DELETE FROM " )
                                        + UserDatabase::tableCollections
                                        + QStringLiteral( " WHERE collectionID = ?" );
    database().transaction();

    QSqlQuery query( database() );
    query.prepare( removeStatement );
    query.addBindValue( id );

    if( !query.exec() ) {
        qCWarning( phxLibrary ) << "Collection Remove Error: " << query.lastError();
    }

    sync();
}

void CollectionsModel::sync() {
    if( !submitAll() ) {
        database().rollback();
    } else {
        database().commit();
    }
}

void CollectionsModel::setFilter( const CollectionRoles role, const QVariant id ) {

    filterParameterMap.insert( role, id );

    select();
}

QHash<int, QByteArray> CollectionsModel::roleNames() const {
    return mRoleNames;
}

bool CollectionsModel::select() {
    const QString query = selectStatement();

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
