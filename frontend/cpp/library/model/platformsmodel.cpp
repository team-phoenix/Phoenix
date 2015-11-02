#include "platformsmodel.h"

using namespace Library;

PlatformsModel::PlatformsModel( QObject *parent )
    : QAbstractListModel( parent ) {

    auto query = QSqlQuery( LibretroDatabase::database() );

    auto exec = query.exec( QStringLiteral( "SELECT DISTINCT UUID, friendlyName, shortName, manufacturer FROM system WHERE enabled=1 ORDER BY UUID ASC" ) );
    Q_ASSERT_X( exec, Q_FUNC_INFO, qPrintable( query.lastError().text() ) );

    mPlatformsList.append( {QStringLiteral( "All" ), QStringLiteral( "" )} );

    while( query.next() ) {
        // Store a friendlier name, if one exists, in the second spot of the UUID
        QStringList UUID = { query.value( 0 ).toString(), QStringLiteral( "" ) };
        QString friendlyName = query.value( 1 ).toString();
        // QString shortName = query.value( 2 ).toString();
        QString manufacturer = query.value( 3 ).toString();

        if( friendlyName != "" ) {
            UUID[1] = manufacturer + " - " + friendlyName;
        }

        mPlatformsList.append( std::move( UUID ) );
    }

    // Just sort the list so it looks pretty. This shouldn't take that long. There aren't
    // that many platforms.
    // mPlatformsList.sort();

}

int PlatformsModel::rowCount( const QModelIndex &parent ) const {
    Q_UNUSED( parent );
    return mPlatformsList.size();
}

QVariant PlatformsModel::data( const QModelIndex &index, int role ) const {
    if( index.isValid() && role == Qt::DisplayRole ) {
        return get( index.row() );
    }
    return QVariant();
}

Qt::ItemFlags PlatformsModel::flags( const QModelIndex &index ) const {
    if( !index.isValid() ) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags( index ) | Qt::ItemIsEditable;
}

bool PlatformsModel::setData( const QModelIndex &index, const QVariant &value, int role ) {

    if( index.isValid() && role == Qt::EditRole ) {

        mPlatformsList.replace( index.row(), { value.toString(), QStringLiteral( "" ) } );
        emit dataChanged( index, index );
        return true;
    }

    return false;
}

int PlatformsModel::count() {
    auto c = rowCount( QModelIndex() );
    emit countChanged();
    return c;
}

QVariant PlatformsModel::get( int index ) const {
    // Off the edge
    if( index >= mPlatformsList.size() ) {
        return QVariant();
    }

    return mPlatformsList[ index ];

}

void PlatformsModel::append( QStringList values ) {

    beginInsertRows( QModelIndex(), mPlatformsList.size(), mPlatformsList.size()
                     + values.size() - 1 );

    for( auto &value : values ) {
        mPlatformsList.append( { value, QStringLiteral( "" ) } );
    }

    endInsertRows();
}

void PlatformsModel::clear() {
    for( int i = 0; i < mPlatformsList.size(); ++i ) {
        remove( i );
    }
}

void PlatformsModel::remove( int index ) {
    beginRemoveRows( QModelIndex(), index, index );
    mPlatformsList.removeAt( index );
    endRemoveRows();
}

// A move operation needs to be called like this 'move( index, index + 1 );'
void PlatformsModel::move( int index, int index2 ) {

    if( index >= mPlatformsList.size() || index2 >= mPlatformsList.size() ) {
        return;
    }

    beginMoveRows( QModelIndex(), index, index, QModelIndex(), index2 + 1 );
    mPlatformsList.swap( index, index2 );
    endMoveRows();
}
