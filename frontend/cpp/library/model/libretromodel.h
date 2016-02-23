#ifndef LIBRETROMODEL_H
#define LIBRETROMODEL_H

#include <QHash>

namespace Library {

    class LibretroModel
    {
    public:

        using HashMap = QHash<QString, QHash<QString,QString>> ;
        LibretroModel();

        const HashMap &model() const
        {
            return mHashMap;
        }


    private:
        QHash<QString, QHash<QString,QString>> mHashMap;
    };

}


#endif // LIBRETROMODEL_H
