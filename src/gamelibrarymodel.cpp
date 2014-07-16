

#include <QSqlQuery>

#include "gamelibrarymodel.h"
#include "logging.h"


GameLibraryModel::GameLibraryModel(QObject *parent)
    : QSqlQueryModel(parent)
{
    role_names = QSqlQueryModel::roleNames();
    role_names.insert(TitleRole, "title");
    role_names.insert(ConsoleRole, "console");
    role_names.insert(TimePlayedRole, "timePlayed");
    role_names.insert(ArtworkRole, "artwork");

    base_query = "SELECT title, console, time_played, artwork FROM games";
    sort_column = 0;
    sort_order = static_cast<Qt::SortOrder>(-1); // default = no sort
    updateQuery();
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
    return role_names;
}

void GameLibraryModel::updateQuery() {
    QString q_str(base_query);
    QSqlQuery q(dbm.handle());;
    if (!search_terms.isEmpty())
        q_str.append(" WHERE title LIKE ?");

    if (sort_order != -1) {
        q_str.append(" ORDER BY ");
        q_str.append(role_names[Qt::UserRole + sort_column + 1]);
        q_str.append(sort_order == 0 ? " ASC" : " DESC");
    }

    qCDebug(phxLibrary) << q_str;
    q.prepare(q_str);
    if (!search_terms.isEmpty())
        q.bindValue(0, "%" + search_terms + "%");

    q.exec();
    setQuery(q);
}

void GameLibraryModel::sort(int column, Qt::SortOrder order)
{
    if (sort_column == column && sort_order == order)
        return;

    sort_column = column;
    sort_order = order;
    updateQuery();
}

void GameLibraryModel::setFilter(QString new_terms)
{
    if (search_terms == new_terms)
        return;

    search_terms = new_terms;
    updateQuery();
}
