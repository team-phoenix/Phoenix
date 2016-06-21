#pragma once

#include <QObject>

#include "libretrocore.h"
#include "node.h"

class LibretroLoader : public Node {
        Q_OBJECT

    public:
        LibretroLoader() = default;

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;

    private:
        bool connectedToCore { false };
};
