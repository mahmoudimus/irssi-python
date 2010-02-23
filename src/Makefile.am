moduledir = $(libdir)/irssi/modules
wrappersdir = $(datadir)/irssi/scripts
module_LTLIBRARIES = libpython.la

libpython_la_DEPENDENCIES = objects/libobjects.la
libpython_la_LIBADD = $(PYTHON_LDFLAGS) objects/libobjects.la
libpython_la_LDFLAGS = -avoid-version

INCLUDES = $(IRSSI_PYTHON_INCLUDES) \
		   -I$(top_srcdir)/src/objects \
		   -DSCRIPTDIR=\""$(datadir)/irssi/scripts"\"

libpython_la_SOURCES = \
	pycore.c \
	pyutils.c \
	pymodule.c \
	pyloader.c \
	pysignals.c\
	pysource.c \
	pythemes.c \
	pystatusbar.c \
	pyconstants.c

noinst_HEADERS = \
	pyconstants.h \
	pycore.h \
	pyirssi.h \
	pyirssi_irc.h \
	pyloader.h \
	pymodule.h \
	pysigmap.h \
	pysignals.h \
	pysource.h \
	pystatusbar.h \
	pythemes.h \
	pyutils.h

wrappers_DATA = irssi.py irssi_startup.py
EXTRA_DIST = $(wrappers_DATA)

SUBDIRS = objects

signalmap:
	awk -f sig2code.awk $(IRSSI_DIST)/docs/signals.txt > pysigmap.h

constants:
	awk -f constants.awk constants.txt > pyconstants.c
