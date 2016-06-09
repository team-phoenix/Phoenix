#ifndef COREMODEL_H
#define COREMODEL_H

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
                SystemFriendlyNameRole,
                NumSystemsRole,
                CoresRole,
                CurrentCoreIndexRole,
                DefaultCoreIndexRole,
                DefaultCoreRole
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
            void save( const QString system, const QString newCurrentCore );
            void save( const QString system, int newCurrentCoreIndex );

            // Check that the given core exists on the filesystem where it should be
            bool coreExists( QString defaultCore );

        protected:
            // Role names used from within QML to access this model's data
            QHash<int, QByteArray> mRoleNames;

            // List of all available systems, serves as indicies for the maps below
            QStringList systemList;
            QStringList systemFriendlyNameList;

            // Core lists indexed by given system
            QMap<QString, QStringList> systemToCoresMap;

            // Core lists indexed by given system, cores that are the default are marked with a label: "(default)"
            QMap<QString, QStringList> systemToCoreFriendlyNamesMap;

            // Contains default core for each system
            QMap<QString, QString> systemToDefaultCoreMap;

            // Index of the default core in the given system's core list, generated from systemToDefaultCoreMap
            QMap<QString, int> defaultCoreIndex;

            // Index of user's choice for current core in the given system's core list
            QMap<QString, int> currentCoreIndex;
    };

}

#endif // COREMODEL_H
