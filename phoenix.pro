TEMPLATE = subdirs

# Externals
SUBDIRS += externals/quazip/quazip

# Our stuff
SUBDIRS += backend
SUBDIRS += frontend

quazip.file = externals/quazip/quazip/quazip.pro
backend.file = backend/backend.pro
frontend.file = frontend/frontend.pro

CONFIG += ordered

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(common.pri)


linux {

    DEST_FOLDER = $$OUT_PWD/share/databases
    SOURCE_FOLDER = $${PWD}/frontend/databases
    OPENVGDB_SOURCE = $$SOURCE_FOLDER/openvgdb.sqlite

    !exists( $$DEST_FOLDER) {
        DIR_CREATED = $$system( mkdir $$DEST_FOLDER )
    }
    CP_SYS_DB = $$system(cp -rf $$SOURCE_FOLDER/systems.db $$DEST_FOLDER/systems.db)
    CP_VG_DB = $$system(cp -rf $$OPENVGDB_SOURCE $$DEST_FOLDER/openvgdb.sqlite)

}

macx {
    depends.files += $${PWD}/frontend/databases/openvgdb.sqlite
    depends.files += $${PWD}/frontend/databases/systems.db
    depends.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += depends
    QMAKE_MAC_SDK = macosx10.11
    ICON = ../phoenix.icns
}

