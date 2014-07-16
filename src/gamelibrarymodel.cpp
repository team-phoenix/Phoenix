

#include <QSqlQuery>

#include "gamelibrarymodel.h"
#include "logging.h"


GameLibraryModel::GameLibraryModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    base_query = "SELECT title, console, time_played, artwork FROM games";
    setQuery(base_query, dbm.handle());
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

void GameLibraryModel::setFilter(QString new_terms)
{
    if (search_terms == new_terms)
        return;

    if (!new_terms.isEmpty()) {
        QSqlQuery q(base_query + " WHERE title LIKE ?", dbm.handle());
        q.bindValue(0, "%" + new_terms + "%");
        q.exec();
        setQuery(q);
    } else if(new_terms.isEmpty()) {
        setQuery(base_query, dbm.handle());
    }

    search_terms = new_terms;
}
