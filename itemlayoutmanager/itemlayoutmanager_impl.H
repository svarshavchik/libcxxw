/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef itemlayoutmanager_impl_h
#define itemlayoutmanager_impl_h

#include "x/w/itemlayoutmanager.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/item_layout_appearance.H"
#include "itemlayoutmanager/peepholed_item_containerobj.H"
#include "itemlayoutmanager/itembuttonfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class itemlayoutmanagerObj::implObj : public layoutmanagerObj::implObj {

 public:
	//! My container
	const ref<peepholed_item_containerObj::implObj> layout_container_impl;

	const const_item_layout_appearance appearance;

	//! Constructor
	implObj(const ref<peepholed_item_containerObj::implObj>
		&layout_container_impl,
		const new_itemlayoutmanager &config);

	//! Destructor
	~implObj();

	//! Implement do_for_each_child()
	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &e)> &)
		override;

	//! Implement num_children().
	size_t num_children(ONLY IN_THREAD) override;

	//! Implement create_public_object()

	layoutmanager create_public_object() override;

	//! Implement recalculate()
	void recalculate(ONLY IN_THREAD) override;

	//! Implement process_updated_position()
	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;

	//! Items we are managing.
	item_info_t item_info;

	//! Add new item after all existing items.
	void append(const itembutton &);

	//! Add new item after all existing items.
	void append(const itembutton &, item_info_t::lock &);

	//! Insert new item before an existing item.
	void insert(const itembutton &, size_t);

	//! Insert new item before an existing item.
	void insert(const itembutton &, size_t, item_info_t::lock &);

	//! Initialize an inserted or an appended button.
	void initialize(const itembutton &, item_info_t::lock &);

	//! Number of items.
	size_t size();

	//! Remove existing items.
	void remove_items(size_t i, size_t n);

	//! Return an existing item
	element get_item(size_t);

	//! Return a vector of focusable objects.
	std::vector<focusable> get_all_focusables();

	//! Return the "main" focusable

	//! If there's at least one item button, its delete button is the
	//! main focusable. Otherwise it's the horizontal scrollbar.
	focusable get_main_focusable();
};

LIBCXXW_NAMESPACE_END

#endif
