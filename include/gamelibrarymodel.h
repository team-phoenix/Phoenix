
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
        ConsoleRole,
        TimePlayedRole,
        ArtworkRole,
    };

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

public slots:
    void setFilter(QString search_terms_, QString new_category);
    virtual void sort(int column, Qt::SortOrder order) override;

private:
    LibraryDbManager *dbm;
    QString base_query;
    QString search_terms;
    QString category;
    QString m_label;
    int sort_column;
    Qt::SortOrder sort_order;
    QHash<int, QByteArray> role_names;

    void updateQuery();

};



#endif
