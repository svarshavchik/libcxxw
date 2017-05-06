@LIBCXX_AM@

AUTOMAKE_OPTIONS=subdir-objects

SUBDIRS= po includes/x/w

configure: libtool

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

STATICLINKFLAGS=-Wl,-lcourier-unicode -Wl,-lpthread @LIBCXXW_LIBS@ -pthread

lib_LTLIBRARIES=libcxxw.la
bin_PROGRAMS=cxxw

CPPFLAGS += -I$(top_srcdir)/includes -I./includes -I$(top_srcdir) -I. \
            -DPKGDATADIR='"$(pkgdatadir)"'
CXXFLAGS += -fvisibility-inlines-hidden

libcxxw_la_LIBADD=-lcxx

libcxxw_la_LDFLAGS=$(STATICLINKFLAGS)

libcxxw_la_SOURCES=\
	assert_or_throw.C			    \
	assert_or_throw.H			    \
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
	button.C				    \
	button.H				    \
	button_event.C				    \
	button_impl.C				    \
	corner_border.C				    \
	corner_border.H				    \
	corner_borderfwd.H			    \
	corner_borderobj.H			    \
	corner_border_impl.C			    \
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
	container_element.H			    \
	container_elementfwd.H			    \
	container_impl.H			    \
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
	editor.C				    \
	editor.H				    \
	editorfwd.H				    \
	editorobj.H				    \
	editor_impl.C				    \
	editor_impl.H				    \
	editor_container.C			    \
	editor_container.H			    \
	editor_containerfwd.H			    \
	editor_containerobj.H			    \
	editor_container_impl.C			    \
	editor_container_impl.H			    \
	element.C				    \
	element.H				    \
	element_draw.H				    \
	elementfwd.H				    \
	element_impl.C				    \
	element_screen.H			    \
	ewmh.C					    \
	ewmh.H					    \
	factory.C				    \
	gc.C					    \
	focus/element_focusable.C		    \
	focus/focus.H				    \
	focus/focusable.C			    \
	focus/focusable.H			    \
	focus/focusablefwd.H			    \
	focus/focusable_element.H		    \
	focus/focusable_elementfwd.H		    \
	focus/focusable_impl.C			    \
	focus/focusable_owner.C			    \
	focus/focusframecontainer.C		    \
	focus/focusframecontainer.H		    \
	focus/focusframecontainerfwd.H		    \
	focus/focusframecontainerobj.H		    \
	focus/focusframecontainer_element.H	    \
	focus/focusframecontainer_elementfwd.H	    \
	focus/focusframecontainer_impl.C	    \
	focus/focusframecontainer_impl.H	    \
	focus/focusframelayoutimpl.C		    \
	focus/focusframelayoutimpl.H		    \
	focus/focusframelayoutimplfwd.H		    \
	fonts/composite_text_stream.C		    \
	fonts/composite_text_stream.H		    \
	fonts/composite_text_streamfwd.H	    \
	fonts/current_fontcollection.C		    \
	fonts/current_fontcollection.H		    \
	fonts/current_fontcollectionfwd.H	    \
	fonts/current_fontcollectionobj.H	    \
	fonts/cached_font.C			    \
	fonts/cached_font.H			    \
	fonts/cached_fontfwd.H			    \
	fonts/font.C				    \
	fonts/fontcharset.C			    \
	fonts/fontcharset.H			    \
	fonts/fontcharsetfwd.H			    \
	fonts/fontcharsetobj.H			    \
	fonts/fontcharset_impl.H		    \
	fonts/fontcharset_impl.C		    \
	fonts/fontcollection.C			    \
	fonts/fontcollection.H			    \
	fonts/fontcollectionfwd.H		    \
	fonts/fontcollectionobj.H		    \
	fonts/fontcollection_impl.C		    \
	fonts/fontcollection_impl.H		    \
	fonts/fontconfig.C			    \
	fonts/fontconfig.H			    \
	fonts/fontconfigfwd.H			    \
	fonts/fontconfigobj.H			    \
	fonts/fontconfig_impl.C			    \
	fonts/fontconfig_impl.H			    \
	fonts/fontid_t.H			    \
	fonts/fontid_t_hash.H			    \
	fonts/fontlist.C			    \
	fonts/fontlist.H			    \
	fonts/fontlistfwd.H			    \
	fonts/fontlistobj.H			    \
	fonts/fontlist_impl.C			    \
	fonts/fontlist_impl.H			    \
	fonts/fontobjectset.C			    \
	fonts/fontobjectset.H			    \
	fonts/fontobjectsetfwd.H		    \
	fonts/fontobjectsetobj.H		    \
	fonts/fontobjectset_impl.C		    \
	fonts/fontobjectset_impl.H		    \
	fonts/fontpattern.C			    \
	fonts/fontpattern.H			    \
	fonts/fontpatternfwd.H			    \
	fonts/fontpatternobj.H			    \
	fonts/fontpattern_impl.C		    \
	fonts/fontpattern_impl.H		    \
	fonts/fontsortedlist.C			    \
	fonts/fontsortedlist.H			    \
	fonts/fontsortedlistfwd.H		    \
	fonts/fontsortedlistobj.H		    \
	fonts/fontsortedlist_impl.C		    \
	fonts/fontsortedlist_impl.H		    \
	fonts/freetype.C			    \
	fonts/freetype.H			    \
	fonts/freetypefwd.H			    \
	fonts/freetypeobj.H			    \
	fonts/freetypefont.C			    \
	fonts/freetypefont.H			    \
	fonts/freetypefontfwd.H			    \
	fonts/freetypefontobj.H			    \
	fonts/freetypefont_impl.C		    \
	fonts/freetypefont_impl.H		    \
	fonts/glyphset.C			    \
	fonts/glyphset.H			    \
	fonts/scanline.H			    \
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
	gridfactory_impl.C			    \
	gridlayoutmanager.C			    \
	gridlayoutmanager.H			    \
	gridlayoutmanager_impl.C		    \
	gridlayoutmanager_impl_elements.C	    \
	gridlayoutmanager_impl_elements.H	    \
	hotspot.C				    \
	hotspot.H				    \
	hotspotfwd.H				    \
	hotspot_bgcolor.C			    \
	hotspot_bgcolor.H			    \
	hotspot_bgcolor_element.H		    \
	hotspot_bgcolor_elementfwd.H		    \
	hotspot_bgcolor_impl.C			    \
	hotspot_element.H			    \
	hotspot_elementfwd.H			    \
	hotspot_impl.C				    \
	icon.C					    \
	icon.H					    \
	iconfwd.H				    \
	iconobj.H				    \
	icon_cache.C				    \
	icon_cache.H				    \
	icon_cachefwd.H				    \
	icon_cacheobj.H				    \
	icon_image.C				    \
	icon_image.H				    \
	icon_imagefwd.H				    \
	icon_imageobj.H				    \
	image.C					    \
	image.H					    \
	image_impl.C				    \
	image_button.C				    \
	image_button.H				    \
	image_button_impl.C			    \
	image_button_internal.C			    \
	image_button_internal.H			    \
	image_button_internalfwd.H		    \
	image_button_internalobj.H		    \
	image_button_internal_impl.H		    \
	image_button_internal_impl.C		    \
	impl_connection_thread.H		    \
	input_field.H				    \
	input_field.C				    \
	input_field_impl.C			    \
	input_mask.C				    \
	keysyms.C				    \
	keysyms.H				    \
	key_event.C				    \
	label.H					    \
	label.C					    \
	label_impl.C				    \
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
	new_layoutmanager.H			    \
	nonrecursive_visibility.H		    \
	nonrecursive_visibilityfwd.H		    \
	peephole.H				    \
	peephole.C				    \
	peepholefwd.H				    \
	peepholeobj.H				    \
	peephole_get_element.H			    \
	peephole_impl.C				    \
	peephole_impl.H				    \
	peephole_layoutmanager_impl.C		    \
	peephole_layoutmanager_impl.H		    \
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
	richtext/fragment_list.C		    \
	richtext/fragment_list.H		    \
	richtext/paragraph_list.C		    \
	richtext/paragraph_list.H		    \
	richtext/richtext.C			    \
	richtext/richtext.H			    \
	richtext/richtextobj.H			    \
	richtext/richtext_draw_info.H		    \
	richtext/richtext_draw_infofwd.H	    \
	richtext/richtext_impl.C		    \
	richtext/richtext_impl.H		    \
	richtext/richtextfwd.H			    \
	richtext/richtext_insert.H		    \
	richtext/richtext_insertfwd.H		    \
	richtext/richtextiterator.C		    \
	richtext/richtextiterator.H		    \
	richtext/richtextiteratorfwd.H		    \
	richtext/richtextiteratorobj.H		    \
	richtext/richtextcursorlocation.C	    \
	richtext/richtextcursorlocation.H	    \
	richtext/richtextcursorlocationfwd.H	    \
	richtext/richtextfragment.C		    \
	richtext/richtextfragment.H		    \
	richtext/richtextfragment_render.H	    \
	richtext/richtexthorizinfo.C		    \
	richtext/richtexthorizinfo.H		    \
	richtext/richtextmeta.C			    \
	richtext/richtextmeta.H			    \
	richtext/richtextmetafwd.H		    \
	richtext/richtextmetalink.C		    \
	richtext/richtextmetalink.H		    \
	richtext/richtextmetalinkfwd.H		    \
	richtext/richtextmetalinkobj.H		    \
	richtext/richtextmetalinkcollection.C	    \
	richtext/richtextmetalinkcollection.H	    \
	richtext/richtextmetalinkcollectionfwd.H    \
	richtext/richtextmetalinkcollectionobj.H    \
	richtext/richtextparagraph.C		    \
	richtext/richtextparagraph.H		    \
	richtext/richtextparagraphfwd.H		    \
	richtext/richtextstring.C		    \
	richtext/richtextstring.H		    \
	richtext/richtextstring2.C		    \
	scratch_buffer.C			    \
	scratch_buffer_impl.C			    \
	scratch_buffer.H			    \
	scratch_buffer_draw.C			    \
	scratch_buffer_draw.H			    \
	scratch_buffer_drawfwd.H		    \
	scratch_and_mask_buffer_draw.C		    \
	scratch_and_mask_buffer_draw.H		    \
	scratch_and_mask_buffer_drawfwd.H	    \
	screen.C				    \
	screen.H				    \
	screen_depthinfo.C			    \
	screen_depthinfo.H			    \
	screen_fontcaches.C			    \
	screen_fontcaches.H			    \
	screen_fontcachesfwd.H			    \
	screen_solidcolorpictures.C		    \
	screen_solidcolorpictures.H		    \
	screen_solidcolorpicturesfwd.H		    \
	shortcut.C				    \
	selection/current_selection.C		    \
	selection/current_selection.H		    \
	selection/current_selectionfwd.H	    \
	selection/incremental_selection_updates.C   \
	selection/incremental_selection_updates.H   \
	selection/convert_selection.C		    \
	service_window_handler.C		    \
	service_window_handler.H		    \
	straight_border.C			    \
	straight_border.H			    \
	straight_borderfwd.H			    \
	straight_borderobj.H			    \
	straight_border_impl.C			    \
	sxg/sxg_image.C				    \
	sxg/sxg_image.H				    \
	sxg/sxg_imagefwd.H			    \
	sxg/sxg_imageobj.H			    \
	sxg/sxg_parser.C			    \
	sxg/sxg_parser.H			    \
	sxg/sxg_parserfwd.H			    \
	sxg/sxg_parserobj.H			    \
	sxg/sxg_parser_text.C			    \
	text_param.C				    \
	theme_border_cache.C			    \
	theme_border_cache.H			    \
	theme_border_cachefwd.H			    \
	windowobj.C				    \
	windowobj.H				    \
	window_handler.C			    \
	window_handler.H			    \
	xim/ximencoding.C			    \
	xim/ximencoding.H			    \
	xim/ximrequest.C			    \
	xim/ximrequest.H			    \
	xim/ximrequestfwd.H			    \
	xim/ximclient.C				    \
	xim/ximclient.H				    \
	xim/ximclientfwd.H			    \
	xim/ximclientobj.H			    \
	xim/ximserver.C				    \
	xim/ximserver.H				    \
	xim/ximserverfwd.H			    \
	xim/ximxtransport.C		  	    \
	xim/ximxtransport.H			    \
	xim/ximxtransportfwd.H			    \
	xim/ximxtransport_impl.C		    \
	xim/ximxtransport_impl.H		    \
	xid_t.C					    \
	xid_t.H					    \
	xid_t_connection_thread.H		    \
	xid_t_fwd.H				    \
        #

XIM_AUTOGEN=xim/ximclient.inc.C xim/ximclient2.inc.C xim/ximclient.inc.H

TESTLINKTYPE=$(if $(STATIC),-static $(STATICLINKFLAGS))

cxxw_SOURCES=cxxw.C
cxxw_LDADD=libcxxw.la
cxxw_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,cxxwoptions.H,cxxwoptions.xml)

EXTRA_DIST = config.rpath m4/ChangeLog COPYING \
	pic_constants.xml gc_constants.xml constants.xsl \
	xim/xim.xml xim/xim.xsl \
	packaging/fedora/libcxxw.spec \
        examples/configure.ac \
	examples/Makefile.am \
        $(shell cd $(srcdir) && echo examples/*.C examples/*.H) \
	$(man_MANS)

$(call THREADMSGDISPATCHER_GEN,connection_thread.msgs.H, connection_thread.msgs.xml)

TESTPROGRAMS=\
	testcollapse					\
	testderivedaxis					\
	testfocusable					\
	testgrid					\
	testrichtextstring				\
	testrichtext					\
	testrichtextiterator				\
	testtextparam					\
	#

INTERACTIVETESTPROGRAMS=\
	testbutton					\
	testimagebuttons			        \
	testinputfield					\
	testmainwindow					\
	testlabel					\
	testwordwrappablelabel				\
	#

EXTRA_PROGRAMS=$(TESTPROGRAMS) $(INTERACTIVETESTPROGRAMS)

noinst_PROGRAMS=$(BUILT_TESTPROGRAMS) $(BUILT_INTERACTIVETESTPROGRAMS) genconstants

testbutton_SOURCES=testbutton.C
testbutton_LDADD=libcxxw.la
testbutton_LDFLAGS=-static $(STATICLINKFLAGS)

testimagebuttons_SOURCES=testimagebuttons.C
testimagebuttons_LDADD=libcxxw.la
testimagebuttons_LDFLAGS=-static $(STATICLINKFLAGS)

testinputfield_SOURCES=testinputfield.C
testinputfield_LDADD=libcxxw.la
testinputfield_LDFLAGS=-static $(STATICLINKFLAGS)

testcollapse_SOURCES=testcollapse.C
testcollapse_LDADD=-lcxx
testcollapse_LDFLAGS=-static

testderivedaxis_SOURCES=testderivedaxis.C
testderivedaxis_LDADD=libcxxw.la
testderivedaxis_LDFLAGS=$(TESTLINKTYPE)

testfocusable_SOURCES=testfocusable.C
testfocusable_LDADD=-lcxx

testgrid_SOURCES=testgrid.C
testgrid_LDADD=libcxxw.la
testgrid_LDFLAGS=-static

testlabel_SOURCES=testlabel.C
testlabel_LDADD=libcxxw.la

testwordwrappablelabel_SOURCES=testwordwrappablelabel.C
testwordwrappablelabel_LDADD=libcxxw.la
testwordwrappablelabel_LDFLAGS=-static $(STATICLINKFLAGS)

testrichtext_SOURCES=testrichtext.C
testrichtext_LDADD=libcxxw.la
testrichtext_LDFLAGS=-static $(STATICLINKFLAGS)

testrichtextiterator_SOURCES=testrichtextiterator.C
testrichtextiterator_LDADD=libcxxw.la
testrichtextiterator_LDFLAGS=-static $(STATICLINKFLAGS)

testrichtextstring_SOURCES=testrichtextstring.C
testrichtextstring_LDADD=-lcxx

testtextparam_SOURCES=testtextparam.C
testtextparam_LDADD=libcxxw.la
testtextparam_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testmainwindowoptions.H,testmainwindowoptions.xml)

testmainwindow_SOURCES=testmainwindow.C
testmainwindow_LDADD=libcxxw.la -lcxx
testmainwindow_LDFLAGS=-static $(STATICLINKFLAGS)

BUILT_SOURCES=includes/x/w/constants_inc.H \
	$(XIM_AUTOGEN) \
	gc_constants.inc.C pic_constants.inc.C

CLEANFILES = $(shell cd $(srcdir) && echo *.inc.[CH]) $(XIM_AUTOGEN)

includes/x/w/constants_inc.H: genconstants
	./genconstants >$@.tmp
	mv $@.tmp $@

gc_constants.inc.C: gc_constants.xml constants.xsl
	xsltproc $(srcdir)/constants.xsl $(srcdir)/gc_constants.xml >gc_constants.inc.C.tmp
	mv -f gc_constants.inc.C.tmp gc_constants.inc.C

pic_constants.inc.C: pic_constants.xml constants.xsl
	xsltproc $(srcdir)/constants.xsl $(srcdir)/pic_constants.xml >pic_constants.inc.C.tmp
	mv -f pic_constants.inc.C.tmp pic_constants.inc.C

xim/ximclient.inc.H: xim/xim.xml xim/xim.xsl
	xsltproc --stringparam mode request-prototypes $(srcdir)/xim/xim.xsl $(srcdir)/xim/xim.xml >xim/ximclient.inc.H.tmp
	mv -f xim/ximclient.inc.H.tmp xim/ximclient.inc.H

xim/ximclient.inc.C: xim/xim.xml xim/xim.xsl
	xsltproc --stringparam mode request $(srcdir)/xim/xim.xsl $(srcdir)/xim/xim.xml >xim/ximclient.inc.C.tmp
	mv -f xim/ximclient.inc.C.tmp xim/ximclient.inc.C

xim/ximclient2.inc.C: xim/xim.xml xim/xim.xsl
	xsltproc --stringparam mode received-switch $(srcdir)/xim/xim.xsl $(srcdir)/xim/xim.xml >xim/ximclient2.inc.C.tmp
	mv -f xim/ximclient2.inc.C.tmp xim/ximclient2.inc.C

genconstants_SOURCES=genconstants.C

if HAVE_DOCS

DOCIMAGES=$(patsubst docbook/%,html/%,$(wildcard docbook/*.gif) $(wildcard docbook/*.png))

define install_docimage
html/$(1): html/index.html docbook/$(1)
	$(INSTALL) -m 0644 $(srcdir)/docbook/$(1) html/$(1)
endef

ALL_DOCS=html/book.css html/.htaccess html/frame.js \
      $(DOCIMAGES)

docs: $(ALL_DOCS) html/index.html

DOCS_SRC = $(shell find includes -name '*.H' -print)

$(foreach f,$(patsubst html/%,%,$(ALL_DOCS)),$(eval $(call install_docimage,$(f))))

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

$(DESTDIR)$(pkgdatadir)/themes/cxxwtheme.properties: themes/cxxwtheme.properties install-data-hook-create-dir
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
	@test -n "$(BUILT_TESTPROGRAMS)" || exit 0; for p in $(BUILT_TESTPROGRAMS); do echo $$p; ./$$p --set-property x::w::themes=$(srcdir)/themes || exit 1; done

test:
	./testmainwindow --state
	./testmainwindow --state --nopadding
	./testmainwindow
	./testmainwindow --nopadding
	./testmainwindow --showhide
	./testmainwindow --showhide --nopadding
	./testmainwindow --usemain
	./testmainwindow --usemain --nopadding
	./testmainwindow --usetheme
	./testmainwindow --usetheme --nopadding
	./testmainwindow --themescale
	./testmainwindow --themescale --nopadding
	./testlabel

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
