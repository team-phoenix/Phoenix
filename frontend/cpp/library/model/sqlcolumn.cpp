#include "sqlcolumn.h"

SqlColumn::SqlColumn( QObject *parent )
    : QObject(parent)
{

}

QString SqlColumn::name() const
{
    return mName;
}

QString SqlColumn::type() const
{
    return mType;
}

void SqlColumn::setName( const QString name ) {
    mName = name;
}

void SqlColumn::setType( const QString type ) {
    mType = type;
}
