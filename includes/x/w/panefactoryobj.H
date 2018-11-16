/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panefactoryobj_h
#define x_w_panefactoryobj_h

#include <x/w/panefactoryfwd.H>
#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/alignment.H>

#include <x/mpobj.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see panefactory

class panefactoryObj : public factoryObj {

public:
	//! My layout manager.

	const panelayoutmanager layout;

	//! Default initial size of the next pane.

	inline panefactoryObj &set_initial_size(const dim_arg &arg)
	{
		set_initial_size_set(arg);
		return *this;
	}

	//! Next pane will have this custom background color.

	inline panefactoryObj &set_background_color(const color_arg &arg)
	{
		set_background_color_set(arg);
		return *this;
	}

	//! Visibility for the next pane's scroll-bar.
	inline panefactoryObj &set_scrollbar_visibility(scrollbar_visibility v)
	{
		set_scrollbar_visibility_set(v);
		return *this;
	}

	//! Set padding for the next pane.

	inline panefactoryObj &left_padding(const dim_arg &arg)
	{
		left_padding_set(arg);
		return *this;
	}

	//! Set padding for the next pane.

	inline panefactoryObj &right_padding(const dim_arg &arg)
	{
		right_padding_set(arg);
		return *this;
	}

	//! Set padding for the next pane.

	inline panefactoryObj &top_padding(const dim_arg &arg)
	{
		top_padding_set(arg);
		return *this;
	}

	//! Set padding for the next pane.

	inline panefactoryObj &bottom_padding(const dim_arg &arg)
	{
		bottom_padding_set(arg);
		return *this;
	}

	//! Set padding for the next pane.

	inline panefactoryObj &padding(const dim_arg &arg)
	{
		padding_set(arg);
		return *this;
	}

	//! Set custom alignment of the element in the new pane.

	inline panefactoryObj &halign(LIBCXXW_NAMESPACE::halign arg)
	{
		halign_set(arg);
		return *this;
	}

	//! Set custom alignment of the element in the new pane.

	inline panefactoryObj &valign(LIBCXXW_NAMESPACE::valign arg)
	{
		valign_set(arg);
		return *this;
	}

	//! Default initial size of the next pane.

	virtual void set_initial_size_set(const dim_arg &arg)=0;

	//! Visibility for the next pane's scroll-bar.
	virtual void set_scrollbar_visibility_set(scrollbar_visibility v)=0;

	//! Next pane will have this custom background color.

	virtual void set_background_color_set(const color_arg &arg)=0;

	//! Set padding for the next pane.

	virtual void left_padding_set(const dim_arg &)=0;

	//! Set padding for the next pane.

	virtual void right_padding_set(const dim_arg &)=0;

	//! Set padding for the next pane.

	virtual void top_padding_set(const dim_arg &)=0;

	//! Set padding for the next pane.

	virtual void bottom_padding_set(const dim_arg &)=0;

	//! Set padding for the next pane.

	virtual void padding_set(const dim_arg &)=0;

	//! Set custom alignment of the element in the new pane.

	virtual void halign_set(LIBCXXW_NAMESPACE::halign)=0;

	//! Set custom alignment of the element in the new pane.

	virtual void valign_set(LIBCXXW_NAMESPACE::valign)=0;

	//! Constructor
	panefactoryObj(const panelayoutmanager &layout) LIBCXX_HIDDEN;

	//! Destructor
	~panefactoryObj() LIBCXX_HIDDEN;

	//! Configure a new list or a table.

	//! Configure a new list for best visual appearance.
	void configure_new_list(new_listlayoutmanager &);
};

LIBCXXW_NAMESPACE_END

#endif
