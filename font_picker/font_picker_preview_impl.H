/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef font_picker_preview_impl_h
#define font_picker_preview_impl_h

#include "font_picker/font_picker_preview.H"
#include "x/w/impl/theme_font_elementfwd.H"
#include "label_elementfwd.H"
#include "x/w/impl/always_visible_elementfwd.H"
#include "x/w/impl/fonts/current_fontcollectionfwd.H"
#include "x/w/impl/child_elementobj.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/font.H"
#include "textlabel.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN font_picker_previewObj::implObj
	: public always_visible_elementObj<theme_font_elementObj<
						   label_elementObj
						   <child_elementObj>>> {

	//! Alias

	typedef always_visible_elementObj<theme_font_elementObj
					  <label_elementObj
					   <child_elementObj>>> superclass_t;

	//! The font being previewed.
	font current_font_thread_only;

	//! The current theme used for previewing fonts.
	defaulttheme current_theme;

	//! The current preview's fontcollection

	//! Store it here, so it gets cached and won't have to be rebuilt for
	//! the actual label text.
	fontcollectionptr current_fontcollection;

 public:
	THREAD_DATA_ONLY(current_font);

	//! Constructor
	implObj(const container_impl &parent_container,
		textlabel_config &init_args);

 public:
	//! Destructor
	~implObj();

	//! Update the font preview

	//! Sets current_font, then calls refresh_preview().
	void update_preview(ONLY IN_THREAD,
			    const font &updated_font);

	//! Override initialize()

	//! Makes sure we refresh ourselves if the current theme changed
	//! after this object was constructed.

	void initialize(ONLY IN_THREAD) override;

	//! Override theme_updated

	//! Saved current_theme, and calls refresh_preview().
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
		override;
 private:

	//! Recreate the preview label.

	void refresh_preview(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
