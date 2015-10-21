#ifndef DEFAULTCOREMODEL_H
#define DEFAULTCOREMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QVariantList>

namespace Library {

    class DefaultCoreModel : public QAbstractTableModel
    {
        Q_OBJECT
    public:
        explicit DefaultCoreModel( QObject *parent = 0 );

        enum DefaultCoreRoles {
            SystemRole = Qt::UserRole + 1,
            CoresRole,
        };

        QModelIndex index(int row, int column, const QModelIndex &parent) const override {
            return hasIndex( row, column, parent ) ? createIndex( row, column, nullptr ) : QModelIndex();
        }

        int rowCount( const QModelIndex &parent) const override;

        int columnCount(const QModelIndex &parent) const override;

        QVariant data(const QModelIndex &index, int role) const override;

        QHash<int, QByteArray> roleNames() const;

    signals:

    public slots:
        void save( const QString system, const QString core );
        bool coreExists( QString defaultCore );

    protected:
        QHash<int, QByteArray> mRoleNames;
        QVariantList systemList;
        QMap<QVariant, QVariantList> coreListMap;
    };
    //  list< map< int, list >>:
    //      item[int] == "Super Nintendo": ( return QVariant data(); )
    //

}

#endif // DEFAULTCOREMODEL_H
