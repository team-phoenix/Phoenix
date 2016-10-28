#include "backendloader.h"

#include <QDebug>

#include <gst/gst.h>

#include "version.h"

static gboolean register_elements( GstPlugin *plugin );

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

    qDebug() << "gst_plugin_register_static():" << gst_plugin_register_static(
        GST_VERSION_MAJOR,
        GST_VERSION_MINOR,
        "libretroelement",
        "A Libretro core. Accepts input and outputs audio, video and haptic feedback.",
        register_elements,
        PHOENIX_VER_STR,
        "GPL",
        "gstlibretroelement.c",
        "Phoenix",
        "https://phoenix.vg/"
    );

    GstElement *libretroElement, *phoenixPipeline;

    phoenixPipeline = gst_pipeline_new( "phoenixPipeline" );
    libretroElement = gst_element_factory_make( "libretroelement", "libretroelement" );

    if( !phoenixPipeline ) {
        qWarning() << "Cannot make Phoenix pipeline!";
    }

    if( !libretroElement ) {
        qWarning() << "Cannot make Libretro element!";
    }

}

static gboolean register_elements( GstPlugin *plugin ) {
    return gst_element_register( plugin, "libretroelement", GST_RANK_NONE, GST_TYPE_LIBRETROELEMENT );
}
