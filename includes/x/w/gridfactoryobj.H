/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridfactoryobj_h
#define x_w_gridfactoryobj_h

#include <x/w/gridfactoryfwd.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/factoryobj.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/border_arg.H>
#include <x/w/alignment.H>
#include <x/w/dim_arg.H>
#include <x/mpobj.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Implement a factory for the grid layout manager.

//! \see gridfactory

class gridfactoryObj : public factoryObj {

	//! The public layout manager
	const layoutmanager layout;

	//! This internal object.
	const ref<gridlayoutmanagerObj::implObj> gridlayout;

	//! The internal lock
	grid_map_t::lock lock;

 public:

	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor

	gridfactoryObj(const layoutmanager &layout,
		       const ref<gridlayoutmanagerObj::implObj> &gridlayout,
		       const ref<implObj> &implArg) LIBCXX_HIDDEN;

	//! Destructor
	~gridfactoryObj() LIBCXX_HIDDEN;

	//! Implement get_container_impl()

	container_impl get_container_impl() override LIBCXX_HIDDEN;

	//! Return get_element_impl()

	elementObj::implObj &get_element_impl() override LIBCXX_HIDDEN;

	//! Set new element's width

	inline gridfactoryObj &colspan(size_t s)
	{
		colspan_set(s);
		return *this;
	}

	//! Set new element's height

	inline gridfactoryObj &rowspan(size_t s)
	{
		rowspan_set(s);
		return *this;
	}

	//! Set new element's alignment

	inline gridfactoryObj &halign(LIBCXXW_NAMESPACE::halign h)
	{
		halign_set(h);
		return *this;
	}

	//! Set new element's alignment

	inline gridfactoryObj &valign(LIBCXXW_NAMESPACE::valign v)
	{
		valign_set(v);
		return *this;
	}

	//! Set new element's border, first.

	inline gridfactoryObj &border(const border_arg &arg)
	{
		border_set(arg);
		return *this;
	}

	//! Set new element's border, first.

	inline gridfactoryObj &left_border(const border_arg &arg)
	{
		left_border_set(arg);
		return *this;
	}

	//! Set new element's border, first.

	inline gridfactoryObj &right_border(const border_arg &arg)
	{
		right_border_set(arg);
		return *this;
	}

	//! Set new element's border, first.

	inline gridfactoryObj &top_border(const border_arg &arg)
	{
		top_border_set(arg);
		return *this;
	}

	//! Set new element's border, first.

	inline gridfactoryObj &bottom_border(const border_arg &arg)
	{
		bottom_border_set(arg);
		return *this;
	}

	//! Set new element's rounded border and padding.

	inline gridfactoryObj &rounded_border_and_padding(const border_arg &arg)
	{
		rounded_border_set(arg);
		return *this;
	}

	//! Implement colspan().

	void colspan_set(size_t s);

	//! Implement rowspan().

	void rowspan_set(size_t s);

	//! Implement halign().

	void halign_set(LIBCXXW_NAMESPACE::halign h);

	//! Implement valign().

	void valign_set(LIBCXXW_NAMESPACE::valign v);

	//! Implement border().

	void border_set(const border_arg &arg);

	//! Implement rounded_border_and_padding().

	void rounded_border_set(const border_arg &arg);

	//! Implement left_border().

	void left_border_set(const border_arg &arg);

	//! Implement right_border().

	void right_border_set(const border_arg &arg);

	//! Implement top_border().

	void top_border_set(const border_arg &arg);

	//! Implement bottom_border().

	void bottom_border_set(const border_arg &arg);

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	inline gridfactoryObj &padding(const dim_arg &arg)
	{
		padding_set(arg);
		return *this;
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	inline gridfactoryObj &left_padding(const dim_arg &arg)
	{
		left_padding_set(arg);
		return *this;
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	inline gridfactoryObj &right_padding(const dim_arg &arg)
	{
		right_padding_set(arg);
		return *this;
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	inline gridfactoryObj &top_padding(const dim_arg &arg)
	{
		top_padding_set(arg);
		return *this;
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	inline gridfactoryObj &bottom_padding(const dim_arg &arg)
	{
		bottom_padding_set(arg);
		return *this;
	}

	//! Implement padding()
	void padding_set(const dim_arg &padding);

	//! Implement left_padding()
	void left_padding_set(const dim_arg &padding);

	//! Implement right_padding()
	void right_padding_set(const dim_arg &padding);

	//! Implement top_padding()
	void top_padding_set(const dim_arg &padding);

	//! Implement bottom_padding()
	void bottom_padding_set(const dim_arg &padding);

	//! When this display element is hidden, "remove" it from the grid.

	//! It is not actually removed, just ignored for the purposes of
	//! calculating the grid layout.

	inline gridfactoryObj &remove_when_hidden(bool flag=true)
	{
		remove_when_hidden_set(flag);
		return *this;
	}

	void remove_when_hidden_set(bool);

private:
	//! Created a new display element for the grid.
	void created(const element &new_element) override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
