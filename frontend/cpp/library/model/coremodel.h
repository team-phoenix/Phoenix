#ifndef COREMODEL_H
#define COREMODEL_H

#include "frontendcommon.h"

#include "userdatabase.h"
#include "libretrodatabase.h"
#include "frontendcommon.h"
#include "phxpaths.h"

/*
 * Used from QML to display available systems (role: "system", a QString).
 * and a secondary model of what cores are available for that system (role: "cores", a QStringList).
 * The default core the user has chosen for a given system is given in the form of an index into the "cores" model.
 * Access it with role: "defaultCoreIndex", an int.
 */

namespace Library {

    class CoreModel : public QAbstractTableModel {
            Q_OBJECT
        public:
            explicit CoreModel( QObject *parent = 0 );

            enum DefaultCoreRoles {
                SystemRole = Qt::UserRole + 1,
                CoresRole,
                DefaultCoreIndexRole
            };

            QModelIndex index( int row, int column, const QModelIndex &parent ) const override {
                return hasIndex( row, column, parent ) ? createIndex( row, column, nullptr ) : QModelIndex();
            }

            int rowCount( const QModelIndex &parent ) const override;

            int columnCount( const QModelIndex &parent ) const override;

            QVariant data( const QModelIndex &index, int role ) const override;

            QHash<int, QByteArray> roleNames() const override;

        signals:

        public slots:

            // Immediately save new default core to user db
            void save( const QString system, const QString core );

            // Check that the given core exists on the filesystem where it should be
            bool coreExists( QString defaultCore );

        protected:
            QHash<int, QByteArray> mRoleNames;
            QStringList systemList;
            QMap<QString, QStringList> systemToCoresMap;
            QMap<QString, int> defaultCoreIndex;
    };

}

#endif // COREMODEL_H
