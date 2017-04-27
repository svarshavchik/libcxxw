/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridfactoryobj_h
#define x_w_gridfactoryobj_h

#include <x/w/gridfactoryfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/border_infomm.H>
#include <x/w/alignment.H>
#include <x/mpobj.H>
#include <experimental/string_view>

LIBCXXW_NAMESPACE_START

//! Implement a factory for the grid layout manager.

//! \see gridfactory

class gridfactoryObj : public factoryObj {

 public:

	//! This factory is for this layout.
	const gridlayoutmanager gridlayout;

	class implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor

	gridfactoryObj(const gridlayoutmanager &gridlayout,
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
	gridfactoryObj &border(const std::experimental::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &left_border(const std::experimental::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &right_border(const std::experimental::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &top_border(const std::experimental::string_view &id);

	//! Set new element's border, first.
	gridfactoryObj &bottom_border(const std::experimental::string_view &id);

	//! Set new element's padding, first.

	gridfactoryObj &padding(double paddingmm);

	//! Set new element's padding, first.

	gridfactoryObj &left_padding(double paddingmm);

	//! Set new element's padding, first.

	gridfactoryObj &right_padding(double paddingmm);

	//! Set new element's padding, first.

	gridfactoryObj &top_padding(double paddingmm);

	//! Set new element's padding, first.

	gridfactoryObj &bottom_padding(double paddingmm);

private:
	//! Created a new display element for the grid.
	void created(const element &new_element) override LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
