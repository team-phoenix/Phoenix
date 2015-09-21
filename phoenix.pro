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
