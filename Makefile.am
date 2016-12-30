@LIBCXX_AM@

SUBDIRS= po po includes/x/w

configure: libtool

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

STATICLINKFLAGS=-Wl,-lcourier-unicode -Wl,-lpthread @LIBCXXW_LIBS@ -pthread

lib_LTLIBRARIES=libcxxw.la
bin_PROGRAMS=cxxw

CPPFLAGS += -DPKGDATADIR='"$(pkgdatadir)"'

libcxxw_la_LIBADD=-lcxx
libcxxw_la_LDFLAGS=$(STATICLINKFLAGS)

libcxxw_la_SOURCES=\
	connection.C                                \
	connection.H                                \
	connection_info.C			    \
	connection_info.H			    \
	connection_thread.C			    \
	connection_thread.H			    \
	connection_threadevent.C		    \
	connection_threadrun.C			    \
	ewmh.C					    \
	ewmh.H					    \
	generic_window.C			    \
	generic_window.H			    \
	generic_window_handler.C		    \
	generic_window_handler.H		    \
	generic_window_impl.C			    \
	pictformat.C				    \
	pictformat.H				    \
	rectangle.C				    \
	render.C				    \
	render.H				    \
	rgb.C					    \
	rgb.H					    \
	screen.C				    \
	screen.H				    \
	screen_depthinfo.C			    \
	screen_depthinfo.H			    \
	window_handler.C			    \
	window_handler.H			    \
	xid_t.H					    \
        #

TESTLINKTYPE=$(if $(STATIC),-static $(STATICLINKFLAGS))

cxxw_SOURCES=cxxw.C
cxxw_LDADD=libcxxw.la
cxxw_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,cxxwoptions.H,cxxwoptions.xml)

EXTRA_DIST = config.rpath m4/ChangeLog  COPYING config.rpath m4/ChangeLog packaging/fedora/libcxxw.spec \
	po/Makevars.in $(man_MANS)


if HAVE_DOCS

docs: html/index.html html/book.css html/.htaccess html/frame.js html/icon.gif

DOCS_SRC = $(shell find includes -name '*.H' -print)

html/book.css: html/index.html docbook/book.css
	$(INSTALL) -m 0644 $(srcdir)/docbook/book.css html/book.css

html/.htaccess: html/index.html docbook/.htaccess
	$(INSTALL) -m 0644 $(srcdir)/docbook/.htaccess html/.htaccess

html/frame.js: html/index.html docbook/frame.js
	$(INSTALL) -m 0644 $(srcdir)/docbook/frame.js html/frame.js

html/icon.gif: html/index.html docbook/icon.gif
	$(INSTALL) -m 0644 $(srcdir)/docbook/icon.gif html/icon.gif

html/index.html: html/ref/index.html $(wildcard docbook/*.xml) docbook/xmllinks.xsl
	mkdir -p docbook
	xsltproc $(srcdir)/docbook/xmllinks.xsl xml/index.xml >docbook/entities
	rm -rf html.tmp
	mkdir -p html.tmp
	xsltproc --nonet -o html.tmp/ --xinclude \
		--stringparam use.id.as.filename 1 \
		--stringparam html.stylesheet book.css \
		--stringparam root.filename 'toc' \
		--stringparam generate.id.attributes 1 \
		--param local.l10n.xml 'document("'`pwd`/docbook/l10.xml'")' \
		$(srcdir)/docbook/docbook.xsl $(srcdir)/docbook/book.xml
	-rm -f html/* 2>/dev/null >/dev/null || :
	mv html.tmp/* html
	rm -rf html.tmp
	rm -rf html/examples
	find examples -depth ! -name autom4te.cache \( -type d -o -name '*.[CH]' -o -name '*.xml' -o -name '*.jpg' -o -name Makefile.am -o -name configure.ac \) -print | cpio -o --quiet | (cd html; cpio -i --quiet --make-directories)
	true; # echo 'ForceType text/plain' >html/examples/.htaccess
	touch INSTALL.tmp # links -dump -no-numbering -no-references html/INSTALL.html >INSTALL.tmp
	mv -f INSTALL.tmp INSTALL

html/ref/index.html: $(DOCS_SRC) Doxyfile
	rm -rf html/ref
	mkdir -p html/ref
	doxygen; c=$$?;	perl -p -i -e 's/INSERT_LIBX_NAMESPACE/x/g' html/ref/*.html; exit $$c
endif

.PHONY: rpm

rpm: dist
	mkdir -p rpm
	rm -rf rpm/SRPMS/*
	rm -rf rpm/RPMS/*
	rm -rf rpm/BUILD/*
	rm -rf rpm/BUILDROOT/*
	rpmbuild -ta --clean \
		--define "_topdir `pwd`/rpm" \
		--define '_rpmdir %{_topdir}/RPMS' \
		--define '_srcrpmdir %{_topdir}/SRPMS' \
		--define '_sourcedir %{_topdir}/SOURCES' \
		--define '_specdir %{_topdir}/SPECS' \
		--define '_builddir %{_topdir}/BUILD' \
		--define '_build_name_fmt %%{ARCH}/%%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm' \
		--define '_tmppath %{_var}/tmp' \
		--define '__spec_prep_pre %{___build_pre}' \
		--define "extrarelease .`date '+%Y%m%d%H%M%S'`" @PACKAGE@-@VERSION@.tar.bz2
	sh $$HOME/repos.sh

ACLOCAL_AMFLAGS = -I m4
