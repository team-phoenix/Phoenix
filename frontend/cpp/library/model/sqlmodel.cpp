#include "sqlmodel.h"

#include <QDebug>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

#include <QStringBuilder>

#include <QDir>
#include <QFile>

int SqlModel::tableRowIndex = -1;

SqlModel::SqlModel( QObject *parent )
    : QSqlTableModel( parent ),
      mCacheModel( false ),
      mAutoCreate( false )
{
}

void SqlModel::setRoleName( const int role, const QByteArray &value) {
    mRoleNames.insert( roleIndex( role ), value );
    mNameToRoleMap.insert( value, roleIndex( role ) );
}

void SqlModel::openDatabase( const QString &connection, const QString &dbName, const QString &dbAbsoluteFile ) {
    mDbAbsoluteFilePath = dbAbsoluteFile;

    Q_ASSERT( !dbAbsoluteFile.isEmpty() );

    QSqlDatabase db;
    if ( QSqlDatabase::contains( dbName ) ) {
        db = QSqlDatabase::database( dbName );
    } else {
        db = QSqlDatabase::addDatabase( connection, dbName );
        db.setDatabaseName( dbAbsoluteFile );
        if ( !mDatabaseSettings.userName().isEmpty() ) {
            db.setUserName( mDatabaseSettings.userName() );
        }
        if ( !mDatabaseSettings.password().isEmpty() ) {
            db.setPassword( mDatabaseSettings.password() );
        }
        if ( !mDatabaseSettings.connectionOptions().isEmpty() ) {
            db.setConnectOptions( mDatabaseSettings.connectionOptions() );
        }
        if ( mDatabaseSettings.port() != INT_MIN ) {
            db.setPort(  mDatabaseSettings.port() );
        }
        if ( !mDatabaseSettings.hostName().isEmpty() ) {
            db.setHostName( mDatabaseSettings.hostName() );
        }
        if ( mDatabaseSettings.numericalPrecisionPolicy() != DatabaseSettings::NumericalPrecisionPolicy::Default ) {
            db.setNumericalPrecisionPolicy( static_cast<QSql::NumericalPrecisionPolicy>( mDatabaseSettings.numericalPrecisionPolicy() ) );
        }
    }

    qDebug( "Opening database %s", qPrintable( db.databaseName() ) );

    if ( !db.isOpen() ) {
        bool o = db.open();
        Q_ASSERT( o );
    }
}

bool SqlModel::addRow( const QVariantMap rowData ) {

    QString statement = QStringLiteral( "INSERT INTO ") % tableName();
    QString colStatement = QStringLiteral( "(" );
    QString valueStatement = QStringLiteral( " VALUES(");

    int i=0;
    for ( const QString &col: rowData.keys() ) {
        if ( i == rowData.size() - 1 ) {
            colStatement += col + QStringLiteral( ")" );
            valueStatement += QStringLiteral( "?);" );
        } else {
            colStatement += col + QStringLiteral( "," );
            valueStatement += QStringLiteral( "?," );
        }
        ++i;
    }
    statement += colStatement + valueStatement;

    QMap<int, QVariant> newCacheMap;

    QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
    bool t = db.transaction();
    Q_ASSERT( t );

    QSqlQuery query( db );

    query.prepare( statement );


    for ( const QString &col: rowData.keys() ) {
        const QVariant val = rowData.value( col );
        query.addBindValue( val );
        int code = mNameToRoleMap.value( col.toLocal8Bit(), -1 );
        Q_ASSERT_X( code != -1, "SqlModel::addRow( const QVariantMap )", "column provided is not in the model." );
        newCacheMap.insert( code, val );
    }

    if ( !query.exec() ) {
        qDebug() << Q_FUNC_INFO << query.lastError().text();
        bool r = db.rollback();
        Q_ASSERT( r );
        return false;
    }


    bool c = db.commit();
    Q_ASSERT( c );

    beginInsertRows( QModelIndex(), rowCount(),  rowCount() );

    if ( cacheModel() ) {
        cachedModel.append( newCacheMap );
    }

    endInsertRows();

    select();


    return true;
}

bool SqlModel::deleteRow( int index, const QString column, const QVariant where ) {

    beginRemoveRows( QModelIndex(), index, index );

    cachedModel.removeAt( index );

    QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() ) ;
    bool t = db.transaction();
    Q_ASSERT( t );
    QSqlQuery query( db );

    QString statement = QStringLiteral( "DELETE FROM " ) % tableName() % QStringLiteral( " WHERE " );
    statement += column % QStringLiteral( "=?" );

    query.prepare( statement );
    query.addBindValue( where );
    if ( !query.exec() ) {
        qDebug() << Q_FUNC_INFO << query.lastError().text();
        bool r = db.rollback();
        Q_ASSERT( r );
        return false;
    }

    bool c = db.commit();
    Q_ASSERT( c );

    endRemoveRows();

    select();

    return true;
}

bool SqlModel::updateRow( int index, const QString column, const QVariant oldData, const QVariant newData ) {

    QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
    bool t = db.transaction();
    Q_ASSERT( t );

    QSqlQuery query( db );
    QString statement = QStringLiteral( "UPDATE " );
    statement += mTableName % QStringLiteral( " SET " );
    statement += column % QStringLiteral( "=? " );
    statement += QStringLiteral( "WHERE " ) % column % QStringLiteral( "=?;" );

    query.prepare( statement );
    query.addBindValue( newData );
    query.addBindValue( oldData );

    if ( !query.exec() ) {
        qCritical() << Q_FUNC_INFO << statement << "\n" << query.lastError().text();
        Q_ASSERT( false );
        return false;
    }

    if ( cacheModel() ) {
        QMap<int, QVariant> map = cachedModel.at( index );

        map.insert( mNameToRoleMap.value( column.toLocal8Bit() ), newData );
        cachedModel.replace( index, map );
    }
    db.commit();

    // All this to update a row? really!?!?!
    QModelIndex ix = this->index( index, mNameToRoleMap.value( column.toLocal8Bit() ) - Qt::UserRole );
    emit dataChanged( ix, ix );

    select();

    return true;
}

QString SqlModel::createFilter() {

    QString result;
    for ( const QString &col : mFilterMap.keys() ) {

        QVariantHash hash = mFilterMap.value( col ).toHash();
        const QString statement = hash.value( filterStatementKey ).toString();

        if ( !result.isEmpty() ) {
            result += QStringLiteral( " AND " ) + statement;
        } else {
            result += statement;
        }
    }

    return result;
}

void SqlModel::clearDatabase() {

    beginRemoveRows( QModelIndex(), 0, rowCount() );

    if ( QSqlDatabase::contains( mDatabaseSettings.connectionName() ) ) {
        QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
        db.close();
    }

    QFile file( mDbAbsoluteFilePath );

    cachedModel.clear();

    if ( file.remove() ) {
        openDatabase( mDatabaseSettings.driverType(), mDatabaseSettings.connectionName(), mDbAbsoluteFilePath );
        createTable();
    }

    endRemoveRows();

    select();

}

QHash<int, QByteArray> SqlModel::roleNames() const {
    return mRoleNames;
}

bool SqlModel::select() {
    const QString s = selectStatement();

    if( s.isEmpty() ) {
        return false;
    }

    beginResetModel();

    //    d->clearCache();

    QSqlQuery query( QSqlDatabase::database( mDatabaseSettings.connectionName() ) );

    query.prepare( s );

    if ( !filter().isEmpty() ) {
        for( const QString &col : mFilterMap.keys() ) {
            const QVariantHash hash = mFilterMap.value( col ).toHash();
            query.addBindValue( hash.value( filterValueKey ) );
        }
    }

    if( !query.exec() ) {
        qDebug() << "LibraryModel::select() error: " << query.lastError().text();
    }

    setQuery( query );

    if( !query.isActive() || lastError().isValid() ) {
        setTable( mTableName ); // resets the record & index
        endResetModel();
        return false;
    }

    endResetModel();
    return true;
}

QString SqlModel::selectStatement() const {

    QString statement = QStringLiteral( "SELECT " );

    for ( int i=0; i < mTableColumns.size(); ++i ) {

        SqlColumn *col = mTableColumns[ i ];
        statement += col->name();
        if ( i != mTableColumns.size() - 1 ) {
            statement += QStringLiteral( ", " );
        }
    }

    statement += QStringLiteral( " FROM " ) % tableName();

    if ( !filter().isEmpty() ) {
        statement += " WHERE " % filter();
    }

    statement += QStringLiteral( ";" );

    return statement;
}

QVariant SqlModel::data( const QModelIndex &index, int role ) const {
    QVariant result = QSqlQueryModel::data( index, role);

    if ( role < Qt::UserRole || !mRoleNames.contains( role ) ) {
        return result;
    }

    int columnIdx = record().indexOf( mRoleNames.value( role  ) );
    return QSqlQueryModel::data( this->index( index.row(), columnIdx ), Qt::DisplayRole );

    return result;
}

bool SqlModel::cacheModel() const
{
    return mCacheModel;
}

QString SqlModel::tableName() const
{
    return mTableName;
}

QUrl SqlModel::fileLocation() const
{
    return mFileLocation;
}

bool SqlModel::autoCreate() const
{
    return mAutoCreate;
}

QQmlListProperty<SqlColumn> SqlModel::tableColumns()
{
    return QQmlListProperty<SqlColumn>( this
                                        , Q_NULLPTR
                                        , &appendTableRow
                                        , Q_NULLPTR
                                        , Q_NULLPTR
                                        , Q_NULLPTR );
}

void SqlModel::appendTableRow(QQmlListProperty<SqlColumn> *list, SqlColumn *row) {
    tableRowIndex++;

    SqlModel *model = qobject_cast<SqlModel *>( list->object );
    Q_ASSERT( model );

    model->setRoleName( tableRowIndex, row->name().toLocal8Bit() );
    model->mTableColumns.append( row );

}

void SqlModel::setCacheModel(const bool cache) {
    mCacheModel = cache;
    emit cacheModelChanged();
}

void SqlModel::setTableName(const QString tableName) {
    mTableName = tableName;
    emit tableNameChanged();
}

void SqlModel::setFileLocation( const QUrl location ) {
    mFileLocation = location;
    emit fileLocationChanged();
}

void SqlModel::setAutoCreate(const bool create) {
    mAutoCreate = create;
    emit autoCreateChanged();
}

void SqlModel::setFilter(const QString column, const QVariant value, const SqlModel::FilterType type) {

    QString tableColumn = mTableName + QStringLiteral( "." ) % column;
    QString comparison = ( type == FilterType::Like ) ? QStringLiteral( "LIKE" ) : QStringLiteral( "=");

    QString filterVal = tableColumn % QStringLiteral( " " ) % comparison % QStringLiteral( " ?") ;
    QVariantHash h;

    h.insert( filterValueKey, value.toString() );
    h.insert( filterStatementKey, filterVal );
    mFilterMap.insert( tableColumn, h );

    QString newFilter = createFilter();
    QSqlTableModel::setFilter( newFilter );
}

void SqlModel::clearFilter(const QString column) {
    QString tableColumn = mTableName + QStringLiteral( "." ) % column;

    if( mFilterMap.remove( tableColumn ) == 0 ) {
        //return;
    }

    QSqlTableModel::setFilter( createFilter() );
}

void SqlModel::finishModelConstruction() {

    SqlModel::tableRowIndex = -1;

    Q_ASSERT( !tableName().isEmpty() );
    Q_ASSERT( !fileLocation().isEmpty() );

    QString dbFile = fileLocation().toString();
    if ( dbFile.startsWith( QStringLiteral( "qrc:" ) ) ) {
        dbFile = dbFile.remove( QStringLiteral( "qrc:" ) );
    } else if ( dbFile.startsWith( QStringLiteral( "file:" ) ) ) {
        dbFile = dbFile.remove( QStringLiteral( "file:" ) );
    }

    if ( mDatabaseSettings.connectionName().isEmpty() ) {
        mDatabaseSettings.setConnectionName( dbFile );
        qWarning() << "connectionName wasn't set! Multiple connections to"
                   << dbFile << "may share connection names. Prefer to set it!";
    }

    openDatabase( mDatabaseSettings.driverType()
                  , mDatabaseSettings.connectionName()
                  , dbFile );
    createTable();


    QString statement = QStringLiteral( "SELECT " );

    for ( int i=0; i < mTableColumns.size(); ++i ) {

        SqlColumn *col = mTableColumns[ i ];
        statement += col->name();
        if ( i != mTableColumns.size() - 1 ) {
            statement += QStringLiteral( ", " );
        }
    }

    statement += QStringLiteral( " FROM " ) % tableName() % QStringLiteral( ";" );
    beginResetModel();

    if ( cacheModel() ) {
        QSqlQuery query( QSqlDatabase::database( mDatabaseSettings.connectionName() ) );

        qDebug() << statement;

        if ( query.exec( statement ) ) {

            while ( query.next() ) {

                QMap<int, QVariant> map;
                for ( int i=0; i < mTableColumns.size(); ++i ) {
                    map.insert( roleIndex( i ), query.value( i ) );
                }

                if ( !map.isEmpty() ) {
                    cachedModel.append( map );
                }

            }

            return;
        }

        Q_ASSERT( false );
    } else {
        setEditStrategy( QSqlTableModel::OnRowChange );
        setTable( mTableName );
        select();
    }

    endResetModel();

}

void SqlModel::createTable() {

    QSqlQuery query( QSqlDatabase::database( mDatabaseSettings.connectionName() ) );
    QString statement = QStringLiteral( "CREATE TABLE IF NOT EXISTS " )
                        % tableName()
                        % QStringLiteral( "(" );

    for ( int i=0; i < mTableColumns.size(); ++i ) {
        SqlColumn *col = mTableColumns[ i ];
        statement += col->name() + " " + col->type();
        if ( i == mTableColumns.size() - 1 ) {
            statement += QStringLiteral( ");" );
        } else {
            statement += QStringLiteral( ", " );
        }
    }

    if ( !query.exec( statement )  ) {
        qCritical() << Q_FUNC_INFO << query.lastError().text();
        Q_ASSERT( false );
    }

}
