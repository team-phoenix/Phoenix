#pragma once

#include <QVector>
#include <QMetaType>
#include <string>

struct retro_variable;

// Container class for a libretro core variable
class LibretroVariable {
    public:
        LibretroVariable() = default;
        LibretroVariable( const retro_variable *var );
        LibretroVariable( const QByteArray &key );
        LibretroVariable( const QByteArray &&key );

        virtual ~LibretroVariable() = default;

        const QByteArray &key() const;

        const QByteArray &value( const QByteArray &default_ ) const;
        const QByteArray &value() const;

        const QByteArray &description() const;

        const QVector<QByteArray> &choices() const;

        bool setValue( const QByteArray &value );
        bool setValue( QByteArray &&t_value );

        bool isValid() const;

        bool operator ==( const LibretroVariable &t_var1 );

    private:
        // use std::strings instead of QStrings, since the later store data as 16bit chars
        // while cores probably use ASCII/utf-8 internally..
        QByteArray m_key;
        QByteArray m_value; // XXX: value should not be modified from the UI while a retro_run() call happens
        QByteArray m_description;
        QVector<QByteArray> m_choices;

};

QDebug operator<<( QDebug debug, const LibretroVariable &var );

Q_DECLARE_METATYPE( LibretroVariable )
