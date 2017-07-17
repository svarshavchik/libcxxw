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
#include <x/w/border_infomm.H>
#include <x/w/alignment.H>
#include <x/w/dimarg.H>
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

	class implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor

	gridfactoryObj(const layoutmanager &layout,
		       const ref<gridlayoutmanagerObj::implObj> &gridlayout,
		       const ref<implObj> &implArg) LIBCXX_HIDDEN;

	//! Destructor
	~gridfactoryObj() LIBCXX_HIDDEN;

	//! Set new element's width

	gridfactoryObj &colspan(size_t);

	//! Set new element's height

	gridfactoryObj &rowspan(size_t);

	//! Set new element's alignment

	gridfactoryObj &halign(LIBCXXW_NAMESPACE::halign);

	//! Set new element's alignment

	gridfactoryObj &valign(LIBCXXW_NAMESPACE::valign);

	//! Set new element's border, first.

	gridfactoryObj &border(const border_infomm &info);

	//! Set new element's border, first.

	gridfactoryObj &left_border(const border_infomm &info);

	//! Set new element's border, first.

	gridfactoryObj &right_border(const border_infomm &info);

	//! Set new element's border, first.

	gridfactoryObj &top_border(const border_infomm &info);

	//! Set new element's border, first.

	gridfactoryObj &bottom_border(const border_infomm &info);

	//! Set new element's border, first.
	gridfactoryObj &border(const std::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &left_border(const std::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &right_border(const std::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &top_border(const std::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &bottom_border(const std::string_view &id);

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	template<typename Arg>
	inline gridfactoryObj &padding(Arg &&padding)
	{
		return padding_set(dimarg(std::forward<Arg>(padding)));
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	template<typename Arg>
	inline gridfactoryObj &left_padding(Arg &&padding)
	{
		return left_padding_set(dimarg(std::forward<Arg>(padding)));
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	template<typename Arg>
	inline gridfactoryObj &right_padding(Arg &&padding)
	{
		return right_padding_set(dimarg(std::forward<Arg>(padding)));
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	template<typename Arg>
	inline gridfactoryObj &top_padding(Arg &&padding)
	{
		return top_padding_set(dimarg(std::forward<Arg>(padding)));
	}

	//! Set new element's padding, first.

	//! The padding can be specified as a theme setting, in form of a
	//! string, or a \c double value, specifying the padding in
	//! millimeters.
	template<typename Arg>
	inline gridfactoryObj &bottom_padding(Arg &&padding)
	{
		return bottom_padding_set(dimarg(std::forward<Arg>(padding)));
	}

	//! Implement padding()
	gridfactoryObj &padding_set(const dimarg &padding);

	//! Implement left_padding()
	gridfactoryObj &left_padding_set(const dimarg &padding);

	//! Implement right_padding()
	gridfactoryObj &right_padding_set(const dimarg &padding);

	//! Implement top_padding()
	gridfactoryObj &top_padding_set(const dimarg &padding);

	//! Implement bottom_padding()
	gridfactoryObj &bottom_padding_set(const dimarg &padding);

	//! When this display element is hidden, "remove" it from the grid.

	//! It is not actually removed, just ignored for the purposes of
	//! calculating the grid layout.

	gridfactoryObj &remove_when_hidden(bool=true);

private:
	//! Created a new display element for the grid.
	void created(const element &new_element) override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
