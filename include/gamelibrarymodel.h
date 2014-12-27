
#ifndef GAMELIBRARYMODEL_H
#define GAMELIBRARYMODEL_H

#include <QSqlTableModel>

#include "librarydbmanager.h"


class GameLibraryModel: public QSqlTableModel
{
    Q_OBJECT

public:
    GameLibraryModel(LibraryDbManager *dbm, QObject *parent = 0);
    virtual ~GameLibraryModel();

    enum GameRoles {
        TitleRole = Qt::UserRole + 1,
        SystemRole,
        TimePlayedRole,
        ArtworkRole,
        FileNameRole,
        SystemPathRole,
    };

    virtual QHash<int, QByteArray> roleNames() const override;
    virtual bool select() override;
    //bool removeRow(int row, const QModelIndex &parent)
    //{
    //    return QSqlTableModel::removeRow(row, parent);
    //}
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


public slots:

#ifdef Q_OS_MACX
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
    // Member declaration raising the warning.
#endif
    void setFilter(const QString &filter, QVariantList params);
#ifdef Q_OS_MACX
#pragma clang diagnostic pop
#endif

    virtual void sort(int column, Qt::SortOrder order) override
    {
        QSqlTableModel::sort(column, order);
    }
    QVariantMap get(int index);


private:
    LibraryDbManager *dbm;
    QVariantList params;
    QHash<int, QByteArray> role_names;
};



#endif
