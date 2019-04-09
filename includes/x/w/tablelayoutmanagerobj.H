/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanagerobj_h
#define x_w_tablelayoutmanagerobj_h

#include <x/w/tablelayoutmanagerfwd.H>
#include <x/w/screen_positionsfwd.H>
#include <x/w/listlayoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see tablelayoutmanager

class tablelayoutmanagerObj : public listlayoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Internal implementation object.
	class LIBCXX_HIDDEN table_synchronized_axisObj;

	//! Constructor
	tablelayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~tablelayoutmanagerObj() LIBCXX_HIDDEN;

	//! Replace a column header.

	factory replace_header(size_t column);

	//! Return the element representing the column header;

	element header(size_t column) const;

	//! Save current adjusted size of the columns in the tables.

	//! For adjustable tables, to be restored next time.
	void save(const std::string &, const const_screen_positions &) const;
};

LIBCXXW_NAMESPACE_END

#endif
