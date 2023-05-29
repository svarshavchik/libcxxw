#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([libcxxw],[0.32.0],[libcxx-users@lists.libcxx.org])
AC_CONFIG_SRCDIR([connection.C])
AC_CONFIG_HEADERS([libcxxw_config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability])
AM_GNU_GETTEXT([external])
AM_SILENT_RULES([yes])

LT_INIT

# Checks for programs.
AC_ARG_WITH(pkgconfigdir,[AS_HELP_STRING([--with-pkgconfigdir=dir],[Specify where pkgconfig configuration file gets installed])],
	[PKGCONFIGDIR="$withval"],
	[
for d in /usr/lib64/pkgconfig /usr/lib/pkgconfig /usr/local/libdata/pkgconfig
do
	if test -d "$d"
	then
		PKGCONFIGDIR="$d"
		break
	fi
done
])

if test "$PKGCONFIGDIR" = ""
then
	AC_MSG_ERROR([Unable to find pkgconfig installation directory (use --with-pkgconfigdir to set manually)])
fi
AC_SUBST(PKGCONFIGDIR)

AC_PROG_CXX
LIBCXX_INIT
CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden"
export CXXFLAGS
LDFLAGS="$LDFLAGS -z relro -z now"
export LDFLAGS
AC_PATH_PROG(XSLTPROC,xsltproc,"none")
if test "$XSLTPROC" = "none"
then
	AC_MSG_ERROR([Cannot find xsltproc, please install libxslt])
fi
AC_SUBST(XSLTPROC)
AC_PATH_PROG(MSGFMT,msgfmt,"none")
if test "$MSGFMT" = "none"
then
	AC_MSG_ERROR([Cannot find msgfmt, please install gettext])
fi

# VERSION_INFO is:
#
#   libtool's -version-info
#
# ABIVER is, basically, the major version, the ABI version.
#
#   $libdir/libcxxw-$ABIVER
#
#   $includedir/libcxxw-$ABIVER
#
#   $datadir/libcxxw-$ABIVER
#
#   $pkgdatadir/libcxxw-$ABIVER
#
# Our package $VERSION is:
#
#   $libdir/pkgconfig/libcxxw-$VERSION.pc
#   $datadir/aclocal/libcxxw-$VERSION.m4
#
ABIVER=0.30
AC_SUBST(ABIVER)

VERSION_INFO=30:0:0
AC_SUBST(VERSION_INFO)

# Checks for libraries.

for pkg in xcb xcb-proto xcb-keysyms xcb-ewmh xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig
do
	if pkg-config --cflags $pkg >/dev/null 2>/dev/null
	then
	    :
	else
	    AC_MSG_ERROR([Cannot find $pkg development libraries, please install the $pkg devel package])
	fi
done

CFLAGS="$CFLAGS `pkg-config --cflags xcb-keysyms xcb-ewmh xcb-sync xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig libpng`"
CXXFLAGS="$CXXFLAGS `pkg-config --cflags xcb-keysyms xcb-ewmh xcb-sync xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig libpng`"

all_libs="`pkg-config --libs xcb-keysyms xcb-ewmh xcb-sync xcb-icccm xcb-image xcb-renderutil freetype2 fontconfig libpng`"

LIBCXXW_LIBS=""

for l in $all_libs
do
	LIBCXXW_LIBS="$LIBCXXW_LIBS -Wl,$l"
done

AC_SUBST([LIBCXXW_LIBS])

AX_COURIER_UNICODE_VERSION(2.2.1)

XGETTEXT_EXTRA_OPTIONS="--keyword=_txt --keyword=_txtw --keyword=_txtn:1,2 --keyword=_txtnw:1,2"
AC_SUBST([XGETTEXT_EXTRA_OPTIONS])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_ARG_WITH(testprograms, [AS_HELP_STRING([--with-testprograms],[Build internal test programs.])],[testprograms="$withval"])

AC_ARG_WITH(localthemedir, [AS_HELP_STRING([--with-localthemedir],[Compile with -DTHEMEDIR=./themes -DCREATORDIR=./creator -DCOMPILERDIR=. (for testing purposes).])],[localthemedir="$withval"])

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
	libcxxw.m4
	libcxxw.mk
	extract-theme-text.xsl
	po/Makefile.in
	includes/x/w/Makefile
	includes/x/w/deps/Makefile
	includes/x/w/impl/Makefile
	includes/x/w/impl/focus/Makefile
	includes/x/w/impl/fonts/Makefile
	includes/x/w/impl/popup/Makefile
	includes/x/w/impl/richtext/Makefile
	includes/x/w/impl/selection/Makefile
	includes/x/w/metrics/Makefile
	includes/x/w/richtext/Makefile
	packaging/fedora/libcxxw.spec
	])

AM_CONDITIONAL([LOCALTHEMEDIR], [test "$localthemedir" = "yes" ])
pwdthemes="`pwd`/themes"
pwdcreator="`pwd`/creator"
AC_SUBST(pwdthemes)
AC_SUBST(pwdcreator)

AC_SUBST([LIBTOOL_DEPS])
AC_OUTPUT
