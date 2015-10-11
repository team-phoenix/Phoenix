TEMPLATE = subdirs

# Externals
SUBDIRS += externals/quazip/quazip

# Our stuff
SUBDIRS += backend
SUBDIRS += frontend

quazip.subdir = externals/quazip/quazip
backend.subdir = backend
frontend.subdir = frontend

frontend.depends = backend quazip

# Make portable target available at the topmost Makefile
portable.CONFIG += recursive
portable.recurse = $$SUBDIRS
portable.recursive_target = install-portable
QMAKE_EXTRA_TARGETS += portable
