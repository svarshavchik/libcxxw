/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_previewobj_h
#define x_w_font_picker_previewobj_h

#include "font_picker/font_picker_previewfwd.H"
#include "peephole/peepholed_fontelementfwd.H"
#include "x/w/labelobj.H"
#include "x/w/fontfwd.H"
#include "x/w/font_arg.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see font_picker_preview

class font_picker_previewObj : public peepholed_fontelementObj<
	peepholed_elementObj<labelObj>> {

	//! Alias
	typedef peepholed_fontelementObj<peepholed_elementObj<labelObj>
					 > superclass_t;
public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	font_picker_previewObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~font_picker_previewObj() LIBCXX_HIDDEN;

	//! Update the font preview

	//! Used by the font picker popup
	void update_preview(ONLY IN_THREAD,
			    const font &updated_font) LIBCXX_HIDDEN;

	//! Update the font preview

	//! Used by cxxwcreator
	void update_preview(ONLY IN_THREAD,
			    const font_arg &updated_font) LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
