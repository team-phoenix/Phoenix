#include "libretrovariableforwarder.h"

#include <QDebug>

LibretroVariableForwarder::LibretroVariableForwarder( QObject *parent ) : Node( parent ) {

}

void LibretroVariableForwarder::commandIn( Command command, QVariant data, qint64 timeStamp ) {
    emit commandOut( command, data, timeStamp );

    switch( command ) {
        case Command::SetLibretroVariable: {
            LibretroVariable variable = data.value<LibretroVariable>();
            QString key = variable.key();
            QStringList values;

            for( QByteArray array : variable.choices() ) {
                values.append( array );
            }

            QString description = variable.description();
            // TODO: Read saved value
            emit insertVariable( key, values, values[ 0 ], description );
            break;
        }

        case Command::Unload: {
            emit clearVariables();
            break;
        }

        default: {
            break;
        }
    }
}

void LibretroVariableForwarder::setVariable( QString key, QString value ) {
    LibretroVariable variable( key.toLatin1() );
    variable.setValue( value.toLatin1() );
    emit commandOut( Command::SetLibretroVariable, QVariant::fromValue<LibretroVariable>( variable ), nodeCurrentTime() );
}
