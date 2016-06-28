#include "backendloader.h"

#include <QDebug>

#include <gst/gst.h>

void backendInit( int argc, char **argv ) {
    const gchar *nano_str;
    guint major, minor, micro, nano;
    gst_init( &argc, &argv );
    gst_version( &major, &minor, &micro, &nano );

    if( nano == 1 ) {
        nano_str = "(CVS)";
    } else if( nano == 2 ) {
        nano_str = "(Prerelease)";
    } else {
        nano_str = "";
    }

    qDebug( "Init Phoenix backend" );
    qDebug( "Using GStreamer %d.%d.%d %s", major, minor, micro, nano_str );
}
