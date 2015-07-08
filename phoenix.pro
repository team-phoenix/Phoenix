TEMPLATE = subdirs

SUBDIRS += phoenix-backend
SUBDIRS += frontend

phoenix-backend.file = phoenix-backend/phoenix-backend.pro
frontend.file = frontend/frontend.pro

CONFIG += ordered

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(common.pri)
