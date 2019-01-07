/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholelayoutmanager_h
#define x_w_peepholelayoutmanager_h

#include <x/w/peepholelayoutmanagerfwd.H>
#include <x/w/peepholelayoutmanagerobj.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/peephole_style.H>
#include <x/w/scrollbar.H>
#include <x/w/factory.H>
#include <x/w/border_arg.H>
#include <x/w/dim_arg.H>
#include <x/w/alignment.H>
#include <x/w/element.H>

#include <x/functionalrefptr.H>
#include <optional>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC new_peepholelayoutmanager;
class LIBCXX_PUBLIC new_scrollable_peepholelayoutmanager;

//! Create a new container that uses the \ref peepholelayoutmanager "peephole layout manager".

class new_peepholelayoutmanager : public new_layoutmanager,
				  public peephole_style {

public:
	//! Peepholed element factory.
	functionref<void (const factory &)> peepholed_factory;

	//! Constructor
	new_peepholelayoutmanager(const functionref<void (const factory &)> &);

	//! Destructor
	~new_peepholelayoutmanager();

private:
	//! Internal implementation
	layout_impl create(const container_impl &) const override;

	//! Internal implementation
	container create(const container_impl &,
			 const function<void(const container &)> &)
		const override;
};

//! Create a new container that uses the \ref peepholelayoutmanager "peephole layout manager", with provided scroll-bars.

class new_scrollable_peepholelayoutmanager : public new_focusable_layoutmanager,
					     public peephole_style {

public:
	//! Peepholed element factory.
	functionref<void (const factory &)> peepholed_factory;

	//! Constructor
	new_scrollable_peepholelayoutmanager(const functionref<void
					     (const factory &)> &);

	//! Destructor
	~new_scrollable_peepholelayoutmanager();

	//! Optional border
	std::optional<border_arg> peephole_border;

	//! Horizontal scroll-bar visibility
	scrollbar_visibility horizontal_scrollbar=
		scrollbar_visibility::automatic;

	//! Vertical scroll-bar visibility
	scrollbar_visibility vertical_scrollbar=
		scrollbar_visibility::automatic;

	//! Extra padding for the element in the peephole.

	dim_arg left_padding=0;

	//! Extra padding for the element in the peephole.

	dim_arg right_padding=0;

	//! Extra padding for the element in the peephole.

	dim_arg top_padding=0;

	//! Extra padding for the element in the peephole.

	dim_arg bottom_padding=0;

private:
	//! Internal implementation
	focusable_container create(const container_impl &) const override
		LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
