#include "sqlmodel.h"

#include "logging.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

#include <QStringBuilder>

#include <QDir>
#include <QFile>

SqlModel::SqlModel( QObject *parent )
    : QSqlTableModel( parent ),
      mCacheModel( false ),
      mAutoCreate( false ) {

    connect( this, &SqlModel::modelReset, this, [this] {
        while( canFetchMore( ) ) {
            fetchMore();
        }
    } );

}

void SqlModel::setRoleName( const int role, const QByteArray &value ) {
    mRoleNames.insert( roleIndex( role ), value );
    mNameToRoleMap.insert( value, roleIndex( role ) );
}

void SqlModel::openDatabase( const QString &connection, const QString &dbName, const QString &dbAbsoluteFile ) {
    mDbAbsoluteFilePath = dbAbsoluteFile;

    Q_ASSERT( !dbAbsoluteFile.isEmpty() );

    QSqlDatabase db;

    if( QSqlDatabase::contains( dbName ) ) {
        db = QSqlDatabase::database( dbName );
    } else {
        db = QSqlDatabase::addDatabase( connection, dbName );
        db.setDatabaseName( dbAbsoluteFile );

        if( !mDatabaseSettings.userName().isEmpty() ) {
            db.setUserName( mDatabaseSettings.userName() );
        }

        if( !mDatabaseSettings.password().isEmpty() ) {
            db.setPassword( mDatabaseSettings.password() );
        }

        if( !mDatabaseSettings.connectionOptions().isEmpty() ) {
            db.setConnectOptions( mDatabaseSettings.connectionOptions() );
        }

        if( mDatabaseSettings.port() != INT_MIN ) {
            db.setPort( mDatabaseSettings.port() );
        }

        if( !mDatabaseSettings.hostName().isEmpty() ) {
            db.setHostName( mDatabaseSettings.hostName() );
        }

        if( mDatabaseSettings.numericalPrecisionPolicy() != DatabaseSettings::NumericalPrecisionPolicy::Default ) {
            db.setNumericalPrecisionPolicy( static_cast<QSql::NumericalPrecisionPolicy>( mDatabaseSettings.numericalPrecisionPolicy() ) );
        }
    }

    qDebug( "Opening database %s", qPrintable( db.databaseName() ) );

    if( !db.isOpen() ) {
        bool o = db.open();
        Q_ASSERT( o );
    }
}

bool SqlModel::addRow( const QVariantMap rowData ) {

    QString statement = QStringLiteral( "INSERT INTO " ) % tableName();
    QString colStatement = QStringLiteral( "(" );
    QString valueStatement = QStringLiteral( " VALUES(" );

    int i = 0;

    for( const QString &col : rowData.keys() ) {
        if( i == rowData.size() - 1 ) {
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

    for( const QString &col : rowData.keys() ) {
        const QVariant val = rowData.value( col );
        query.addBindValue( val );
        int code = mNameToRoleMap.value( col.toLocal8Bit(), -1 );
        Q_ASSERT_X( code != -1, "SqlModel::addRow( const QVariantMap )", "column provided is not in the model." );
        newCacheMap.insert( code, val );
    }

    if( !query.exec() ) {
        qDebug() << Q_FUNC_INFO << query.lastError().text();
        bool r = db.rollback();
        Q_ASSERT( r );
        return false;
    }


    bool c = db.commit();
    Q_ASSERT( c );

    beginInsertRows( QModelIndex(), rowCount(),  rowCount() );

    if( cacheModel() ) {
        cachedModel.append( newCacheMap );
    }

    endInsertRows();
    select();

    return true;
}

bool SqlModel::addRows( const QVariantList rows ) {

    if( rows.isEmpty() ) {
        return false;
    }

    QString statement = QStringLiteral( "INSERT INTO " ) % tableName();
    QString colStatement = QStringLiteral( "(" );
    QString valueStatement = QStringLiteral( " VALUES(" );

    int i = 0;

    QVariantMap rowData = rows.first().toMap();

    for( const QString &col : rowData.keys() ) {
        if( i == rowData.size() - 1 ) {
            colStatement += col + QStringLiteral( ")" );
            valueStatement += QStringLiteral( "?);" );
        } else {
            colStatement += col + QStringLiteral( "," );
            valueStatement += QStringLiteral( "?," );
        }

        ++i;
    }

    statement += colStatement + valueStatement;

    QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
    bool t = db.transaction();

    for( int i = 0; i < rows.size(); ++i ) {
        QVariantMap rowData = rows.at( i ).toMap();
        Q_ASSERT( t );

        QSqlQuery query( db );
        query.prepare( statement );

        for( const QString &col : rowData.keys() ) {
            const QVariant val = rowData.value( col );
            query.addBindValue( val );
            int code = mNameToRoleMap.value( col.toLocal8Bit(), -1 );
            Q_ASSERT_X( code != -1, "SqlModel::addRow( const QVariantMap )", "column provided is not in the model." );
        }

        if( !query.exec() ) {
            qDebug() << Q_FUNC_INFO << query.lastError().text();
            bool r = db.rollback();
            Q_ASSERT( r );
            return false;
        }
    }


    bool c = db.commit();
    Q_ASSERT( c );

    beginInsertRows( QModelIndex(), rowCount(),  rowCount() + rows.size() );

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

    if( !query.exec() ) {
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

    if( !query.exec() ) {
        qCritical() << Q_FUNC_INFO << statement << "\n" << query.lastError().text();
        Q_ASSERT( false );
        return false;
    }

    if( cacheModel() ) {
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

    for( const QString &col : mFilterMap.keys() ) {

        QVariantHash hash = mFilterMap.value( col ).toHash();
        const QString statement = hash.value( filterStatementKey ).toString();

        if( !result.isEmpty() ) {
            result += QStringLiteral( " AND " ) + statement;
        } else {
            result += statement;
        }
    }

    return result;
}

void SqlModel::clearDatabase() {

    beginRemoveRows( QModelIndex(), 0, rowCount() );

    if( QSqlDatabase::contains( mDatabaseSettings.connectionName() ) ) {
        QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
        db.close();
    }

    QFile file( mDbAbsoluteFilePath );

    cachedModel.clear();

    if( file.remove() ) {
        openDatabase( mDatabaseSettings.driverType(), mDatabaseSettings.connectionName(), mDbAbsoluteFilePath );
        createTable();
    }

    endRemoveRows();

    select();

}

bool SqlModel::addEntries( const FileList rows ) {

    QString statement = QStringLiteral( "INSERT INTO " ) % tableName();
    QString colStatement = QStringLiteral( "(" );
    QString valueStatement = QStringLiteral( " VALUES(" );

    // qCDebug( phxLibrary ) << rows;
    qCDebug( phxLibrary ) << "Adding" << rows.size() << "games to the database" << mFileLocation;

    // Grab the column names from the QVariantMap factory function, create an insert statement from them
    {
        QVariantMap rowData = QVariantMap( static_cast<FileEntry>( rows.first() ) );

        int i = 0;

        for( const QString &col : rowData.keys() ) {
            if( i == rowData.size() - 1 ) {
                colStatement += col + QStringLiteral( ")" );
                valueStatement += QStringLiteral( "?);" );
            } else {
                colStatement += col + QStringLiteral( "," );
                valueStatement += QStringLiteral( "?," );
            }

            ++i;
        }
    }

    statement += colStatement + valueStatement;

    QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
    bool t = db.transaction();
    Q_ASSERT( t );

    int ignoredGames = 0;

    // For each row create an insert statement and bind the values of this statement to the current row's values and execute
    for( int i = 0; i < rows.size(); ++i ) {
        QVariantMap rowData = QVariantMap( static_cast<FileEntry>( rows.at( i ) ) );

        QSqlQuery query( db );
        query.prepare( statement );

        for( const QString &col : rowData.keys() ) {
            const QVariant val = rowData.value( col );
            query.addBindValue( val );
            int code = mNameToRoleMap.value( col.toLocal8Bit(), -1 );
            Q_ASSERT_X( code != -1
                        , "SqlModel::addRow( const QVariantMap )"
                        , qPrintable( QString( "column[%1] provided is not in the model" ).arg( col ) ) );
        }

        if( !query.exec() ) {
            if( query.lastError().text().contains( "UNIQUE" ) ) {
                // qCDebug( phxLibrary ) << "Ignoring game already in library";
                ignoredGames++;
                continue;
            }

            qDebug() << Q_FUNC_INFO << query.lastError().text();
            qDebug() << statement << rowData;
            bool r = db.rollback();
            Q_ASSERT( r );
            return false;
        }
    }

    if( ignoredGames ) {
        qCDebug( phxLibrary ) << "Ignored" << ignoredGames << "games already in library";
    }

    bool c = db.commit();
    Q_ASSERT( c );

    beginInsertRows( QModelIndex(), rowCount(), rowCount() + rows.size() );

    endInsertRows();
    select();

    return true;
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

    if( mSelectStatementOverride.isEmpty() && !filter().isEmpty() ) {
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

    if( !mSelectStatementOverride.isEmpty() ) {
        return mSelectStatementOverride;
    }

    QString statement = QStringLiteral( "SELECT " );

    for( int i = 0; i < mTableColumns.size(); ++i ) {

        SqlColumn *col = mTableColumns[ i ];
        statement += col->name();

        if( i != mTableColumns.size() - 1 ) {
            statement += QStringLiteral( ", " );
        }
    }

    statement += QStringLiteral( " FROM " ) % tableName();

    if( !filter().isEmpty() ) {
        statement += " WHERE " % filter();
    }

    statement += mOrderBy;
    statement += QStringLiteral( ";" );

    return statement;
}

QVariant SqlModel::data( const QModelIndex &index, int role ) const {
    if( !index.isValid() ) {
        return QVariant();
    }

    if( role < Qt::UserRole || !mRoleNames.contains( role ) ) {
        return QSqlQueryModel::data( index, role );
    }

    int columnIdx = record().indexOf( mRoleNames.value( role ) );
    QVariant result = QSqlQueryModel::data( this->index( index.row(), columnIdx ), Qt::DisplayRole );
    return result;
}

bool SqlModel::cacheModel() const {
    return mCacheModel;
}

QString SqlModel::tableName() const {
    return mTableName;
}

QUrl SqlModel::fileLocation() const {
    return mFileLocation;
}

bool SqlModel::autoCreate() const {
    return mAutoCreate;
}

DatabaseSettings *SqlModel::databaseSettings() {
    return &mDatabaseSettings;
}

QQmlListProperty<SqlColumn> SqlModel::tableColumns() {
    return QQmlListProperty<SqlColumn>( this
                                        , Q_NULLPTR
                                        , &appendTableRow
                                        , Q_NULLPTR
                                        , Q_NULLPTR
                                        , Q_NULLPTR );
}

void SqlModel::appendTableRow( QQmlListProperty<SqlColumn> *list, SqlColumn *row ) {
    SqlModel *model = qobject_cast<SqlModel *>( list->object );
    Q_ASSERT( model );

    model->setRoleName( model->mTableColumns.size(), row->name().toLocal8Bit() );
    model->mTableColumns.append( row );
}

bool SqlModel::attachDatabase( const QString dbFile, const QString alias ) {
    QSqlDatabase db = QSqlDatabase::database( mDatabaseSettings.connectionName() );
    QSqlQuery query( db );

    if( !query.exec( QString( "ATTACH DATABASE '%1' as %2;" ).arg( dbFile, alias ) ) ) {
        qDebug() << Q_FUNC_INFO << query.lastError().text();
        Q_ASSERT( false );
        return false;
    }

    return true;
}

void SqlModel::setOrderBy( const QStringList columns, const SqlModel::OrderBy order ) {
    if( columns.isEmpty() ) {
        return;
    }

    mOrderBy = QStringLiteral( " ORDER BY " );

    for( int i = 0; i < columns.size(); ++i ) {
        mOrderBy += columns[i];

        if( i < columns.size() - 1 ) {
            mOrderBy += QStringLiteral( "," );
        }
    }

    mOrderBy += ( order == SqlModel::OrderBy::ASC ) ? QStringLiteral( " ASC" ) : QStringLiteral( " DESC" );

}

void SqlModel::setCacheModel( const bool cache ) {
    mCacheModel = cache;
    emit cacheModelChanged();
}

void SqlModel::setTableName( const QString tableName ) {
    mTableName = tableName;
    emit tableNameChanged();
}

void SqlModel::setFileLocation( const QUrl location ) {
    mFileLocation = location;
    emit fileLocationChanged();
}

void SqlModel::setAutoCreate( const bool create ) {
    mAutoCreate = create;
    emit autoCreateChanged();
}

void SqlModel::setSelectStatement( const QString statement ) {
    mSelectStatementOverride = statement;
    emit selectStatementChanged();
}

void SqlModel::setFilter( const QString column, const QVariant value, const SqlModel::FilterType type ) {

    QString tableColumn = mTableName + QStringLiteral( "." ) % column;
    QString comparison = ( type == FilterType::Like ) ? QStringLiteral( "LIKE" ) : QStringLiteral( "=" );

    QString filterVal = tableColumn % QStringLiteral( " " ) % comparison % QStringLiteral( " ?" ) ;
    QVariantHash h;

    h.insert( filterValueKey, value.toString() );
    h.insert( filterStatementKey, filterVal );
    mFilterMap.insert( tableColumn, h );

    QString newFilter = createFilter();
    QSqlTableModel::setFilter( newFilter );
}

void SqlModel::clearFilter( const QString column ) {
    QString tableColumn = mTableName + QStringLiteral( "." ) % column;

    if( mFilterMap.remove( tableColumn ) == 0 ) {
        //return;
    }

    QSqlTableModel::setFilter( createFilter() );
}

void SqlModel::finishModelConstruction() {

    Q_ASSERT( !tableName().isEmpty() );
    Q_ASSERT( !fileLocation().isEmpty() );

    QString dbFile = fileLocation().toString();

    if( dbFile.startsWith( QStringLiteral( "qrc:" ) ) ) {
        dbFile = dbFile.remove( QStringLiteral( "qrc:" ) );
    } else if( dbFile.startsWith( QStringLiteral( "file:" ) ) ) {
        dbFile = dbFile.remove( QStringLiteral( "file:" ) );
    }

    if( mDatabaseSettings.connectionName().isEmpty() ) {
        mDatabaseSettings.setConnectionName( dbFile );
        qWarning() << "connectionName wasn't set! Multiple connections to"
                   << dbFile << "may share connection names. Prefer to set it!";
    }

    openDatabase( mDatabaseSettings.driverType()
                  , mDatabaseSettings.connectionName()
                  , dbFile );
    createTable();


    QString statement = QStringLiteral( "SELECT " );

    for( int i = 0; i < mTableColumns.size(); ++i ) {

        SqlColumn *col = mTableColumns[ i ];
        statement += col->name();

        if( i != mTableColumns.size() - 1 ) {
            statement += QStringLiteral( ", " );
        }
    }

    statement += QStringLiteral( " FROM " ) % tableName() % QStringLiteral( ";" );
    beginResetModel();

    if( cacheModel() ) {
        QSqlQuery query( QSqlDatabase::database( mDatabaseSettings.connectionName() ) );

        qDebug() << statement;

        if( query.exec( statement ) ) {

            while( query.next() ) {

                QMap<int, QVariant> map;

                for( int i = 0; i < mTableColumns.size(); ++i ) {
                    map.insert( roleIndex( i ), query.value( i ) );
                }

                if( !map.isEmpty() ) {
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

    for( int i = 0; i < mTableColumns.size(); ++i ) {
        SqlColumn *col = mTableColumns[ i ];
        statement += col->name() + " " + col->type();

        if( i == mTableColumns.size() - 1 ) {
            statement += QStringLiteral( ");" );
        } else {
            statement += QStringLiteral( ", " );
        }
    }

    if( !query.exec( statement ) ) {
        qCritical() << Q_FUNC_INFO << query.lastError().text();
        Q_ASSERT( false );
    }

}
