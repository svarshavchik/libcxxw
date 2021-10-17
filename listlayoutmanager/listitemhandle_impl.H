/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemhandle_impl_h
#define listitemhandle_impl_h

#include "x/w/listitemhandle.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/extra_list_row_infofwd.H"
#include <x/weakptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class listitemhandleObj::implObj : public listitemhandleObj {

	const weakptr<ptr<listlayoutmanagerObj::implObj>> listlayout_impl;

	const weakptr<extra_list_row_infoptr> extra;

	struct recover_weak_ref;

public:
	//! Constructor
	implObj(const ref<listlayoutmanagerObj::implObj> &impl,
		const extra_list_row_info &extra);

	//! Destructor
	~implObj();

	//! Implement enabled()
	bool enabled() const override;

	//! Implement enabled()
	void enabled(bool flag) override;

	//! Implement enabled()
	void enabled(ONLY IN_THREAD, bool) override;

	//! Implement selected()
	bool selected() const override;

	//! Implement selected()
	void selected(bool selected_flag) override;

	//! Implement selected()
	void selected(ONLY IN_THREAD, bool selected_flag,
		      const callback_trigger_t &trigger) override;

	using listitemhandleObj::selected;

	//! Implement autoselect()
	void autoselect() override;

	//! Implement autoselect()
	void autoselect(ONLY IN_THREAD, const callback_trigger_t &trigger)
		override;

	using listitemhandleObj::autoselect;

	//! Return the menu item's layout manager.
	listlayoutmanagerptr submenu_listlayout() const override;

	//! Install a new status update callback.
	void on_status_update(const list_item_status_change_callback &)
		override;

	//! Install a new status update callback.
	void on_status_update(ONLY IN_THREAD,
			      const list_item_status_change_callback &)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
