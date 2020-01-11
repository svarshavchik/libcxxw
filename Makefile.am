@LIBCXX_AM@
include $(shell pwd)/libcxxw.mk


EXTRA_DIST =

if LOCALTHEMEDIR
CPPFLAGS += -DTHEMEDIR=\"@pwdthemes@\" -DCREATORDIR=\"@pwdcreator@\"
endif

AUTOMAKE_OPTIONS=subdir-objects

SUBDIRS= po includes/x/w

DISTCHECK_CONFIGURE_FLAGS=--with-pkgconfigdir='$$(prefix)/local/pkgconf'

pkgdatainstdir=$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@
pkgdatainst_DATA=extract-theme-text.xsl libcxxw.mk

creatorinstdir=$(pkgdatainstdir)/creator

BUILT_SOURCES =

pkgconfdir=@PKGCONFIGDIR@

pkgconf_DATA=libcxxw-@ABIVER@.@LIBCXX_VERSION@.pc

libcxxw-@ABIVER@.@LIBCXX_VERSION@.pc: Makefile
	echo "prefix=${prefix}" >libcxxw.pc.tmp
	echo "exec_prefix=${exec_prefix}" >>libcxxw.pc.tmp
	echo "datadir=${datadir}" >>libcxxw.pc.tmp
	echo "pkgdatadir=${pkgdatadir}-@ABIVER@-@LIBCXX_VERSION@" >>libcxxw.pc.tmp
	echo "libdir=${libdir}/libcxxw-@ABIVER@-@LIBCXX_VERSION@" >>libcxxw.pc.tmp
	echo "includedir=${includedir}" >>libcxxw.pc.tmp
	echo "" >>libcxxw.pc.tmp
	echo "Name: LibCXX Windows Library" >>libcxxw.pc.tmp
	echo "Description: LibCXX Windows Library" >>libcxxw.pc.tmp
	echo "Version: @VERSION@.@LIBCXX_VERSION@" >>libcxxw.pc.tmp
	echo "Cflags: -I$(includedir)/libcxxw-@ABIVER@-@LIBCXX_VERSION@" >>libcxxw.pc.tmp
	echo 'Libs: -L${libdir}/libcxxw-@ABIVER@-@LIBCXX_VERSION@ -Wl,-rpath -Wl,${libdir}/libcxxw-@ABIVER@-@LIBCXX_VERSION@' >>libcxxw.pc.tmp
	mv -f libcxxw.pc.tmp libcxxw-@ABIVER@.@LIBCXX_VERSION@.pc

aclocaldir=@datadir@/aclocal
aclocal_DATA=libcxxw-@VERSION@-@LIBCXX_VERSION@.m4

libcxxw-@VERSION@-@LIBCXX_VERSION@.m4: libcxxw.m4
	rm -f libcxxw-@VERSION@-@LIBCXX_VERSION@.m4
	ln libcxxw.m4 libcxxw-@VERSION@-@LIBCXX_VERSION@.m4

CLEANFILES=$(aclocal_DATA) $(pkgconf_DATA)

STATICLINKFLAGS=-Wl,-ljpeg -Wl,-lgif -Wl,-lcourier-unicode -Wl,-lpthread @LIBCXXW_LIBS@ -pthread -Wl,-export-dynamic

abilibdir=$(libdir)/libcxxw-@ABIVER@-@LIBCXX_VERSION@
abilib_LTLIBRARIES=libcxxw.la libcxxwcups.la

bin_PROGRAMS=cxxw cxxwtheme cxxwcreator

CPPFLAGS += -I$(top_srcdir)/includes -I./includes -I$(top_srcdir) -I. \
            -DPKGDATADIR='"$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@"'
CXXFLAGS += -fvisibility-inlines-hidden $(if $(PROFILE),-pg)

libcxxw_la_LIBADD=-lcxx

libcxxw_la_LDFLAGS=$(STATICLINKFLAGS) -version-info @VERSION_INFO@

libcxxw_la_SOURCES=\
	activated_in_thread.C			    \
	activated_in_thread.H			    \
	activated_in_threadfwd.H		    \
	activated_in_threadobj.H		    \
	appearance/appearance.C			    \
	appearance/book_appearance.C		    \
	appearance/bookpage_appearance.C	    \
	appearance/button_appearance.C		    \
	appearance/color_picker_appearance.C        \
	appearance/combobox_appearance.C	    \
	appearance/date_input_field_appearance.C    \
	appearance/element_popup_appearance.C       \
	appearance/file_dialog_appearance.C	    \
	appearance/focus_border_appearance.C        \
	appearance/font_picker_appearance.C	    \
	appearance/frame_appearance.C		    \
	appearance/generic_window_appearance.C	    \
	appearance/image_button_appearance.C	    \
	appearance/input_field_appearance.C	    \
	appearance/item_button_appearance.C	    \
	appearance/item_layout_appearance.C	    \
	appearance/list_appearance.C		    \
	appearance/main_window_appearance.C	    \
	appearance/pane_appearance.C		    \
	appearance/pane_layout_appearance.C	    \
	appearance/peephole_appearance.C	    \
	appearance/print_dialog_appearance.C	    \
	appearance/progressbar_appearance.C	    \
	appearance/popup_list_appearance.C	    \
	appearance/scrollbar_appearance.C	    \
	appearance/scrollbar_images_appearance.C    \
	appearance/table_appearance.C		    \
	appearance/tooltip_appearance.C		    \
	appearance/tooltip_border_appearance.C      \
	assert_or_throw.C			    \
	assert_or_throw.H			    \
	background_color.C			    \
	background_color_element.C		    \
	background_color_element_recalculated.H	    \
	background_color_element_recalculatedfwd.H  \
	batch_queue.C				    \
	batch_queue.H				    \
	batch_queueobj.H			    \
	booklayoutmanager/bookgridlayoutmanager.C   \
	booklayoutmanager/bookgridlayoutmanager.H   \
	booklayoutmanager/booklayoutmanager.C       \
	booklayoutmanager/booklayoutmanager_impl.C  \
	booklayoutmanager/booklayoutmanager_impl.H  \
	booklayoutmanager/bookpagefactory_impl.C    \
	booklayoutmanager/pagetab.C		    \
	booklayoutmanager/pagetab.H		    \
	booklayoutmanager/pagetab_impl.C	    \
	booklayoutmanager/pagetab_impl.H	    \
	booklayoutmanager/pagetabfwd.H		    \
	booklayoutmanager/pagetabobj.H		    \
	booklayoutmanager/pagetabgridcontainer_impl.H \
	booklayoutmanager/pagetabgridcontainer_implfwd.H \
	booklayoutmanager/pagetabgridlayoutmanager.C \
	booklayoutmanager/pagetabgridlayoutmanager.H \
	booklayoutmanager/pagetabgridlayoutmanager_impl.C \
	booklayoutmanager/pagetabgridlayoutmanager_impl.H \
	booklayoutmanager/pagetabgridlayoutmanagerobj.H \
	booklayoutmanager/pagetabsingletonlayoutmanager_impl.C \
	booklayoutmanager/pagetabsingletonlayoutmanager_implobj.H \
	border_cache.C				    \
	border_cache.H				    \
	border_cachefwd.H			    \
	border_impl.C				    \
	border_info.C				    \
	border_infomm.C				    \
	bordercontainer_impl.C			    \
	borderlayoutmanager.C			    \
	borderlayoutmanager_impl.C		    \
	builtin_atoms.C				    \
	builtin_atoms.H				    \
	busy.H					    \
	busy.C					    \
	button.C				    \
	button.H				    \
	button_event.C				    \
	button_impl.C				    \
	copy_cut_paste_menu_items.C		    \
	corner_border.C				    \
	corner_border.H				    \
	corner_borderfwd.H			    \
	corner_borderobj.H			    \
	corner_border_impl.C			    \
	calculate_borders.H			    \
	canvas.C				    \
	canvas_impl.C				    \
	capturefactory.C			    \
	capturefactory.H			    \
	capturefactoryfwd.H			    \
	capturefactoryobj.H			    \
	catch_exceptions.H			    \
	child_element.C				    \
	color_picker/color_picker.C		    \
	color_picker/color_picker_alpha_canvas.C    \
	color_picker/color_picker_alpha_canvas.H    \
	color_picker/color_picker_alpha_canvas_impl.C \
	color_picker/color_picker_alpha_canvas_impl.H \
	color_picker/color_picker_alpha_canvasfwd.H \
	color_picker/color_picker_alpha_canvasobj.H \
	color_picker/color_picker_current_canvas.C  \
	color_picker/color_picker_current_canvas.H  \
	color_picker/color_picker_current_canvas_impl.C \
	color_picker/color_picker_current_canvas_impl.H \
	color_picker/color_picker_current_canvasfwd.H \
	color_picker/color_picker_current_canvasobj.H \
	color_picker/color_picker_impl.C	    \
	color_picker/color_picker_impl.H	    \
	color_picker/color_picker_selector.C	    \
	color_picker/color_picker_selector.H	    \
	color_picker/color_picker_selectorfwd.H	    \
	color_picker/color_picker_selectorobj.H	    \
	color_picker/color_picker_selector_impl.C   \
	color_picker/color_picker_selector_impl.H   \
	color_picker/color_picker_square.C	    \
	color_picker/color_picker_square.H	    \
	color_picker/color_picker_squarefwd.H       \
	color_picker/color_picker_squareobj.H       \
	color_picker/color_picker_square_impl.C     \
	color_picker/color_picker_square_impl.H     \
	combobox/custom_comboboxlayoutmanager.C	    \
	combobox/custom_comboboxlayoutmanager.H	    \
	combobox/custom_comboboxlayoutmanager_impl.C \
	combobox/custom_combobox_container.C	    \
	combobox/custom_combobox_container.H	    \
	combobox/custom_combobox_container_impl.C   \
	combobox/custom_combobox_container_impl.H   \
	combobox/custom_combobox_containerfwd.H     \
	combobox/custom_combobox_containerobj.H     \
	combobox/custom_combobox_popup_container.C  \
	combobox/custom_combobox_popup_container.H  \
	combobox/custom_combobox_popup_container_impl.C \
	combobox/custom_combobox_popup_container_impl.H \
	combobox/custom_combobox_popup_containerfwd.H \
	combobox/custom_combobox_popup_containerobj.H \
	combobox/editable_comboboxlayoutmanager.C	    \
	combobox/editable_comboboxlayoutmanager.H	    \
	combobox/editable_comboboxlayoutmanager_impl.C \
	combobox/standard_comboboxlayoutmanager.C	    \
	combobox/standard_comboboxlayoutmanager.H	    \
	combobox/standard_comboboxlayoutmanager_impl.C \
	configfile.C				    \
	configfile.H				    \
	connection.C                                \
	connection.H                                \
	connectionfwd.H				    \
	connection_info.C			    \
	connection_info.H			    \
	connection_infofwd.H			    \
	connection_thread.C			    \
	connection_thread.H			    \
	connection_threadevent.C		    \
	connection_threadrun.C			    \
	connection_threadrunelement.C		    \
	connection_thread_debug.C		    \
	connection_thread_debug.H		    \
	container.C				    \
	container_impl.H			    \
	container_impl.C			    \
	current_border_impl.C			    \
	cursor_pointer.C			    \
	cursor_pointer.H			    \
	cursor_pointerobj.H			    \
	cursor_pointer_cache.C			    \
	cursor_pointer_cache.H			    \
	cursor_pointer_cachefwd.H		    \
	cursor_pointer_cacheobj.H		    \
	cursor_pointer_element.H		    \
	cursor_pointer_elementfwd.H		    \
	date_input_field/date_input_field.C	    \
	date_input_field/date_input_field_filter.H  \
	date_input_field/date_input_field_handler.C \
	date_input_field/date_input_field_handler.H \
	date_input_field/date_input_field_impl.C    \
	date_input_field/date_input_field_impl.H    \
	date_input_field/date_input_field_calendar.C \
	date_input_field/date_input_field_calendar.H \
	date_input_field/date_input_field_calendarfwd.H \
	date_input_field/date_input_field_calendarobj.H \
	defaulttheme.C				    \
	defaulttheme.H				    \
	defaultthemeobj.H			    \
	dialog.H				    \
	dialog.C				    \
	dialog_impl.C				    \
	dialog_impl.H				    \
	dialog_handler.C			    \
	dialog_handler.H			    \
	dim_axis_arg.C				    \
	dirlisting/filedircontents.C		    \
	dirlisting/filedircontents.H		    \
	dirlisting/filedircontentsfwd.H		    \
	dirlisting/filedircontentsobj.H		    \
	dirlisting/filedircontents_impl.C	    \
	dirlisting/filedircontents_impl.H	    \
	dirlisting/filedirlist_manager.C	    \
	dirlisting/filedirlist_manager.H	    \
	dirlisting/filedirlist_managerfwd.H	    \
	dirlisting/filedirlist_managerobj.H	    \
	dirlisting/filedirlist_manager_impl.C	    \
	dirlisting/filedirlist_manager_impl.H	    \
	dirlisting/filedir_file.C		    \
	dirlisting/filedir_file.H		    \
	dirlisting/filedir_filefwd.H		    \
	dirlisting/filedir_fileobj.H		    \
	drag_destination_element.H		    \
	drag_destination_elementfwd.H		    \
	drag_source.C				    \
	drag_source.H				    \
	drag_source_element.H			    \
	drag_source_elementfwd.H		    \
	drop_destination.C			    \
	drawable.C				    \
	drawable.H				    \
	drawable_impl.C				    \
	editor.C				    \
	editor.H				    \
	editorfwd.H				    \
	editorobj.H				    \
	editor_impl.C				    \
	editor_impl.H				    \
	editor_peephole_impl.C			    \
	editor_peephole_impl.H			    \
	element.C				    \
	element_impl.C				    \
	ellipsiscache.C				    \
	ellipsiscache.H				    \
	ellipsiscachefwd.H			    \
	ellipsiscacheobj.H			    \
	ewmh.C					    \
	ewmh.H					    \
	factory.C				    \
	gc.C					    \
	file_dialog/file_dialog.C		    \
	file_dialog/file_dialog_impl.C		    \
	file_dialog/file_dialog_impl.H		    \
	focusable_container.C			    \
	focusable_container_owner.C		    \
	focusable_label.C			    \
	focusable_label_impl.C			    \
	focusable_label_impl.H			    \
	focusable_owner_container.C		    \
	focusable_owner_container.H		    \
	focusable_owner_containerfwd.H		    \
	focusable_owner_containerobj.H		    \
	focus/delayed_input_focus.C		    \
	focus/element_focusable.C		    \
	focus/focusable.C			    \
	focus/focusable_container.C		    \
	focus/focusable_impl.C			    \
	focus/focusable_owner.C			    \
	focus/focusframecontainer_impl.C	    \
	focus/focusframelayoutimpl.C		    \
	focus/focusframelayoutimpl.H		    \
	focus/focusframelayoutimplfwd.H		    \
	focus/label_for.C			    \
	focus/label_for.H			    \
	focus/label_forobj.H			    \
	focus/standard_focusframecontainer_element.C \
	focus/standard_focusframecontainer_element_impl.C \
	font_picker/font_picker.C		    \
	font_picker/font_picker_impl.C		    \
	font_picker/font_picker_impl.H		    \
	font_picker/font_family_group_entry.C	    \
	font_picker/font_family_group_entry.H	    \
	font_picker/font_family_group_entryfwd.H    \
	font_picker/font_family_group_entryobj.H    \
	font_picker/font_picker_preview.C	    \
	font_picker/font_picker_preview.H	    \
	font_picker/font_picker_previewfwd.H	    \
	font_picker/font_picker_previewobj.H	    \
	font_picker/font_picker_preview_impl.C	    \
	font_picker/font_picker_preview_impl.H	    \
	fonts/composite_text_stream.C		    \
	fonts/current_fontcollection.C		    \
	fonts/cached_font.C			    \
	fonts/cached_font.H			    \
	fonts/cached_fontfwd.H			    \
	fonts/font.C				    \
	fonts/fontcharset.C			    \
	fonts/fontcharset.H			    \
	fonts/fontcharsetobj.H			    \
	fonts/fontcharset_impl.H		    \
	fonts/fontcharset_impl.C		    \
	fonts/fontcollection.C			    \
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
	fonts/freetypeerror.C			    \
	fonts/freetypefont.C			    \
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
	grabbed_pointer.C			    \
	grabbed_pointer.H			    \
	grabbed_pointerobj.H			    \
	grid_element.C				    \
	grid_element.H				    \
	grid_elementobj.H			    \
	grid_map_info.C				    \
	grid_map_info.H				    \
	grid_map_infoobj.H			    \
	gridfactory.C				    \
	gridfactory.H				    \
	gridfactory_impl.C			    \
	gridlayoutmanager.C			    \
	gridlayoutmanager.H			    \
	gridlayoutmanager_impl.C		    \
	gridlayoutmanager_impl_elements.C	    \
	gridlayoutmanager_impl_elements.H	    \
	hash.C					    \
	hotspot.C				    \
	hotspot.H				    \
	hotspot_bgcolor.C			    \
	hotspot_bgcolor.H			    \
	hotspot_bgcolor_element.H		    \
	hotspot_bgcolor_elementfwd.H		    \
	hotspot_bgcolor_impl.C			    \
	hotspot_element.H			    \
	hotspot_elementfwd.H			    \
	hotspot_impl.C				    \
	icon.C					    \
	icon_cache.C				    \
	icon_cache.H				    \
	icon_cachefwd.H				    \
	icon_cacheobj.H				    \
	icon_images_set_element.H		    \
	icon_images_set_elementfwd.H		    \
	icon_images_vector.C			    \
	icon_images_vector.H			    \
	icon_images_vector_element.H		    \
	icon_images_vector_elementfwd.H		    \
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
	inherited_visibility_info.H		    \
	input_dialog.C				    \
	input_field/editor_search_impl.C	    \
	input_field/editor_search_impl.H	    \
	input_field/editor_search_implfwd.H	    \
	input_field/editor_search_implobj.H	    \
	input_field/input_field.C		    \
	input_field/input_field.H		    \
	input_field/input_field_impl.C		    \
	input_field/input_field_search.C	    \
	input_field/input_field_search.H	    \
	input_field/input_field_searchfwd.H	    \
	input_field/input_field_searchobj.H	    \
	input_field/input_field_search_popup_handler.C \
	input_field/input_field_search_popup_handler.H \
	input_field/input_field_search_popup_handlerfwd.H \
	input_field/input_field_search_popup_handlerobj.H \
	input_field/input_field_search_thread.C	    \
	input_field/input_field_search_thread.H	    \
	input_field/input_field_search_threadfwd.H  \
	input_field/input_field_search_threadobj.H  \
	input_mask.C				    \
	itemlayoutmanager/itembutton.C		    \
	itemlayoutmanager/itembutton.H		    \
	itemlayoutmanager/itembuttonfwd.H	    \
	itemlayoutmanager/itembuttonobj.H	    \
	itemlayoutmanager/itembutton_impl.C	    \
	itemlayoutmanager/itembutton_impl.H	    \
	itemlayoutmanager/itemlayoutmanager.C	    \
	itemlayoutmanager/itemlayoutmanager_impl.C  \
	itemlayoutmanager/itemlayoutmanager_impl.H  \
	itemlayoutmanager/peepholed_item_container.C \
	itemlayoutmanager/peepholed_item_container.H \
	itemlayoutmanager/peepholed_item_containerfwd.H \
	itemlayoutmanager/peepholed_item_containerobj.H \
	itemlayoutmanager/peepholed_item_container_impl.C \
	itemlayoutmanager/peepholed_item_container_impl.H \
	keysyms.C				    \
	keysyms.H				    \
	key_event.C				    \
	label.C					    \
	label_element.H				    \
	label_elementfwd.H			    \
	layoutmanager.C				    \
	layoutmanager_impl.C			    \
	list.C					    \
	listlayoutmanager/extra_list_row_info.C	    \
	listlayoutmanager/extra_list_row_info.H	    \
	listlayoutmanager/extra_list_row_infofwd.H  \
	listlayoutmanager/extra_list_row_infoobj.H  \
	listlayoutmanager/in_thread_new_cells_info.C \
	listlayoutmanager/in_thread_new_cells_info.H \
	listlayoutmanager/in_thread_new_cells_infofwd.H \
	listlayoutmanager/in_thread_new_cells_infoobj.H \
	listlayoutmanager/list_cell.C		    \
	listlayoutmanager/list_cell.H		    \
	listlayoutmanager/list_cellfwd.H	    \
	listlayoutmanager/list_cellobj.H	    \
	listlayoutmanager/list_cellimage.C	    \
	listlayoutmanager/list_cellimage.H	    \
	listlayoutmanager/list_cellimagefwd.H	    \
	listlayoutmanager/list_cellimageobj.H	    \
	listlayoutmanager/list_cellseparator.C	    \
	listlayoutmanager/list_cellseparator.H	    \
	listlayoutmanager/list_cellseparatorfwd.H   \
	listlayoutmanager/list_cellseparatorobj.H   \
	listlayoutmanager/list_celltext.C	    \
	listlayoutmanager/list_celltext.H	    \
	listlayoutmanager/list_celltextfwd.H	    \
	listlayoutmanager/list_celltextobj.H	    \
	listlayoutmanager/list_element.C	    \
	listlayoutmanager/list_element.H	    \
	listlayoutmanager/list_elementfwd.H	    \
	listlayoutmanager/list_elementobj.H	    \
	listlayoutmanager/list_element_impl.C	    \
	listlayoutmanager/list_element_impl.H	    \
	listlayoutmanager/listcontainer.C	    \
	listlayoutmanager/listcontainer.H	    \
	listlayoutmanager/listcontainerfwd.H	    \
	listlayoutmanager/listcontainerobj.H	    \
	listlayoutmanager/listcontainer_impl.C	    \
	listlayoutmanager/listcontainer_impl.H	    \
	listlayoutmanager/listcontainer_pseudo_impl.C \
	listlayoutmanager/listcontainer_pseudo_impl.H \
	listlayoutmanager/listcontainer_pseudo_implfwd.H \
	listlayoutmanager/listitemhandle.C	    \
	listlayoutmanager/listitemhandle_impl.C	    \
	listlayoutmanager/listitemhandle_impl.H	    \
	listlayoutmanager/listlayoutmanager.C       \
	listlayoutmanager/listlayoutmanager_impl.C  \
	listlayoutmanager/listlayoutmanager_impl.H  \
	listlayoutmanager/listlayoutstyle_impl.H    \
	listlayoutmanager/listlayoutstyle_implfwd.H \
	listlayoutmanager/listlayoutstyle.C         \
	listlayoutmanager/menu_item.H		    \
	main_window.C				    \
	main_window.H				    \
	main_window_handler.C			    \
	main_window_handler.H			    \
	main_window_impl.C			    \
	menu/menu.C				    \
	menu/menu_impl.C			    \
	menu/menu_impl.H			    \
	menu/menubar_container_impl.C		    \
	menu/menubar_container_impl.H		    \
	menu/menubar_container_implfwd.H	    \
	menu/menubar_hotspot_impl.C		    \
	menu/menubar_hotspot_implobj.H		    \
	menu/menubarfactory.C			    \
	menu/menubarlayoutmanager.C		    \
	menu/menubarlayoutmanager_impl.C	    \
	menu/menubarlayoutmanager_impl.H	    \
	menu/menu_popup.H			    \
	menu/menu_popup.C			    \
	messages.C				    \
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
	new_layoutmanager.C			    \
	override_redirect.C			    \
	override_redirect.H			    \
	override_redirectfwd.H			    \
	override_redirectobj.H			    \
	panelayoutmanager/create_pane_info.H	    \
	panelayoutmanager/panecontainer_impl.C	    \
	panelayoutmanager/panecontainer_impl.H	    \
	panelayoutmanager/panefactory.C		    \
	panelayoutmanager/panefactory_impl.C	    \
	panelayoutmanager/panefactory_impl.H	    \
	panelayoutmanager/panelayoutmanager.C	    \
	panelayoutmanager/panelayoutmanager_impl.C  \
	panelayoutmanager/panelayoutmanager_impl.H  \
	panelayoutmanager/pane_peephole.C	    \
	panelayoutmanager/pane_peephole.H	    \
	panelayoutmanager/pane_peepholefwd.H	    \
	panelayoutmanager/pane_peepholeobj.H	    \
	panelayoutmanager/pane_peephole_impl.C	    \
	panelayoutmanager/pane_peephole_impl.H	    \
	panelayoutmanager/pane_peephole_container.C \
	panelayoutmanager/pane_peephole_container.H \
	panelayoutmanager/pane_peephole_containerfwd.H \
	panelayoutmanager/pane_peephole_containerobj.H \
	panelayoutmanager/pane_peephole_container_impl.C \
	panelayoutmanager/pane_peephole_container_impl.H \
	panelayoutmanager/pane_slider.C		    \
	panelayoutmanager/pane_slider.H		    \
	panelayoutmanager/pane_sliderfwd.H	    \
	panelayoutmanager/pane_sliderobj.H	    \
	panelayoutmanager/pane_slider_impl.C	    \
	panelayoutmanager/pane_slider_impl.H	    \
	panelayoutmanager/pane_slider_impl_element.H \
	panelayoutmanager/pane_slider_impl_elementfwd.H \
	panelayoutmanager/pane_slider_focusframe.C  \
	panelayoutmanager/pane_slider_focusframe.H  \
	panelayoutmanager/pane_slider_original_sizes.H \
	peephole/peephole.H			    \
	peephole/peephole.C			    \
	peephole/peepholefwd.H			    \
	peephole/peepholeobj.H			    \
	peephole/peephole_gridlayoutmanager.C	    \
	peephole/peephole_gridlayoutmanagerobj.H    \
	peephole/peephole_impl.C		    \
	peephole/peephole_impl.H		    \
	peephole/peephole_impl_element.H	    \
	peephole/peephole_impl_elementfwd.H	    \
	peephole/peephole_layoutmanager.C	    \
	peephole/peephole_layoutmanager_impl.C	    \
	peephole/peephole_layoutmanager_impl.H	    \
	peephole/peephole_layoutmanager_impl_scrollbars.C \
	peephole/peephole_layoutmanager_impl_scrollbars.H \
	peephole/peephole_toplevel.C		    \
	peephole/peephole_toplevel.H		    \
	peephole/peephole_toplevel_gridlayoutmanager.C	  \
	peephole/peephole_toplevel_gridlayoutmanager.H	  \
	peephole/peephole_toplevel_gridlayoutmanagerfwd.H \
	peephole/peephole_toplevel_gridlayoutmanagerobj.H \
	peephole/peepholed.C			    \
	peephole/peepholed.H			    \
	peephole/peepholedfwd.H			    \
	peephole/peepholedobj.H			    \
	peephole/peepholed_attachedto_container.C   \
	peephole/peepholed_attachedto_container.H   \
	peephole/peepholed_attachedto_container_impl.C \
	peephole/peepholed_attachedto_container_impl.H \
	peephole/peepholed_attachedto_containerfwd.H \
	peephole/peepholed_attachedto_containerobj.H \
	peephole/peepholed_element.H		    \
	peephole/peepholed_elementfwd.H		    \
	peephole/peepholed_fontelement.H	    \
	peephole/peepholed_fontelementfwd.H	    \
	peephole/peepholed_toplevel.C		    \
	peephole/peepholed_toplevel.H		    \
	peephole/peepholed_toplevelfwd.H	    \
	peephole/peepholed_toplevelobj.H	    \
	peephole/peepholed_toplevel_element.H	    \
	peephole/peepholed_toplevel_elementfwd.H    \
	peepholed_toplevel_listcontainer/create_popup.C \
	peepholed_toplevel_listcontainer/create_popup.H \
	peepholed_toplevel_listcontainer/create_popupfwd.H \
	peepholed_toplevel_listcontainer/element.H  \
	peepholed_toplevel_listcontainer/elementfwd.H \
	peepholed_toplevel_listcontainer/handler.C  \
	peepholed_toplevel_listcontainer/handler.H  \
	peepholed_toplevel_listcontainer/handlerfwd.H \
	peepholed_toplevel_listcontainer/handlerobj.H \
	peepholed_toplevel_listcontainer/impl.C	    \
	peepholed_toplevel_listcontainer/impl.H	    \
	peepholed_toplevel_listcontainer/impl_element.H \
	peepholed_toplevel_listcontainer/impl_elementfwd.H \
	peepholed_toplevel_listcontainer/layoutmanager_impl.C \
	peepholed_toplevel_listcontainer/layoutmanager_impl.H \
	peepholed_focusable.C			    \
	peepholed_focusable.H			    \
	peepholed_focusable_impl.H		    \
	peepholed_listcontainer.C		    \
	peepholed_listcontainer.H		    \
	peepholed_listcontainerfwd.H		    \
	peepholed_listcontainerobj.H		    \
	peepholed_toplevel_main_window.C	    \
	peepholed_toplevel_main_window.H	    \
	peepholed_toplevel_main_windowobj.H	    \
	peepholed_toplevel_main_windowfwd.H	    \
	peepholed_toplevel_main_window_impl.C	    \
	peepholed_toplevel_main_window_impl.H	    \
	pictformat.C				    \
	pictformat.H				    \
	picture.C				    \
	picture.H				    \
	picture_impl.C				    \
	pixmap.C				    \
	pixmap.H				    \
	pixmap_impl.C				    \
	pixmap_extractor.C			    \
	pixmap_extractor.H			    \
	pixmap_loader.C				    \
	pixmap_loader.H				    \
	pixmap_with_picture.C			    \
	popup_imagebutton.C			    \
	popup_imagebutton.H			    \
	popup_imagebutton_impl.C		    \
	popup_imagebutton_impl.H		    \
	popup_imagebutton_implfwd.H		    \
	popup/popup.C				    \
	popup/popup.H				    \
	popup/popup_handler.C			    \
	popup/popup_handler.H			    \
	popup/popup_impl.C			    \
	popup/popup_impl.H			    \
	popup/popup_attachedto_element.C	    \
	popup/popup_attachedto_element.H	    \
	popup/popup_attachedto_elementfwd.H	    \
	popup/popup_attachedto_info.C		    \
	popup/popup_attachedto_info.H		    \
	popup/popup_attachedto_infofwd.H	    \
	popup/popup_attachedto_infoobj.H	    \
	popup/popup_showhide_element.H		    \
	popup/popup_showhide_elementfwd.H	    \
	progressbar.C				    \
	progressbar_handler.C			    \
	progressbar_handler.H			    \
	progressbar_impl.C			    \
	progressbar_impl.H			    \
	progressbar_slider.C			    \
	progressbar_slider.H			    \
	progressbar_sliderfwd.H			    \
	radio_group.C				    \
	radio_group.H				    \
	radio_group_impl.C			    \
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
	richtext/richtext_alteration_config.H	    \
	richtext/richtext_alteration_configfwd.H    \
	richtext/richtext_draw_boundaries.C	    \
	richtext/richtext_draw_boundaries.H	    \
	richtext/richtext_draw_info.H		    \
	richtext/richtext_impl.C		    \
	richtext/richtext_impl.H		    \
	richtext/richtext_insert.H		    \
	richtext/richtextiterator.C		    \
	richtext/richtextiterator.H		    \
	richtext/richtextiteratorobj.H		    \
	richtext/richtextcursorlocation.C	    \
	richtext/richtextcursorlocation.H	    \
	richtext/richtextfragment.C		    \
	richtext/richtextfragment.H		    \
	richtext/richtextfragment_render.H	    \
	richtext/richtexthorizinfo.C		    \
	richtext/richtexthorizinfo.H		    \
	richtext/richtextmeta.C			    \
	richtext/richtextparagraph.C		    \
	richtext/richtextparagraph.H		    \
	richtext/richtext_password_info.C	    \
	richtext/richtext_password_info.H	    \
	richtext/richtextstring.C		    \
	richtext/richtextstring2.C		    \
	richtext/text_hotspot.C			    \
	run_as.H				    \
	scratch_buffer.C			    \
	scratch_buffer_impl.C			    \
	scratch_buffer_draw.C			    \
	scratch_and_mask_buffer_draw.C		    \
	screen.C				    \
	screen.H				    \
	screen_depthinfo.C			    \
	screen_depthinfo.H			    \
	screen_fontcaches.C			    \
	screen_fontcaches.H			    \
	screen_fontcachesfwd.H			    \
	screen_picturecache.C			    \
	screen_picturecache.H			    \
	screen_picturecachefwd.H		    \
	screen_positions.C			    \
	screen_positions_impl.C			    \
	screen_positions_impl.H			    \
	screen_update_current_theme.C		    \
	scrollbar/scrollbar.C			    \
	scrollbar/scrollbar_icon_set.H		    \
	scrollbar/scrollbar_impl.C		    \
	scrollbar/scrollbar_impl.H		    \
	scrollbar/scrollbar_metrics.C		    \
	scrollbar/scrollbar_metrics.H		    \
	selection/current_selection.C		    \
	selection/current_selection.H		    \
	selection/current_selectionfwd.H	    \
	selection/current_selection_handler.C	    \
	selection/current_selection_handler.H	    \
	selection/current_selection_handlerobj.H    \
	selection/current_selection_paste_handler.C \
	selection/current_selection_paste_handler.H \
	selection/current_selection_paste_handlerfwd.H \
	selection/current_selection_paste_handlerobj.H \
	selection/incremental_selection_updates.C   \
	selection/incremental_selection_updates.H   \
	selection/convert_selection.C		    \
	service_window_handler.C		    \
	service_window_handler.H		    \
	shared_handler_data.C			    \
	shared_handler_data.H			    \
	shared_handler_datafwd.H		    \
	shared_handler_dataobj.H		    \
	shortcut.C				    \
	shortcut/independent_shortcut_activation.C  \
	shortcut/independent_shortcut_activation.H  \
	shortcut/independent_shortcut_activationfwd.H \
	shortcut/independent_shortcut_activationobj.H \
	shortcut/installed_shortcut.C		    \
	shortcut/installed_shortcut.H		    \
	shortcut/installed_shortcutfwd.H	    \
	shortcut/installed_shortcutobj.H	    \
	shortcut/shortcut_activation_element.H	    \
	shortcut/shortcut_activation_elementfwd.H   \
	shortcut/shortcut_activation_element_impl.C \
	shortcut/shortcut_activation_element_impl.H \
	singletonlayoutmanager.C		    \
	singletonlayoutmanager_impl.C		    \
	size_hints.C				    \
	size_hints.H				    \
	size_hintsfwd.H				    \
	size_hintsobj.H				    \
	straight_border.C			    \
	straight_border.H			    \
	straight_borderfwd.H			    \
	straight_borderobj.H			    \
	straight_border_impl.C			    \
	straight_border_impl.H			    \
	synchronized_axis_impl.C		    \
	synchronized_axis_impl.H		    \
	synchronized_axis_value.C		    \
	synchronized_axis_value.H		    \
	synchronized_axis_valuefwd.H		    \
	synchronized_axis_valueobj.H		    \
	pagefactory.C				    \
	pagefactory_impl.C			    \
	pagefactory_impl.H			    \
	pagelayoutmanager.C			    \
	pagelayoutmanager_impl.C		    \
	pagelayoutmanager_impl.H		    \
	table.C					    \
	tablelayoutmanager/tablelayoutmanager.C	    \
	tablelayoutmanager/tablelayoutmanager_impl.C \
	tablelayoutmanager/tablelayoutmanager_impl.H \
	tablelayoutmanager/table_synchronized_axis.C \
	tablelayoutmanager/table_synchronized_axis.H \
	tablelayoutmanager/table_synchronized_axisfwd.H \
	tablelayoutmanager/table_synchronized_axisobj.H \
	textlabel.C				    \
	textlabel.H				    \
	textlabel_impl.C			    \
	sxg/sxg_parser.C			    \
	sxg/sxg_parser.H			    \
	sxg/sxg_parserfwd.H			    \
	sxg/sxg_parserobj.H			    \
	sxg/sxg_parser_text.C			    \
	sync_counter.C				    \
	sync_counter.H				    \
	text_param.C				    \
	theme_text.C				    \
	themeborder_element_impl.C		    \
	themedim.C				    \
	themedim_element_minoverride.H		    \
	themedim_element_minoverridefwd.H	    \
	theme_font.C				    \
	themeiconobj.H				    \
	themeicon.C				    \
	themeiconpixmapobj.H			    \
	themeiconpixmap.C			    \
	theme_parser_lockfwd.H			    \
	theme_parser_lock.H			    \
	theme_parser_lock.C			    \
	toolboxlayoutmanager/toolboxfactory.C       \
	toolboxlayoutmanager/toolboxlayoutmanager.C \
	toolboxlayoutmanager/toolboxlayoutmanager_impl.C \
	toolboxlayoutmanager/toolboxlayoutmanager_impl.H \
	tooltip.C				    \
	uicompilerfwd.H				    \
	uicompiler.H				    \
	uicompiler.C				    \
	uicompiler2.C				    \
	uicompiler3.C				    \
	uicompiler4.C				    \
	uicompiler_booklayout.C			    \
	uicompiler_borderlayout.C		    \
	uicompiler_bookpagefactory.C		    \
	uicompiler_editable_comboboxlayout.C	    \
	uicompiler_gridfactory.C		    \
	uicompiler_gridlayout.C			    \
	uicompiler_itemlayout.C			    \
	uicompiler_listlayout.C			    \
	uicompiler_menubarfactory.C		    \
	uicompiler_menubarlayout.C		    \
	uicompiler_standard_comboboxlayout.C	    \
	uicompiler_new_booklayout.C		    \
	uicompiler_new_borderlayout.C		    \
	uicompiler_new_custom_comboboxlayout.C	    \
	uicompiler_new_editable_comboboxlayout.C    \
	uicompiler_new_itemlayout.C		    \
	uicompiler_new_listlayout.C		    \
	uicompiler_new_pagelayout.C		    \
	uicompiler_new_panelayout.C		    \
	uicompiler_new_standard_comboboxlayout.C    \
	uicompiler_new_tablelayout.C		    \
	uicompiler_new_toolboxlayout.C		    \
	uicompiler_pagefactory.C		    \
	uicompiler_pagelayout.C			    \
	uicompiler_panefactory.C		    \
	uicompiler_panelayout.C			    \
	uicompiler_tablelayout.C		    \
	uicompiler_toolboxfactory.C		    \
	uicompiler_toolboxlayout.C		    \
	uigenerators.C				    \
	uielements.C				    \
	windowobj.C				    \
	windowobj.H				    \
	window_handler.C			    \
	window_handler.H			    \
	window_handlerfwd.H			    \
	window_handler_constructor_params.H	    \
	xid_t.C					    \
	xid_t.H					    \
	xid_t_connection_thread.H		    \
	xid_t_fwd.H				    \
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
	xim_generic_window.C			    \
	xim_generic_window.H			    \
        #


$(call REFCOLLECTION_GEN,print_dialog/print_dialog.inc.H, \
			print_dialog/print_dialog.xml)

$(call REFCOLLECTION_GEN,color_picker/color_picker.inc.H, \
			color_picker/color_picker.xml)

$(call REFCOLLECTION_GEN,font_picker/font_picker.inc.H, \
			font_picker/font_picker.xml)

libcxxwcups_la_LIBADD=libcxxw.la -lcxxcups -lcxx -lcourier-unicode
libcxxwcups_la_LDFLAGS=$(STATICLINKFLAGS) -version-info @VERSION_INFO@
libcxxwcups_la_SOURCES=\
	print_dialog/print_dialog.C		    \
	print_dialog/print_dialog_impl.C	    \
	print_dialog/print_dialog_impl.H	    \
        #

XIM_AUTOGEN=xim/ximclient.inc.C xim/ximclient2.inc.C xim/ximclient.inc.H

TESTLINKTYPE=$(if $(STATIC),-static $(STATICLINKFLAGS))

cxxw_SOURCES=cxxw.C
cxxw_LDADD=libcxxw.la
cxxw_LDFLAGS=$(TESTLINKTYPE)

cxxwtheme_SOURCES=cxxwtheme.C
cxxwtheme_LDADD=libcxxw.la -lcourier-unicode
cxxwtheme_LDFLAGS=$(TESTLINKTYPE)


$(call REFCOLLECTION_GEN,creator/appelements.inc.H,$(srcdir)/creator/appelements.xml)
$(call EXTRACT_THEME_TEXT_GEN,creator.inc.C,creator/main.xml)

creatorinst_DATA=creator/main.xml
EXTRA_DIST += $(creatorinst_DATA)

cxxwcreator_SOURCES=\
	creator/app.C				\
	creator/app.H				\
	creator/appdimension.C			\
	creator/appcolors.C			\
	creator/cxxwcreator.C			\
	#

cxxwcreator_LDADD=libcxxw.la -lcxx
cxxwcreator_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,cxxwoptions.H,cxxwoptions.xml)

man_MANS=cxxwtheme.1

EXTRA_DIST += config.rpath m4/ChangeLog COPYING \
	pic_constants.xml gc_constants.xml constants.xsl \
	parseuicompiler.xsl uicompiler.xml \
	xim/xim.xml xim/xim.xsl \
	cxxwtheme.desktop \
	packaging/fedora/libcxxw.spec \
        examples/configure.ac \
	examples/Makefile.am \
	appearance/create_headers.xsl \
	appearance/appearance_parser.xsl \
	all_appearancesfwd.xsl \
	all_appearances.xsl \
	appearance/appearance_create_list.xsl \
	appearance/appearance_create_proto.xsl \
        $(shell cd $(srcdir) && \
                  echo appearance/*.xml examples/*.C examples/*.H) \
	$(man_MANS)

$(call THREADMSGDISPATCHER_GEN,connection_thread.msgs.H, connection_thread.msgs.xml)

$(call THREADMSGDISPATCHER_GEN,dirlisting/filedircontents_impl.msgs.H,dirlisting/filedircontents_impl.msgs.xml)

TESTPROGRAMS=\
	testcollapse					\
	testdateinputfilter				\
	testderivedaxis					\
	testfiledircontents				\
	testfocusable					\
	testgrid					\
	testlist					\
	testrichtextstring				\
	testrichtext					\
	testrichtextiterator				\
	testscrollbar_metrics			        \
	testsearch					\
	testtextparam					\
	#

INTERACTIVETESTPROGRAMS=\
	testbook					\
	testbusy					\
	testbutton					\
	testcolorpicker					\
	testcombobox					\
	testcustomcanvas				\
	testdateinput					\
	testfocusrestore				\
	testimagebuttons			        \
testinputfield					\
	testitemlayoutmanager				\
	testmainwindow					\
	testmenu					\
	testlabel					\
	testpeephole					\
	testping					\
	testprogressbar					\
	testunfocus					\
	testscrollbar					\
	testshowhide					\
	testpage					\
	testpane					\
	testtoolbox					\
	testtooltip					\
	testwordwrappablelabel				\
	#

EXTRA_PROGRAMS=$(TESTPROGRAMS) $(INTERACTIVETESTPROGRAMS)

noinst_PROGRAMS=$(BUILT_TESTPROGRAMS) $(BUILT_INTERACTIVETESTPROGRAMS) genconstants

testbook_SOURCES=testbook.C
testbook_LDADD=libcxxw.la
testbook_LDFLAGS=$(TESTLINKTYPE)

$(call THREADMSGDISPATCHER_GEN,testbusy.inc.H,testbusy.xml)
testbusy_SOURCES=testbusy.C
testbusy_LDADD=libcxxw.la
testbusy_LDFLAGS=$(TESTLINKTYPE)

testbutton_SOURCES=testbutton.C
testbutton_LDADD=libcxxw.la
testbutton_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,testcolorpicker.inc.H,testcolorpicker.xml)
testcolorpicker_SOURCES=testcolorpicker.C
testcolorpicker_LDADD=libcxxw.la
testcolorpicker_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,testcombobox.inc.H,testcombobox.xml)
testcombobox_SOURCES=testcombobox.C testcombobox.inc.H
testcombobox_LDADD=libcxxw.la
testcombobox_LDFLAGS=$(TESTLINKTYPE)

testcustomcanvas_SOURCES=testcustomcanvas.C
testcustomcanvas_LDADD=libcxxw.la
testcustomcanvas_LDFLAGS=-static $(STATICLINKFLAGS)

testdateinput_SOURCES=testdateinput.C
testdateinput_LDADD=libcxxw.la
testdateinput_LDFLAGS=$(TESTLINKTYPE)

testimagebuttons_SOURCES=testimagebuttons.C
testimagebuttons_LDADD=libcxxw.la
testimagebuttons_LDFLAGS=-static $(STATICLINKFLAGS)

$(call REFCOLLECTION_GEN,testinputfield.inc.H,testinputfield.xml)

testinputfield_SOURCES=testinputfield.C
testinputfield_LDADD=libcxxw.la
testinputfield_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testitemlayoutmanager.inc.H,testitemlayoutmanager.xml)
testitemlayoutmanager_SOURCES=testitemlayoutmanager.C
testitemlayoutmanager_LDADD=libcxxw.la -lcourier-unicode
testitemlayoutmanager_LDFLAGS=$(TESTLINKTYPE)

testcollapse_SOURCES=testcollapse.C
testcollapse_LDADD=-lcxx
testcollapse_LDFLAGS=-static $(STATICLINKFLAGS)

testdateinputfilter_SOURCES=testdateinputfilter.C
testdateinputfilter_LDADD=-lcxx -lcourier-unicode
EXTRA_DIST += testdateinput.xml

testderivedaxis_SOURCES=testderivedaxis.C
testderivedaxis_LDADD=libcxxw.la
testderivedaxis_LDFLAGS=-static $(STATICLINKFLAGS)

testfiledircontents_SOURCES=testfiledircontents.C
testfiledircontents_LDADD=libcxxw.la
testfiledircontents_LDFLAGS=-static $(STATICLINKFLAGS)

testfocusable_SOURCES=testfocusable.C
testfocusable_LDADD=-lcxx

testfocusrestore_SOURCES=testfocusrestore.C
testfocusrestore_LDADD=libcxxw.la
testfocusrestore_LDFLAGS=$(TESTLINKTYPE)

testgrid_SOURCES=testgrid.C
testgrid_LDADD=libcxxw.la
testgrid_LDFLAGS=-static

$(call OPTIONS_GEN,testlabel.inc.H,testlabel.xml)
testlabel_SOURCES=testlabel.C
testlabel_LDADD=libcxxw.la
testlabel_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testlistoptions.H,testlistoptions.xml)
testlist_SOURCES=testlist.C
testlist_LDADD=libcxxw.la
testlist_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testmainwindowoptions.H,testmainwindowoptions.xml)

testmainwindow_SOURCES=testmainwindow.C
testmainwindow_LDADD=libcxxw.la -lcxx
testmainwindow_LDFLAGS=-static $(STATICLINKFLAGS)

testmenu_SOURCES=testmenu.C
testmenu_LDADD=libcxxwcups.la libcxxw.la
testmenu_LDFLAGS=$(TESTLINKTYPE)

$(call OPTIONS_GEN,testpage.inc.H,testpage.xml)
testpage_SOURCES=testpage.C
testpage_LDADD=libcxxw.la
testpage_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testpane.inc.H,testpane.xml)
testpane_SOURCES=testpane.C testpane_theme.xml
testpane_LDADD=libcxxw.la
testpane_LDFLAGS=$(TESTLINKTYPE)

testpeephole_SOURCES=testpeephole.C
testpeephole_LDADD=libcxxw.la
testpeephole_LDFLAGS=$(TESTLINKTYPE)

testping_SOURCES=testping.C
testping_LDADD=libcxxw.la
testping_LDFLAGS=$(TESTLINKTYPE)

testprogressbar_SOURCES=testprogressbar.C
testprogressbar_LDADD=libcxxw.la
testprogressbar_LDFLAGS=-static $(STATICLINKFLAGS)

testrichtext_SOURCES=testrichtext.C
testrichtext_LDADD=libcxxw.la
testrichtext_LDFLAGS=-static $(STATICLINKFLAGS)

testrichtextiterator_SOURCES=testrichtextiterator.C
testrichtextiterator_LDADD=libcxxw.la
testrichtextiterator_LDFLAGS=-static $(STATICLINKFLAGS)

testrichtextstring_SOURCES=testrichtextstring.C
testrichtextstring_LDADD=-lcxx

testscrollbar_SOURCES=testscrollbar.C
testscrollbar_LDADD=libcxxw.la
testscrollbar_LDFLAGS=-static $(STATICLINKFLAGS)

testscrollbar_metrics_SOURCES=testscrollbar_metrics.C
testscrollbar_metrics_LDADD=libcxxw.la
testscrollbar_metrics_LDFLAGS=-static $(STATICLINKFLAGS)

$(call REFCOLLECTION_GEN,testshowhide.inc.H,testshowhide.xml)
testshowhide_SOURCES=testshowhide.C
testshowhide_LDADD=libcxxw.la

testsearch_SOURCES=testsearch.C
testsearch_LDADD=-lcxx
testsearch_LDFLAGS=-static $(STATICLINKFLAGS)

XTEST_MO_DIR=xtest/en_US/LC_MESSAGES
XTEST_MO_FILE=xtest.mo

BUILT_SOURCES += $(XTEST_MO_DIR)/$(XTEST_MO_FILE)
CLEANFILES += $(XTEST_MO_DIR)/$(XTEST_MO_FILE)
EXTRA_DIST += testmessages.po

$(XTEST_MO_DIR)/$(XTEST_MO_FILE): testmessages.po
	mkdir -p $(XTEST_MO_DIR)
	rm -f $(XTEST_MO_DIR)/$(XTEST_MO_FILE)
	msgfmt -o $(XTEST_MO_DIR)/$(XTEST_MO_FILE).tmp $(srcdir)/testmessages.po
	mv $(XTEST_MO_DIR)/$(XTEST_MO_FILE).tmp $(XTEST_MO_DIR)/$(XTEST_MO_FILE)

BUILT_SOURCES += testtextparam_dummy_xml
CLEANFILES += testtextparam_dummy_xml

testtextparam_dummy_xml:
	echo '<theme />' >testtextparam_dummy_xml

$(call EXTRACT_THEME_TEXT_GEN,theme-text.inc.C,themes/default/theme.xml)

testtextparam_SOURCES=testtextparam.C $(XTEST_MO_DIR)/$(XTEST_MO_FILE) \
        testtextparam_dummy_xml

testtextparam_LDADD=libcxxw.la
testtextparam_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testtoolboxoptions.H,testtoolboxoptions.xml)
testtoolbox_SOURCES=testtoolbox.C
testtoolbox_LDADD=libcxxw.la
testtoolbox_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testtooltip.H,testtooltip.xml)
testtooltip_SOURCES=testtooltip.C testtooltip.H
testtooltip_LDADD=libcxxw.la
testtooltip_LDFLAGS=-static $(STATICLINKFLAGS)

testunfocus_SOURCES=testunfocus.C
testunfocus_LDADD=libcxxw.la
testunfocus_LDFLAGS=-static $(STATICLINKFLAGS)

$(call OPTIONS_GEN,testwordwrappablelabel.inc.H,testwordwrappablelabel.xml)
testwordwrappablelabel_SOURCES=testwordwrappablelabel.C
testwordwrappablelabel_LDADD=libcxxw.la
testwordwrappablelabel_LDFLAGS=-static $(STATICLINKFLAGS)

# Pick up all appearance/*_appearance.xml file, get a list of all the *
#
# This is the list of appearance files, this is used to...
ALL_APPEARANCES := $(shell cd $(srcdir)/appearance && ls *_appearance.xml | sed 's/_appearance.xml$$//')


ALL_APPEARANCES_XML = \
	$(patsubst %,appearance/%_appearance.xml,$(ALL_APPEARANCES))

# Generate appearance_parser_inc.C
#
# First we generate a master XML document that XIncludes ALL_APPEARANCES
appearance/appearance_parser.inc.C: \
		appearance/appearance_parser.xsl appearance/appearance_parser.xml
	$(XSLTPROC) -xinclude $(srcdir)/appearance/appearance_parser.xsl \
          appearance/appearance_parser.xml \
              >appearance/appearance_parser.inc.C.tmp
	mv -f appearance/appearance_parser.inc.C.tmp \
		appearance/appearance_parser.inc.C

appearance/appearance_list.xml: appearance/appearance_parser.xml \
	appearance/appearance_create_list.xsl
	$(XSLTPROC) -xinclude $(srcdir)/appearance/appearance_create_list.xsl \
		appearance/appearance_parser.xml \
		>appearance/appearance_list.xml.tmp
	sed '1d;s/\&amp;/\&/g' <appearance/appearance_list.xml.tmp \
		>appearance/appearance_list.xml.tmp2
	rm -f appearance/appearance_list.xml.tmp
	mv -f appearance/appearance_list.xml.tmp2 appearance/appearance_list.xml

appearance/appearance_parser.xml: $(ALL_APPEARANCES_XML)
	echo "<?xml version='1.0'?>" >appearance/appearance_parser.xml.tmp
	echo "<root xmlns:xi='http://www.w3.org/2003/XInclude'>" >>appearance/appearance_parser.xml.tmp
	for f in $(ALL_APPEARANCES) ; \
        do \
		echo "<xi:include href='$(abs_top_srcdir)/appearance/$${f}_appearance.xml' xpointer='xpointer(/appearance)' />" >>appearance/appearance_parser.xml.tmp ; \
	done
	echo '</root>' >>appearance/appearance_parser.xml.tmp
	mv -f appearance/appearance_parser.xml.tmp appearance/appearance_parser.xml

includes/x/w/all_appearancesfwd.H: \
	all_appearancesfwd.xsl appearance/appearance_parser.xml
	$(XSLTPROC) -xinclude $(srcdir)/all_appearancesfwd.xsl \
          appearance/appearance_parser.xml \
              >includes/x/w/all_appearancesfwd.H.tmp
	mv -f includes/x/w/all_appearancesfwd.H.tmp includes/x/w/all_appearancesfwd.H

includes/x/w/all_appearances.H: \
	all_appearances.xsl appearance/appearance_parser.xml
	$(XSLTPROC) -xinclude $(srcdir)/all_appearances.xsl \
          appearance/appearance_parser.xml \
              >includes/x/w/all_appearances.H.tmp
	mv -f includes/x/w/all_appearances.H.tmp includes/x/w/all_appearances.H

uicompiler_appearance_proto.inc.H: \
	appearance/appearance_create_proto.xsl appearance/appearance_parser.xml
	$(XSLTPROC) -xinclude $(srcdir)/appearance/appearance_create_proto.xsl \
          appearance/appearance_parser.xml \
              >uicompiler_appearance_proto.inc.H.tmp
	mv -f uicompiler_appearance_proto.inc.H.tmp uicompiler_appearance_proto.inc.H

BUILT_SOURCES += includes/x/w/constants_inc.H \
	$(XIM_AUTOGEN) \
	appearance/appearance_list.xml     \
	appearance/appearance_parser.xml \
	appearance/appearance_parser.inc.C \
	includes/x/w/all_appearancesfwd.H \
	includes/x/w/all_appearances.H \
	uicompiler_appearance_proto.inc.H \
	gc_constants.inc.C pic_constants.inc.C

CLEANFILES += \
	gc_constants.inc.C pic_constants.inc.C \
	appearance/appearance_list.xml \
	appearance/appearance_parser.inc.C \
	appearance/appearance_parser.xml \
	uicompiler_appearance_proto.inc.H \
        $(XIM_AUTOGEN)

dist-hook:
	rm -f $(distdir)/appearance/appearance_parser.xml

includes/x/w/constants_inc.H: genconstants
	./genconstants >$@.tmp
	mv $@.tmp $@

gc_constants.inc.C: gc_constants.xml constants.xsl
	$(XSLTPROC) $(srcdir)/constants.xsl $(srcdir)/gc_constants.xml >gc_constants.inc.C.tmp
	mv -f gc_constants.inc.C.tmp gc_constants.inc.C

pic_constants.inc.C: pic_constants.xml constants.xsl
	$(XSLTPROC) $(srcdir)/constants.xsl $(srcdir)/pic_constants.xml >pic_constants.inc.C.tmp
	mv -f pic_constants.inc.C.tmp pic_constants.inc.C

xim/ximclient.inc.H: xim/xim.xml xim/xim.xsl
	$(XSLTPROC) --stringparam mode request-prototypes $(srcdir)/xim/xim.xsl $(srcdir)/xim/xim.xml >xim/ximclient.inc.H.tmp
	mv -f xim/ximclient.inc.H.tmp xim/ximclient.inc.H

xim/ximclient.inc.C: xim/xim.xml xim/xim.xsl
	$(XSLTPROC) --stringparam mode request $(srcdir)/xim/xim.xsl $(srcdir)/xim/xim.xml >xim/ximclient.inc.C.tmp
	mv -f xim/ximclient.inc.C.tmp xim/ximclient.inc.C

xim/ximclient2.inc.C: xim/xim.xml xim/xim.xsl
	$(XSLTPROC) --stringparam mode received-switch $(srcdir)/xim/xim.xsl $(srcdir)/xim/xim.xml >xim/ximclient2.inc.C.tmp
	mv -f xim/ximclient2.inc.C.tmp xim/ximclient2.inc.C

uicompiler.stamp.inc.H: uicompiler.xml parseuicompiler.xsl
	rm -rf uicompiler.inc.H
	mkdir uicompiler.inc.H
	$(XSLTPROC) $(srcdir)/parseuicompiler.xsl $(srcdir)/uicompiler.xml
	touch uicompiler.stamp.inc.H

uicompiler.lo					\
uicompiler_booklayout.lo			\
uicompiler_bookpagefactory.lo			\
uicompiler_borderlayout.lo			\
uicompiler_editable_comboboxlayout.lo		\
uicompiler_gridfactory.lo			\
uicompiler_gridlayout.lo			\
uicompiler_itemlayout.lo			\
uicompiler_listlayout.lo			\
uicompiler_menubarlayout.lo			\
uicompiler_new_booklayout.lo			\
uicompiler_new_borderlayout.lo			\
uicompiler_new_custom_comboboxlayout.lo		\
uicompiler_new_editable_comboboxlayout.lo	\
uicompiler_new_itemlayout.lo			\
uicompiler_new_listlayout.lo			\
uicompiler_new_pagelayout.lo			\
uicompiler_new_panelayout.lo			\
uicompiler_new_standard_comboboxlayout.lo	\
uicompiler_new_tablelayout.lo			\
uicompiler_new_toolboxlayout.lo			\
uicompiler_pagefactory.lo			\
uicompiler_pagelayout.lo			\
uicompiler_panefactory.lo			\
uicompiler_panelayout.lo			\
uicompiler_standard_comboboxlayout.lo		\
uicompiler_toolboxfactory.lo			\
uicompiler_toolboxlayout.lo			\
	: uicompiler.stamp.inc.H

clean-local:
	rm -rf uicompiler.stamp.inc.H uicompiler.inc.H

genconstants_SOURCES=genconstants.C

if HAVE_DOCS

DOCIMAGES=$(patsubst docbook/%,html/%,$(wildcard docbook/*.gif) $(wildcard docbook/*.jpg) $(wildcard docbook/*.png))

define install_docimage
html/$(1): html/index.html docbook/$(1)
	$(INSTALL) -m 0644 $(srcdir)/docbook/$(1) html/$(1)
endef

ALL_DOCS=html/book.css html/.htaccess html/frame.js \
      $(DOCIMAGES)

docs: $(ALL_DOCS) html/index.html
	-chcon -R -t httpd_sys_content_t html/. 2>/dev/null || true
	rm -rf ~/www/www.courier-mta.org/libcxx/w
	rsync -a -X html/. ~/www/www.courier-mta.org/libcxx/w

DOCS_SRC = $(shell find includes -name '*.H' -print)

$(foreach f,$(patsubst html/%,%,$(ALL_DOCS)),$(eval $(call install_docimage,$(f))))

html/index.html: html/ref/index.html $(wildcard docbook/*.xml docbook/xmllinks*.xsl) appearance/appearance_list.xml
	mkdir -p docbook
	echo '<tags>' >docbook/entities.tmp
	$(XSLTPROC) --stringparam 'path' '/ref' $(srcdir)/docbook/xmllinks-extract.xsl ../libcxx/xml/index.xml >>docbook/entities.tmp
	$(XSLTPROC) --stringparam 'path' 'ref' $(srcdir)/docbook/xmllinks-extract.xsl xml/index.xml >>docbook/entities.tmp
	echo '</tags>' >>docbook/entities.tmp
	$(XSLTPROC) $(srcdir)/docbook/xmllinks-dedupe.xsl docbook/entities.tmp >docbook/entities.tmp2
	$(XSLTPROC) $(srcdir)/docbook/xmllinks-generate.xsl docbook/entities.tmp2 >docbook/entities
	rm -f docbook/entities.tmp docbook/entities.tmp2
	rm -rf html.tmp
	mkdir -p html.tmp
	$(XSLTPROC) --nonet -o html.tmp/ --xinclude \
		--stringparam use.id.as.filename 1 \
		--stringparam html.stylesheet book.css \
		--stringparam root.filename 'toc' \
		--stringparam generate.id.attributes 1 \
		--param local.l10n.xml 'document("'`pwd`/docbook/l10.xml'")' \
		$(srcdir)/docbook/docbook.xsl $(srcdir)/docbook/book.xml
	mkdir html.tmp/ref
	ln html/ref/* html.tmp/ref
	find examples -depth ! -name autom4te.cache \( -type d -o -name '*.[CH]' -o -name '*.xml' -o -name '*.jpg' -o -name Makefile.am -o -name configure.ac \) -print | cpio -o --quiet | (cd html.tmp; cpio -i --quiet --make-directories)
	echo 'ForceType text/plain' >html.tmp/examples/.htaccess
	echo 'Options +Indexes' >>html.tmp/examples/.htaccess
	links -dump -no-numbering -no-references html.tmp/INSTALL.html >INSTALL.tmp
	mv -f INSTALL.tmp INSTALL
	-rm -rf html 2>/dev/null >/dev/null || :
	mv html.tmp html

$(man_MANS): man.stamp
	touch $@

CLEANFILES += $(man_MANS)

man.stamp: $(wildcard docbook/cxxwtheme.xml)
	set -e; rm -rf manpages; \
	$(XSLTPROC) --nonet -o manpages/ http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl docbook/book.xml; \
	mv -f manpages/* . ; touch man.stamp
	rm -rf manpages

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

$(call passthrough,all_installed_theme_files += $(DESTDIR)$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@/themes/$(2)/$(3))
$(call passthrough,all_theme_source_files += $(1))

$(DESTDIR)$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@/themes/$(2)/$(3): $(1) install-data-hook-create-dir
	$(INSTALL_DATA) $(1) $(DESTDIR)$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@/themes/$(2)/$(3)
endef

define install_theme
$(call passthrough,all_installed_theme_dirs += $(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@/themes/$(1))

$(foreach f,$(wildcard $(srcdir)/themes/$(1)/*.sxg) $(wildcard $(srcdir)/themes/$(1)/*.xml), $(call install_theme_file,$(f),$(1),$(notdir $(f))))
endef

$(foreach d,$(all_themes),$(eval $(call install_theme,$d)))

install-data-hook: $(all_installed_theme_files)

install-data-hook-create-dir:
	$(MKDIR_P) $(foreach d,$(all_installed_theme_dirs),$(DESTDIR)$(d))

uninstall-hook:
	rm -f $(all_installed_theme_files)
	-rmdir $(foreach d,$(all_installed_theme_dirs),$(DESTDIR)$(d))
	-rmdir $(DESTDIR)$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@/themes
	-rmdir $(DESTDIR)$(pkgdatadir)-@ABIVER@-@LIBCXX_VERSION@

EXTRA_DIST += $(patsubst $(srcdir)/%,%,$(all_theme_source_files))

check-am:
	@test -n "$(BUILT_TESTPROGRAMS)" || exit 0; for p in $(BUILT_TESTPROGRAMS); do echo $$p; ./$$p --test || exit 1; done

test:
	./testmainwindow --state
	./testmainwindow --state --nopadding
	./testmainwindow --exception
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
	./testlabel --truncatable
	LC_ALL=ru_RU.UTF-8 ./testtooltip --width=25
	./testtooltip --width=50
	./testprogressbar
	./testitemlayoutmanager --testitems
	LC_ALL=ru_RU.UTF-8 ./testpage --testfocus
	./testwordwrappablelabel --testmetrics
	./testunfocus
	./testtoolbox --norestore -w=4 --testmetrics
	./testtoolbox --testmetrics
	./testtoolbox --norestore --testthemescale
	./testcombobox -l

.PHONY: rpm

# Make sure auto-generated files get built, before make dist, because that
# generates PO files, which looks for source files.
dist: $(BUILT_SOURCES)

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

upload:
	git tag -s -a @PACKAGE@/@VERSION@/`date +%Y%m%d%H%M%S` -m 'Tag version @VERSION@'
	rm -f @PACKAGE@-@VERSION@.tar.bz2.sig; gpg --detach-sign --default-key mrsam@courier-mta.com @PACKAGE@-@VERSION@.tar.bz2; test -f @PACKAGE@-@VERSION@.tar.bz2.sig || exit 1
	pag upload @PACKAGE@-@VERSION@.tar.bz2
	pag upload @PACKAGE@-@VERSION@.tar.bz2.sig
