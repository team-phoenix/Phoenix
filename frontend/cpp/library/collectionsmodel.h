#ifndef COLLECTIONSMODEL_H
#define COLLECTIONSMODEL_H

#include "libraryinternaldatabase.h"

#include <QSqlTableModel>
#include <QHash>
#include <QVariantMap>

namespace Library {

    class CollectionsModel : public QSqlTableModel {
            Q_OBJECT
        public:
            explicit CollectionsModel( QObject *parent = 0 );

            enum RowRole {
                RowIndicesRole = Qt::UserRole + 1,
                CollectionNameRole,
                CollectionIDRole,
            };

            virtual QHash<int, QByteArray> roleNames() const override;
            virtual bool select() override;
            virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
            virtual void sort( int column, Qt::SortOrder order ) override;

        public:
            void append( const QString name );

        private:
            explicit CollectionsModel( LibraryInternalDatabase &db, QObject *parent = 0 );

            explicit CollectionsModel( LibraryInternalDatabase *db, QObject *parent = 0 );

            LibraryInternalDatabase *mLibraryDatabase;
            QHash<int, QByteArray> mRoleNames;
            QVariantList params;

    };
}

#endif // COLLECTIONSMODEL_H
