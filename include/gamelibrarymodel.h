
#ifndef GAMELIBRARYMODEL_H
#define GAMELIBRARYMODEL_H

#include <QSqlTableModel>
#include <QDirIterator>
#include <QStringList>
#include "librarydbmanager.h"


class GameLibraryModel: public QSqlTableModel
{
    Q_OBJECT

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

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

    qreal progress() const {
        return m_progress;
    }


public slots:
    void setFilter(QString search_terms_, QString new_category);
    virtual void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;
    void scanFolder(QString path);

signals:
    void progressChanged(qreal);

private:
    LibraryDbManager dbm;
    QString base_query;
    QString search_terms;
    QString category;
    int sort_column;
    Qt::SortOrder sort_order;
    QHash<int, QByteArray> role_names;

    int m_file_count;
    qreal m_progress;

    void addFilters(QStringList &filter_list);

    void updateQuery();
};

#endif
