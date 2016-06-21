#pragma once

#include <QObject>

#include "videooutput.h"
#include "node.h"

// A wrapper for VideoOutput that enables it to exist as a node
// Necessary as VideoOutput, being a QQuickItem, cannot inherrit Node (not allowed in Qt)
// TODO: Safety checks?
class VideoOutputNode : public Node {
        Q_OBJECT
        Q_PROPERTY( VideoOutput *videoOutput MEMBER videoOutput NOTIFY videoOutputChanged )

    public:
        explicit VideoOutputNode( Node *parent = nullptr );

    signals:
        void videoOutputChanged();

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;
        void dataIn( DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) override;

    private:
        VideoOutput *videoOutput{ nullptr };
};
