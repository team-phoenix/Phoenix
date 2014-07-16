
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

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

public slots:
    void setFilter(QString search_terms_);
    virtual void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;

private:
    LibraryDbManager dbm;
    QString base_query;
    QString search_terms;
    int sort_column;
    Qt::SortOrder sort_order;
    QHash<int, QByteArray> role_names;

    void updateQuery();
};

#endif
