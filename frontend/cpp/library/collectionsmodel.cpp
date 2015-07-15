#include "collectionsmodel.h"

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
    mRoleNames.insert( CollectionNameRole, QByteArrayLiteral( "collectionName" ) );
    mRoleNames.insert( RowIndicesRole, QByteArrayLiteral( "rowIndices" ) );
    mRoleNames.insert( CollectionIDRole, QByteArrayLiteral( "collectionID" ) );

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

void CollectionsModel::append( const QString name ) {
    static const auto insertCollectionStatement = QStringLiteral( "" );
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

    for( auto &val : params ) {
        qu.addBindValue( val );
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

