

#include <gamelibrarymodel.h>


GameLibraryModel::GameLibraryModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    setQuery("SELECT title, console, time_played, artwork FROM games", dbm.handle());
}

QVariant GameLibraryModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    if (role < Qt::UserRole) {
        return value;
    } else {
        int columnIdx = (role - Qt::UserRole - 1);
        return QSqlQueryModel::data(this->index(index.row(), columnIdx), Qt::DisplayRole);
    }
}

QHash<int, QByteArray> GameLibraryModel::roleNames() const
{
    auto roles = QSqlQueryModel::roleNames();
    roles.insert(TitleRole, "title");
    roles.insert(ConsoleRole, "console");
    roles.insert(TimePlayedRole, "timePlayed");
    roles.insert(ArtworkRole, "artwork");
    return roles;
}
