@LIBCXX_AM@

SUBDIRS= po includes/x/w

configure: libtool

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

STATICLINKFLAGS=-Wl,-lcourier-unicode -Wl,-lpthread @LIBCXXW_LIBS@ -pthread

lib_LTLIBRARIES=libcxxw.la
bin_PROGRAMS=cxxw

CPPFLAGS += -I$(top_srcdir)/includes -I./includes -DPKGDATADIR='"$(pkgdatadir)"'

libcxxw_la_LIBADD=-lcxx
libcxxw_la_LDFLAGS=$(STATICLINKFLAGS)

libcxxw_la_SOURCES=\
	background_color.C			    \
	background_color.H			    \
	background_colorfwd.H			    \
	background_colorobj.H			    \
	batch_queue.C				    \
	batch_queue.H				    \
	batch_queueobj.H			    \
	border_impl.C				    \
	border_impl.H				    \
	border_implfwd.H			    \
	border_implobj.H			    \
	border_info.C				    \
	border_info.H				    \
	border_infofwd.H			    \
	border_infomm.C				    \
	builtin_atoms.C				    \
	builtin_atoms.H				    \
	calculate_borders.H			    \
	canvas.H				    \
	canvas.C				    \
	canvas_impl.C				    \
	catch_exceptions.H			    \
	child_element.C				    \
	child_element.H				    \
	child_elementfwd.H			    \
	child_elementobj.H			    \
	configfile.C				    \
	configfile.H				    \
	connection.C                                \
	connection.H                                \
	connection_info.C			    \
	connection_info.H			    \
	connection_infofwd.H			    \
	connection_thread.C			    \
	connection_thread.H			    \
	connection_threadfwd.H			    \
	connection_threadevent.C		    \
	connection_threadrun.C			    \
	connection_threadrunelement.C		    \
	container.C				    \
	container.H				    \
	container_element_overrides_decl.H	    \
	container_element_overrides_impl.H	    \
	container_impl.C			    \
	current_border_impl.C			    \
	current_border_impl.H			    \
	current_border_implfwd.H		    \
	current_border_implobj.H		    \
	custom_border_cache.C			    \
	custom_border_cache.H			    \
	custom_border_cachefwd.H		    \
	defaulttheme.C				    \
	defaulttheme.H				    \
	defaultthemefwd.H			    \
	defaultthemeobj.H			    \
	drawable.C				    \
	drawable.H				    \
	drawable_impl.C				    \
	draw_infofwd.H				    \
	draw_info.H				    \
	draw_info_cache.H			    \
	draw_info_cachefwd.H			    \
	element.C				    \
	element.H				    \
	elementfwd.H				    \
	element_impl.C				    \
	element_screen.H			    \
	ewmh.C					    \
	ewmh.H					    \
	factory.C				    \
	gc.C					    \
	gc.H					    \
	gc_thread.H				    \
	gc_impl.C				    \
	generic_window.C			    \
	generic_window.H			    \
	generic_window_handler.C		    \
	generic_window_handler.H		    \
	generic_window_impl.C			    \
	grid_element.C				    \
	grid_element.H				    \
	grid_elementobj.H			    \
	gridfactory.C				    \
	gridfactory.H				    \
	gridfactoryfwd.H			    \
	gridfactoryobj.H			    \
	gridlayoutmanager.C			    \
	gridlayoutmanager.H			    \
	gridlayoutmanager_impl.C		    \
	gridlayoutmanager_impl_elements.C	    \
	gridlayoutmanager_impl_elements.H	    \
	impl_connection_thread.H		    \
	layoutmanager.C				    \
	layoutmanager.H				    \
	layoutmanager_impl.C			    \
	main_window.C				    \
	main_window.H				    \
	main_window_handler.C			    \
	main_window_handler.H			    \
	main_window_impl.C			    \
	messages.H				    \
	metrics_axis.C				    \
	metrics_axis.H				    \
	metrics_grid.H				    \
	metrics_grid.C				    \
	metrics_grid_axisrange.C		    \
	metrics_grid_axisrange.H		    \
	metrics_grid_axisrangefwd.H		    \
	metrics_grid_xy.H			    \
	metrics_grid_pos.C			    \
	metrics_grid_pos.H			    \
	metrics_grid_posfwd.H			    \
	metrics_grid_posobj.H			    \
	metrics_horizvert.C			    \
	metrics_horizvert.H			    \
	metrics_horizvertfwd.H			    \
	metrics_horizvertobj.H			    \
	metrics_mmaxis.C			    \
	new_layoutmanager.C			    \
	pictformat.C				    \
	pictformat.H				    \
	picture.C				    \
	picture.H				    \
	picture_impl.C				    \
	pixmap.C				    \
	pixmap.H				    \
	pixmap_impl.C				    \
	rectangle.C				    \
	rectangle.H				    \
	recycled_pixmaps.C			    \
	recycled_pixmaps.H			    \
	recycled_pixmapsfwd.H			    \
	recycled_pixmapsobj.H			    \
	render.C				    \
	render.H				    \
	returned_pointer.H			    \
	rgb.C					    \
	scratch_buffer.C			    \
	scratch_buffer_impl.C			    \
	scratch_buffer.H			    \
	screen.C				    \
	screen.H				    \
	screen_depthinfo.C			    \
	screen_depthinfo.H			    \
	screen_solidcolorpictures.C		    \
	screen_solidcolorpictures.H		    \
	screen_solidcolorpicturesfwd.H		    \
	straight_border.C			    \
	straight_border.H			    \
	straight_borderfwd.H			    \
	straight_borderobj.H			    \
	straight_border_impl.C			    \
	window_handler.C			    \
	window_handler.H			    \
	xid_t.C					    \
	xid_t.H					    \
	xid_t_connection_thread.H		    \
        #

TESTLINKTYPE=$(if $(STATIC),-static $(STATICLINKFLAGS))

cxxw_SOURCES=cxxw.C
cxxw_LDADD=libcxxw.la
cxxw_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,cxxwoptions.H,cxxwoptions.xml)

EXTRA_DIST = config.rpath m4/ChangeLog COPYING \
	pic_constants.xml gc_constants.xml constants.xsl \
	$(man_MANS)

$(call THREADMSGDISPATCHER_GEN,connection_thread.msgs.H, connection_thread.msgs.xml)

TESTPROGRAMS=\
	testcollapse					\
	testderivedaxis					\
	testgrid					\
	#

INTERACTIVETESTPROGRAMS=\
	testmainwindow					\
	#

noinst_PROGRAMS=$(TESTPROGRAMS) $(INTERACTIVETESTPROGRAMS) genconstants

testcollapse_SOURCES=testcollapse.C
testcollapse_LDADD=-lcxx
testcollapse_LDFLAGS=-static

testderivedaxis_SOURCES=testderivedaxis.C
testderivedaxis_LDADD=libcxxw.la
testderivedaxis_LDFLAGS=$(TESTLINKTYPE)

testgrid_SOURCES=testgrid.C
testgrid_LDADD=libcxxw.la
testgrid_LDFLAGS=-static

$(call OPTIONS_GEN,testmainwindowoptions.H,testmainwindowoptions.xml)

testmainwindow_SOURCES=testmainwindow.C
testmainwindow_LDADD=libcxxw.la -lcxx
testmainwindow_LDFLAGS=-static $(STATICLINKFLAGS)

BUILT_SOURCES=includes/x/w/constants_inc.H \
	gc_constants.inc.C pic_constants.inc.C

CLEANFILES = $(shell cd $(srcdir) && echo *.inc.[CH])

includes/x/w/constants_inc.H: genconstants
	./genconstants >$@.tmp
	mv $@.tmp $@

gc_constants.inc.C: gc_constants.xml constants.xsl
	xsltproc $(srcdir)/constants.xsl $(srcdir)/gc_constants.xml >gc_constants.inc.C.tmp
	mv -f gc_constants.inc.C.tmp gc_constants.inc.C

pic_constants.inc.C: pic_constants.xml constants.xsl
	xsltproc $(srcdir)/constants.xsl $(srcdir)/pic_constants.xml >pic_constants.inc.C.tmp
	mv -f pic_constants.inc.C.tmp pic_constants.inc.C

genconstants_SOURCES=genconstants.C

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

# Find the names of all the themes

all_themes=$(shell cd $(srcdir)/themes && echo */. | tr ' ' '\012' | sed 's@/.$$@@')

# Prepend $(pkgdatadir)/themes to each directory in all_themes
all_installed_theme_dirs=

# We will go through all files in all_themes, append each file to
# all_theme_source_files, and prepend $pkgdatadir/themes/name to each file
# and put it into all_installed_theme_files

all_theme_source_files=
all_installed_theme_files=

define passthrough
$(1)
endef

define install_theme_file

$(call passthrough,all_installed_theme_files += $(DESTDIR)$(pkgdatadir)/themes/$(2)/$(3))
$(call passthrough,all_theme_source_files += $(1))

$(DESTDIR)$(pkgdatadir)/themes/$(2)/$(3): $(1) install-data-hook-create-dir
	$(INSTALL_DATA) $(1) $(DESTDIR)$(pkgdatadir)/themes/$(2)/$(3)
endef

define install_theme
$(call passthrough,all_installed_theme_dirs += $(pkgdatadir)/themes/$(1))

$(foreach f,$(wildcard $(srcdir)/themes/$(1)/*.sxg) $(wildcard $(srcdir)/themes/$(1)/*.xml), $(call install_theme_file,$(f),$(1),$(notdir $(f))))
endef

$(foreach d,$(all_themes),$(eval $(call install_theme,$d)))

install-data-hook: $(all_installed_theme_files) $(DESTDIR)$(pkgdatadir)/themes/cxxwtheme.properties

install-data-hook-create-dir:
	$(MKDIR_P) $(foreach d,$(all_installed_theme_dirs),$(DESTDIR)$(d))

$(DESTDIR)$(pkgdatadir)/themes/cxxwtheme.properties: themes/cxxwtheme.properties
	$(INSTALL_DATA) $(srcdir)/themes/cxxwtheme.properties $(DESTDIR)$(pkgdatadir)/themes/cxxwtheme.properties

uninstall-hook:
	rm -f $(all_installed_theme_files)
	rm -f $(DESTDIR)$(pkgdatadir)/themes/cxxwtheme.properties
	-rmdir $(foreach d,$(all_installed_theme_dirs),$(DESTDIR)$(d))
	-rmdir $(DESTDIR)$(pkgdatadir)/themes
	-rmdir $(DESTDIR)$(pkgdatadir)

EXTRA_DIST += $(patsubst $(srcdir)/%,%,$(all_theme_source_files)) \
	themes/cxxwtheme.properties

check-am:
	@for p in $(TESTPROGRAMS); do echo $$p; ./$$p || exit 1; done

test:
	./testmainwindow --state
	./testmainwindow
	./testmainwindow --showhide
	./testmainwindow --usemain
	./testmainwindow --usetheme

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
