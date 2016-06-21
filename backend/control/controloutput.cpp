#include "controloutput.h"
#include "logging.h"

ControlOutput::ControlOutput( Node *parent ) : Node( parent ) {

}

void ControlOutput::commandIn( Node::Command command, QVariant data, qint64 timeStamp ) {
    emit commandOut( command, data, timeStamp );

    switch( command ) {
        case Command::Stop: {
            qCDebug( phxControlOutput ) << command;
            state = State::Stopped;
            emit stateChanged( state );
            break;
        }

        case Command::Load: {
            qCDebug( phxControlOutput ) << command;
            state = State::Loading;
            emit stateChanged( state );
            break;
        }

        case Command::Play: {
            qCDebug( phxControlOutput ) << command;
            state = State::Playing;
            emit stateChanged( state );
            break;
        }

        case Command::Pause: {
            qCDebug( phxControlOutput ) << command;
            state = State::Paused;
            emit stateChanged( state );
            break;
        }

        case Command::Unload: {
            qCDebug( phxControlOutput ) << command;
            state = State::Unloading;
            emit stateChanged( state );
            break;
        }

        default:
            break;
    }
}
