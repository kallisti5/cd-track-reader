dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(cd_track_reader, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC




dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=cd_track_reader
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL



PKG_CHECK_MODULES(CD_TRACK_READER, [gtk+-2.0 >= 2.8 libglade-2.0 >= 2.6.0    ])
AC_SUBST(CD_TRACK_READER_CFLAGS)
AC_SUBST(CD_TRACK_READER_LIBS)




AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
data/Makefile
data/cd-track-reader.desktop.in
data/icons/Makefile
data/icons/128x128/Makefile
data/icons/128x128/apps/Makefile
])
