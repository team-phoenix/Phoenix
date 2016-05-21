TEMPLATE = subdirs

SUBDIRS += quazip
SUBDIRS += backend
SUBDIRS += frontend

quazip.subdir = externals/quazip/quazip
backend.subdir = backend
frontend.subdir = frontend

# We'll always be 64-bit
CONFIG += x86_64

# Ensure that frontend is built last
frontend.depends = quazip backend
backend.depends = quazip

# Make portable target available at the topmost Makefile
portable.CONFIG += recursive
portable.recurse = $$SUBDIRS
portable.recurse_target = portable
QMAKE_EXTRA_TARGETS += portable
