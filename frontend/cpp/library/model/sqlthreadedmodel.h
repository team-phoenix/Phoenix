#pragma once

#include "sqlmodel.h"

#include <QObject>
#include <QAbstractTableModel>
#include <QThread>

// SqlThreadedModel is a threaded SQL model that keeps track of and
// forwards slot function calls to an internal SqlModel. In other words, it is a
// proxy model.

// This class will technically be slower than SqlModel, so if your database queries
// are very fast, just use SqlModel. This is for queries that will hang the UI
// thread significantly if not threaded. On a good note, I have not noticed any slowdowns at all
// while using model instead of an SqlModel.

// Used the exact same way as SqlModel.

class SqlThreadedModel : public QAbstractTableModel {
        Q_OBJECT

        // [!!] These Q_PROPERTY's must always be the same as the ones in SqlModel
        Q_PROPERTY( bool cacheModel READ cacheModel WRITE setCacheModel NOTIFY cacheModelChanged )
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
        explicit SqlThreadedModel( QObject *parent = nullptr );
        ~SqlThreadedModel();

        QHash<int, QByteArray> roleNames() const override;
        int rowCount( const QModelIndex &parent ) const override;
        int columnCount( const QModelIndex &parent ) const override;
        QVariant data( const QModelIndex &index, int role ) const override;

        // [!!] QML Q_PROPERTY ( Getters )
        bool cacheModel() const;
        QString tableName() const;
        QUrl fileLocation() const;
        bool autoCreate() const;
        DatabaseSettings *databaseSettings();
        // ~[!!]

        // [!!] QML Q_PROPERTY ( Setters )
        void setCacheModel( const bool cache );
        void setTableName( const QString tableName );
        void setFileLocation( const QUrl location );
        void setAutoCreate( const bool create );
        // ~[!!]

        // [!!] QML QQmlListProperties ( Hooks )
        QQmlListProperty<SqlColumn> tableColumns();
        static void appendTableRow( QQmlListProperty<SqlColumn> *list, SqlColumn *row );
        // [!!]

    signals:
        // [!!] QML Q_PROPERTY signals
        void cacheModelChanged();
        void tableNameChanged();
        void fileLocationChanged();
        void autoCreateChanged();
        // ~[!!]

    public slots:
        // Sets an SQL filter on the query. The added filter will remain until
        // 'clearFilter()' is called on the filtered column.
        void setFilter( const QString column
                        , const QVariant value
                        , const SqlModel::FilterType type = SqlModel::FilterType::Like );

        // Removes an SQL filter on the query.
        void clearFilter( const QString column );

        // This function needs to be called once all the Q_PROPERTIES have been assigned.
        // This is called in Component.OnCompleted.
        void finishModelConstruction();

        // Inserts a new row into the sql model.
        bool addRow( const QVariantMap rowData );

        //Remove row from sql model.
        bool deleteRow( int index, const QString column, const QVariant absFilePath );

        // Updates the row with the new value;
        bool updateRow( int index, const QString column, const QVariant oldData, const QVariant newData );

        // Deletes the database and resets the sql schema. The model is now initialized
        // but is still empty of rows.
        void clearDatabase();

    private Q_SLOTS:
        // ~[!!] Slots that connect to the internal SqlModel and
        // repeat 'beginResetModel' and 'endResetModel' to update the view attached
        // to this thread.
        void aboutToResetModel();
        void resetModel();

    private:
        QThread mModelThread;
        SqlModel mModel;
};
