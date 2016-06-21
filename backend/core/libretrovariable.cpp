#include "libretrovariable.h"
#include "libretro.h"

#include <QDebug>
#include <QDebugStateSaver>

QDebug operator<<( QDebug debug, const LibretroVariable &var ) {

    // join a QVector of std::strings. (Really, C++ ?)

    QDebugStateSaver saver( debug );
    Q_UNUSED( saver );

    debug.nospace() << "LibretroVariable{"
                    << var.key() << "=" << var.value( QByteArrayLiteral( "<not set>" ) ) << ","
                    << " description" << var.description() << ","
                    << " choices=" << var.choices()
                    << "}";

    return debug;

}

LibretroVariable::LibretroVariable( const retro_variable *var ) {
    m_key = var->key;

    // "Text before first ';' is description. This ';' must be followed by a space,
    // and followed by a list of possible values split up with '|'."
    QByteArray valdesc( var->value );
    int splitidx = valdesc.indexOf( "; " );

    if( splitidx != -1 ) {
        m_description = valdesc.mid( 0, splitidx );
        m_choices = valdesc.mid( splitidx + 2 ).split( '|' ).toVector();

    } else {
        // unknown value
    }
}

LibretroVariable::LibretroVariable( const QByteArray &key )
    : m_key( key )  {

}

LibretroVariable::LibretroVariable( const QByteArray &&key )
    : LibretroVariable( key ) {
}

const QByteArray &LibretroVariable::key() const {
    return m_key;
}

const QByteArray &LibretroVariable::value( const QByteArray &default_ ) const {
    return m_value.isEmpty() ? default_ : m_value;
}

const QByteArray &LibretroVariable::value() const {
    return value( QByteArrayLiteral( "" ) );
}

const QByteArray &LibretroVariable::description() const {
    return m_description;
}

const QVector<QByteArray> &LibretroVariable::choices() const {
    return m_choices;
}

bool LibretroVariable::setValue( const QByteArray &value ) {
    m_value = value;
    return true;
}

bool LibretroVariable::setValue( QByteArray &&t_value ) {
    return setValue( t_value );
}

bool LibretroVariable::isValid() const {
    return !m_key.isEmpty();
}

bool LibretroVariable::operator ==( const LibretroVariable &t_var1 ) {
    return ( m_key == t_var1.m_key );
}
