## Process this file with automake to produce Makefile.in

## Created by Anjuta


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(CD_TRACK_READER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = cd_track_reader

cd_track_reader_SOURCES = \
	callbacks.c \
	callbacks.h \
	main.c \
	cd_info.c \
	cd_reader.c \
	isoextract.c

cd_track_reader_LDFLAGS = \
	-lisofs -Wl,--export-dynamic

cd_track_reader_LDADD = $(CD_TRACK_READER_LIBS)


