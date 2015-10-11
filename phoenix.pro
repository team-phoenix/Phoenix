TEMPLATE = subdirs

# Externals
SUBDIRS += externals/quazip/quazip

# Our stuff
SUBDIRS += backend
SUBDIRS += frontend

# Ensure that frontend is built last
frontend.depends = backend externals/quazip/quazip

# Make portable target available at the topmost Makefile
portable.CONFIG += recursive
portable.recurse = $$SUBDIRS
portable.recurse_target = portable
QMAKE_EXTRA_TARGETS += portable
