#pragma once

#include "node.h"
#include "libretrovariable.h"

class LibretroVariableForwarder : public Node {
        Q_OBJECT

    public:
        explicit LibretroVariableForwarder( QObject *parent = nullptr );

        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;

    signals:
        void insertVariable( QString key, QStringList values, QString currentValue, QString description );
        void clearVariables();

    public slots:
        void setVariable( QString key, QString value );
};
