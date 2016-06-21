#pragma once

#include <QObject>

#include "libretrocore.h"
#include "node.h"

class LibretroRunner : public Node {
        Q_OBJECT

    public:
        LibretroRunner() = default;

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;
        void dataIn( DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) override;

    private:
        bool connectedToCore { false };
};
