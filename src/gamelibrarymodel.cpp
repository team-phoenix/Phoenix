

#include <gamelibrarymodel.h>


int GameLibraryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 10000;
}

int GameLibraryModel::columnCount(const QModelIndex &parent) const
{
    // "columnCount() should return 0 when the parent is valid."
    return parent.isValid() ? 0 : 3;
}

QVariant GameLibraryModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return QVariant::fromValue(QString("test %1").arg(index.row()));
    case ConsoleRole:
        return QVariant::fromValue(QString("Virtual Boy"));
    case PlayTimeRole:
        return QVariant::fromValue(QString("0 hours 0 minutes 0 seconds"));
    case ArtworkRole:
        return QVariant::fromValue(QString("test"));
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> GameLibraryModel::roleNames() const
{
    auto roles = QAbstractTableModel::roleNames();
    roles.insert(TitleRole, "title");
    roles.insert(ConsoleRole, "console");
    roles.insert(PlayTimeRole, "playTime");
    roles.insert(ArtworkRole, "artwork");
    return roles;
}
