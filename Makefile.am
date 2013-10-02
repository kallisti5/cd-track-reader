## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po data

cd_track_readerdocdir = ${prefix}/doc/cd_track_reader
cd_track_readerdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(cd_track_readerdoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done
DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper
