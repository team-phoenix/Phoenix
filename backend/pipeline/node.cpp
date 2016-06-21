#include "node.h"

Node::Node( QObject *parent ) : QObject( parent ) {

}

void Node::commandIn( Command command, QVariant data, qint64 timeStamp ) {
    emit commandOut( command, data, timeStamp );
}

void Node::dataIn( DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) {
    emit dataOut( type, mutex, data, bytes, timeStamp );
}

QList<QMetaObject::Connection> connectNodes( Node *t_parent, Node *t_child, Qt::ConnectionType type ) {
    Q_ASSERT( t_parent != nullptr );
    Q_ASSERT( t_child != nullptr );

    return {
        QObject::connect( t_parent, &Node::dataOut, t_child, &Node::dataIn, type ),
        QObject::connect( t_parent, &Node::commandOut, t_child, &Node::commandIn, type )
    };
}

bool disconnectNodes( Node *t_parent, Node *t_child ) {
    Q_ASSERT( t_parent != nullptr );
    Q_ASSERT( t_child != nullptr );
    return ( QObject::disconnect( t_parent, &Node::dataOut, t_child, &Node::dataIn ) &&
             QObject::disconnect( t_parent, &Node::commandOut, t_child, &Node::commandIn )
           );
}

bool disconnectNodes( Node &t_parent, Node &t_child ) {
    return disconnectNodes( &t_parent, &t_child );
}
