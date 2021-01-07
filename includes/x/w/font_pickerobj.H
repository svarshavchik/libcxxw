/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_pickerobj_h
#define x_w_font_pickerobj_h

#include <x/w/font_pickerfwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see font_picker

class font_pickerObj : public focusable_containerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	font_pickerObj(const ref<implObj> &impl,
		       const ref<containerObj::implObj> &c_impl,
		       const layout_impl &container_layoutmanager)
		LIBCXX_HIDDEN;

	//! Destructor
	~font_pickerObj() LIBCXX_HIDDEN;

	//! Override get_impl().
	focusable_impl get_impl() const override LIBCXX_HIDDEN;

	//! Install a new callback that gets invoked to report a new font.
	void on_font_update(const functionref<font_picker_callback_t> &cb);

	//! Return the currently selected font.
	font current_font() const;

	//! Replace the font shown in the popup with this font.
	void current_font(const font &f);

	//! Replace the font shown in the popup with this font.
	void current_font(ONLY IN_THREAD, const font &f);

	//! Set most recently used font family groups.
	void most_recently_used(const std::vector<font_picker_group_id> &);

	//! Set most recently used font family groups.
	void most_recently_used(ONLY IN_THREAD,
				const std::vector<font_picker_group_id> &);

	//! Get the most recently used font family groups

	std::vector<font_picker_group_id> most_recently_used() const;
};

LIBCXXW_NAMESPACE_END

#endif
