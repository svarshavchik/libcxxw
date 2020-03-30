/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_borderlayoutmanagerobj_h
#define x_w_borderlayoutmanagerobj_h

#include <x/w/borderlayoutmanagerfwd.H>
#include <x/w/singletonlayoutmanagerobj.H>
#include <x/w/text_paramfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see borderlayoutmanager

class borderlayoutmanagerObj : public singletonlayoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	borderlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~borderlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Update the title.

	void update_title(const text_param &);

	//! Update the title

	void update_title(ONLY IN_THREAD, const text_param &);

	//! Update the border
	void update_border(const border_arg &new_border);

	//! Update the border
	void update_border(ONLY IN_THREAD, const border_arg &new_border);

	//! Update all borders

	//! Specify individual borders for each side.
	void update_borders(const border_arg &new_left_border,
			    const border_arg &new_right_border,
			    const border_arg &new_top_border,
			    const border_arg &new_bottom_border);

	//! Update all borders

	//! Specify individual borders for each side.
	void update_borders(ONLY IN_THREAD,
			    const border_arg &new_left_border,
			    const border_arg &new_right_border,
			    const border_arg &new_top_border,
			    const border_arg &new_bottom_border);

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
