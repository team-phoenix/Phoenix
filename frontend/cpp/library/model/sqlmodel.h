#pragma once

#include "frontendcommon.h"

#include "sqlcolumn.h"
#include "databasesettings.h"

const static QString filterValueKey = QStringLiteral( "bindValue" );
const static QString filterStatementKey = QStringLiteral( "filter" );

class SqlModel : public QSqlTableModel {
        Q_OBJECT
        // Q_PROPERTY( bool cacheModel READ cacheModel WRITE setCacheModel NOTIFY cacheModelChanged )
        Q_PROPERTY( bool autoCreate READ autoCreate WRITE setAutoCreate NOTIFY autoCreateChanged )

        Q_PROPERTY( QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged )
        Q_PROPERTY( QUrl fileLocation READ fileLocation WRITE setFileLocation NOTIFY fileLocationChanged )

        Q_PROPERTY( DatabaseSettings *databaseSettings READ databaseSettings )

        Q_PROPERTY( QQmlListProperty<SqlColumn> tableColumns READ tableColumns )

        // This enables SqlModel to use SqlColumns declared as children and
        // use them in the 'tableColumns' Q_PROPERTY, without needing to directly
        // assign them.
        Q_CLASSINFO( "DefaultProperty", "tableColumns" )

    public:
        enum class FilterType {
            Exact = 0,
            Like,
        };
        Q_ENUMS( FilterType )

        SqlModel( QObject *parent = nullptr );

        void setRoleName( const int role, const QByteArray &value );

        // [!!] Reimplemented virtual model functions.
        QHash<int, QByteArray> roleNames() const override;
        bool select() override;
        QString selectStatement() const override;
        QVariant data( const QModelIndex &index, int role ) const override;
        // ~[!!]

        // [!!] QML Q_PROPERTY ( Getters )
        bool cacheModel() const;
        QString tableName() const;
        QUrl fileLocation() const;
        bool autoCreate() const;

        // !![WARNING]!!
        // Only meant to be accessed from QML, this is not a deletable pointer!
        DatabaseSettings *databaseSettings() {
            return &mDatabaseSettings;
        }
        // ~!![WARNING]!!

        // ~[!!]

        // [!!] QML Q_PROPERTY ( Setters )
        void setCacheModel( const bool cache );
        void setTableName( const QString tableName );
        void setConnectionName( const QString name );
        void setFileLocation( const QUrl location );
        void setAutoCreate( const bool create );
        // ~[!!]

        // [!!] QML QQmlListProperties ( Hooks )
        QQmlListProperty<SqlColumn> tableColumns();
        static void appendTableRow( QQmlListProperty<SqlColumn> *list, SqlColumn *row );
        // [!!]

    protected:
        QList<SqlColumn *> mTableColumns;

    signals:
        // [!!] QML Q_PROPERTY signals
        void cacheModelChanged();
        void tableNameChanged();
        void connectionNameChanged();
        void fileLocationChanged();
        void autoCreateChanged();

    public slots:
        // Sets an SQL filter on the query. The added filter will remain until
        // 'clearFilter()' is called on the filtered column.
        void setFilter( const QString column
                        , const QVariant value
                        , const SqlModel::FilterType type = FilterType::Like );

        // Removes an SQL filter on the query.
        void clearFilter( const QString column );

        // This function needs to be called once all the Q_PROPERTIES have been assigned.
        // This is called in Component.OnCompleted.
        void finishModelConstruction();

        // Inserts a new row into the sql model.
        bool addRow( const QVariantMap rowData );

        //Remove row from sql model.
        bool deleteRow( int index, const QString column, const QVariant where );

        // Updates the row with the new value;
        bool updateRow( int index, const QString column, const QVariant oldData, const QVariant newData );

        // Deletes the database and resets the sql schema. The model is now initialized
        // but is still empty of rows.
        void clearDatabase();

    private:
        QString mDbAbsoluteFilePath;

        // QML Properties
        bool mCacheModel;
        QString mTableName;
        QString mConnectionName;
        QUrl mFileLocation;
        bool mAutoCreate;

        // No need to delete, is parented to model.
        DatabaseSettings mDatabaseSettings;

        QHash<int, QByteArray> mRoleNames;
        QHash<QByteArray, int>  mNameToRoleMap;

        QVariantHash mFilterMap;
        QList< QMap<int, QVariant> >cachedModel;

        // [!!] Helpers
        void createTable();
        QString createFilter();
        void openDatabase( const QString &connection, const QString &dbName, const QString &dbAbsoluteFile );

        inline int roleIndex( const int index );
        // ~[!!]
};

int SqlModel::roleIndex( const int index )  {
    return Qt::UserRole + index;
}
