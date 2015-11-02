#ifndef COREMODEL_H
#define COREMODEL_H

#include "frontendcommon.h"

#include "userdatabase.h"
#include "libretrodatabase.h"
#include "frontendcommon.h"
#include "phxpaths.h"

/*
 * Primary model is a sorted list of available systems (role: QString system)
 * and a secondary sorted list of what cores are available for that system (role: QStringList cores).
 * The default core the user has chosen for a given system is given in the form of an index into the "cores" model.
 * (role: int defaultCoreIndex)
 */

namespace Library {

    class CoreModel : public QAbstractTableModel {
            Q_OBJECT
        public:
            explicit CoreModel( QObject *parent = 0 );

            enum CoreModelRoles {
                SystemRole = Qt::UserRole + 1,
                CoresRole,
                DefaultCoreIndexRole
            };

            // Implementations of the base class functions
            QModelIndex index( int row, int column, const QModelIndex &parent ) const override;
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
