
#ifndef GAMELIBRARYMODEL_H
#define GAMELIBRARYMODEL_H

#include <QAbstractTableModel>

class GameLibraryModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    GameLibraryModel(QObject *parent = 0) : QAbstractTableModel(parent) {}
    virtual ~GameLibraryModel() {}

    enum GameRoles {
        TitleRole = Qt::UserRole + 1,
        ConsoleRole,
        PlayTimeRole,
        ArtworkRole,
    }; 

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
};

#endif
