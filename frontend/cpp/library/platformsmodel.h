#ifndef PLATFORMSMODEL_H
#define PLATFORMSMODEL_H

#include "frontendcommon.h"

#include "systemdatabase.h"
#include "logging.h"

namespace Library {

    class PlatformsModel : public QAbstractListModel {
            Q_OBJECT

            Q_PROPERTY( int count READ count NOTIFY countChanged )

        public:

            explicit PlatformsModel( QObject *parent = 0 );

            int count();

            int rowCount( const QModelIndex &parent ) const;

            QVariant data( const QModelIndex &index, int role ) const;

            Qt::ItemFlags flags( const QModelIndex &index ) const;

            bool setData( const QModelIndex &index, const QVariant &value, int role );

        public slots:

            QVariant get( int index ) const;

            void append( QStringList values );

            void clear();

            void remove( int index );

            void move( int index, int index2 );

        signals:
            void countChanged();

        private:
            QStringList mPlatformsList;
            QString mFilePath;
    };

}

#endif // PLATFORMSMODEL_H
