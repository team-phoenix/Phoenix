#include "libretrovariablemodel.h"
#include "libretrovariableforwarder.h"
#include "node.h"

#include <QDebug>

LibretroVariableModel::LibretroVariableModel( QObject *parent )
    : QAbstractTableModel( parent ),
      roles {
    { Role::Key, QByteArrayLiteral( "key" ) },
    { Role::Choices, QByteArrayLiteral( "choices" ) },
    { Role::Description, QByteArrayLiteral( "description" ) },
} {

}

// Public

QHash<int, QByteArray> LibretroVariableModel::roleNames() const {
    return roles;
}

int LibretroVariableModel::rowCount( const QModelIndex & ) const {
    return values.size();
}

int LibretroVariableModel::columnCount( const QModelIndex & ) const {
    return roles.size();
}

QVariant LibretroVariableModel::data( const QModelIndex &index, int role ) const {
    if( index.isValid() && index.row() < values.size() ) {
        switch( role ) {
            case Role::Key: {
                QString key = values.keys()[ index.row() ];
                return key;
            }

            case Role::Choices: {
                QString key = values.keys()[ index.row() ];
                return values[ key ];
            }

            case Role::Description: {
                QString key = values.keys()[ index.row() ];
                return descriptions[ key ];
            }

            default:
                break;
        }
    }

    return QVariant();
}

void LibretroVariableModel::setForwarder( LibretroVariableForwarder *forwarder ) {
    this->forwarder = forwarder;
    connect( this, &LibretroVariableModel::setVariable, forwarder, &LibretroVariableForwarder::setVariable );
    connect( forwarder, &LibretroVariableForwarder::insertVariable, this, &LibretroVariableModel::insertVariable );
    connect( forwarder, &LibretroVariableForwarder::clearVariables, this, &LibretroVariableModel::clearVariables );
}

void LibretroVariableModel::clearVariables() {
    beginResetModel();
    this->values.clear();
    this->currentValues.clear();
    this->descriptions.clear();
    endResetModel();
}

void LibretroVariableModel::insertVariable( QString key, QStringList values, QString currentValue, QString description ) {
    beginResetModel();
    this->values[ key ] = values;
    this->currentValues[ key ] = currentValue;
    this->descriptions[ key ] = description;
    endResetModel();
}

void LibretroVariableModel::updateVariable( QString key, QString value ) {
    if( this->currentValues[ key ] != value ) {
        emit setVariable( key, value );
    }
}
