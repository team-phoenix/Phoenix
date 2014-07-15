
#ifndef GAMELIBRARYMODEL_H
#define GAMELIBRARYMODEL_H

#include <QSqlQueryModel>

#include "librarydbmanager.h"


class GameLibraryModel: public QSqlQueryModel
{
    Q_OBJECT
public:
    GameLibraryModel(QObject *parent = 0);
    virtual ~GameLibraryModel() {}

    enum GameRoles {
        TitleRole = Qt::UserRole + 1,
        ConsoleRole,
        TimePlayedRole,
        ArtworkRole,
    }; 

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    LibraryDbManager dbm;
};

#endif
