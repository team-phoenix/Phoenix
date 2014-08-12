

#include <QSqlQuery>

#include "gamelibrarymodel.h"
#include "logging.h"


GameLibraryModel::GameLibraryModel(QObject *parent)
    : QSqlTableModel(parent)
{
    role_names = QSqlTableModel::roleNames();
    role_names.insert(TitleRole, "title");
    role_names.insert(ConsoleRole, "console");
    role_names.insert(TimePlayedRole, "timePlayed");
    role_names.insert(ArtworkRole, "artwork");

    base_query = "SELECT title, console, time_played, artwork FROM games";
    sort_column = 0;
    sort_order = static_cast<Qt::SortOrder>(-1); // default = no sort
    updateQuery();

    m_file_count = 0;
    m_progress = 0;

}

QVariant GameLibraryModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlTableModel::data(index, role);
    if (role < Qt::UserRole) {
        return value;
    } else {
        int columnIdx = (role - Qt::UserRole - 1);
        return QSqlTableModel::data(this->index(index.row(), columnIdx), Qt::DisplayRole);
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
        q_str.append(" WHERE " + category + " LIKE ?");

    if (sort_order != -1) {
        q_str.append(" ORDER BY ");
        q_str.append(role_names[Qt::UserRole + sort_column + 1]);
        q_str.append(sort_order == 0 ? " ASC" : " DESC");
    }

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

void GameLibraryModel::setFilter(QString new_terms, QString new_category)
{
    if (search_terms == new_terms)
        return;
    category = new_category;
    search_terms = new_terms;
    updateQuery();
}

void GameLibraryModel::addFilters(QStringList &filter_list)
{
    filter_list << "n64"
                << "z64"
                << "nes"
                << "gba"
                << "gb"
                << "gbc"
                << "cue"
                << "sfc"
                << "srm";
}

void GameLibraryModel::scanFolder(QString path)
{
    QDirIterator dir_iter(path, QDirIterator::Subdirectories);

    QVector<QFileInfo> files;

    QStringList filter;
    addFilters(filter);

    while (dir_iter.hasNext()) {
        dir_iter.next();
        QFileInfo info(dir_iter.fileInfo());
        if (info.isFile()) {
            for (int i=0; i < filter.size(); ++i) {
                if (info.suffix() == filter.at(i)) {
                    files.append(info);
                    break;
                }
            }
        }
    }

    m_file_count = files.size();
    qDebug() << m_file_count;

    for (int i=0; i < m_file_count; ++i) {
        qreal progress = i / m_file_count;
        emit progressChanged(progress);

        QFileInfo file_info = files.at(i);
        qDebug(phxLibrary) << file_info.baseName();
    }

}

