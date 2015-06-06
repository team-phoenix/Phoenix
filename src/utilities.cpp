#include "utilities.h"

#include <QDir>

Utilities::Utilities() {

}

Utilities::~Utilities() {

}

void Utilities::createFolder( const QString &path ) {
    QDir dir( path );

    if( !dir.exists() ) {
        dir.mkpath( "." );
    }
}

