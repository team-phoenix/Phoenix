

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include "gamelibrarymodel.h"
#include "logging.h"


GameLibraryModel::GameLibraryModel(LibraryDbManager *dbm, QObject *parent)
    : QSqlTableModel(parent, dbm->handle()),
      dbm(dbm)
{
    role_names = QSqlTableModel::roleNames();
    role_names.insert(TitleRole, "title");
    role_names.insert(SystemRole, "system");
    role_names.insert(TimePlayedRole, "time_played");
    role_names.insert(ArtworkRole, "artwork");
    role_names.insert(FileNameRole, "filename");
    role_names.insert(SystemPathRole, "system_path");

    setTable("games");
    select();
}

GameLibraryModel::~GameLibraryModel()
{
}

QVariant GameLibraryModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlTableModel::data(index, role);
    if (role < Qt::UserRole) {
        return value;
    } else {
        if (!role_names.contains(role))
            return value;

        // role name need to be the same as column name.
        int columnIdx = record().indexOf(role_names.value(role));
        return QSqlTableModel::data(this->index(index.row(), columnIdx), Qt::DisplayRole);
    }
}

QHash<int, QByteArray> GameLibraryModel::roleNames() const
{
    return role_names;
}

bool GameLibraryModel::select()
{
    const QString query = selectStatement();
    if (query.isEmpty())
        return false;

    beginResetModel();

//    d->clearCache();

    QSqlQuery qu(database());
    qu.prepare(query);
    for (auto &val : params) {
        qu.addBindValue(val);
    }
    qu.exec();
    setQuery(qu);

    if (!qu.isActive() || lastError().isValid()) {
        setTable(tableName()); // resets the record & index
        endResetModel();
        return false;
    }
    endResetModel();
    return true;
}

void GameLibraryModel::setFilter(const QString &filter, QVariantList _params)
{
    this->params = _params;
    QSqlTableModel::setFilter(filter);
}

QVariantMap GameLibraryModel::get(int inx)
{
    QVariantMap map;
    foreach (int i, roleNames().keys()) {
        map[roleNames().value(i)] = data(index(inx, 0), i);
    }
    return map;
}
