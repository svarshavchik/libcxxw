#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libcxxw], [0.1], [mrsam@courier-mta.com])
AC_CONFIG_SRCDIR([connection.C])
AC_CONFIG_HEADERS([libcxxw_config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability])
AM_GNU_GETTEXT([external])
AM_SILENT_RULES([yes])

LT_INIT
# Checks for programs.
AC_PROG_CXX
LIBCXX_INIT

# Checks for libraries.

for pkg in xcb xcb-proto xcb-keysyms xcb-ewmh xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig
do
	if pkg-config --cflags $pkg >/dev/null 2>/dev/null
	then
	    :
	else
	    AC_ERROR([Cannot find $pkg development libraries, please install the $pkg devel package])
	fi
done

CFLAGS="$CFLAGS `pkg-config --cflags xcb-keysyms xcb-ewmh xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig libpng`"
CXXFLAGS="$CXXFLAGS `pkg-config --cflags xcb-keysyms xcb-ewmh xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig libpng`"

all_libs="`pkg-config --libs xcb-keysyms xcb-ewmh xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig libpng`"

LIBCXXW_LIBS=""

for l in $all_libs
do
	LIBCXXW_LIBS="$LIBCXXW_LIBS -Wl,$l"
done

AC_SUBST([LIBCXXW_LIBS])

save_LIBS="$LIBS"
LIBS="-lcourier-unicode"
AC_LINK_IFELSE(
[AC_LANG_PROGRAM([
#include <courier-unicode.h>
],[
	unicode_default_chset();
])],[:],[AC_MSG_ERROR([The Courier Unicode Library 1.2 appears not to be installed. You may need to install a separate development subpackage, in addition to the main package])])
LIBS="$save_LIBS"

XGETTEXT_EXTRA_OPTIONS="--keyword=_txt --keyword=_txtw --keyword=_txtn:1,2 --keyword=_txtnw:1,2 \`cd \$(top_srcdir); echo *.C\`"
AC_SUBST([XGETTEXT_EXTRA_OPTIONS])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_ARG_WITH(testprograms, [Build internal test programs],[testprograms="$withval"])

BUILT_TESTPROGRAMS=""
BUILT_INTERACTIVETESTPROGRAMS=""

if test "$testprograms" = "yes"
then
	BUILT_TESTPROGRAMS='$(TESTPROGRAMS)'
	BUILT_INTERACTIVETESTPROGRAMS='$(INTERACTIVETESTPROGRAMS)'
fi
AC_SUBST(BUILT_TESTPROGRAMS)
AC_SUBST(BUILT_INTERACTIVETESTPROGRAMS)

AM_CONDITIONAL(HAVE_DOCS,[test -d $srcdir/docbook])
AC_CONFIG_FILES([Makefile
	Doxyfile
	po/Makefile.in
	includes/x/w/Makefile
	includes/x/w/deps/Makefile
	includes/x/w/metrics/Makefile
	packaging/fedora/libcxxw.spec
	])

AC_SUBST([LIBTOOL_DEPS])
AC_OUTPUT
