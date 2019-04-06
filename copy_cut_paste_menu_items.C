/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/copy_cut_paste_menu_items.H"
#include "x/w/focusable.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/main_window.H"
#include "x/w/listitemhandle.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "generic_window_handler.H"
#include "messages.H"
#include <tuple>
#include <optional>

LIBCXXW_NAMESPACE_START

copy_cut_paste_menu_itemsObj::copy_cut_paste_menu_itemsObj()=default;

copy_cut_paste_menu_itemsObj::~copy_cut_paste_menu_itemsObj()=default;

namespace {
#if 0
}
#endif

//! Implementation object.

//! \see target_element

class weak_target_elementObj : virtual public obj {

public:

	weakptr<element_implptr> e_implptr;

	// Weakly capture the parent element and the list layout manager.

	weak_target_elementObj(const element_impl &e_impl)
		: e_implptr{e_impl}
	{
	}

	~weak_target_elementObj()=default;

	element_implptr get_element_impl()
	{
		return e_implptr.getptr();
	}
};

//! Weak reference to cut/copy/paste display element.

//! Both the copy_cut_paste_menu_items object itself, and the callbacks
//! for the menu items, share this object, whose only purpose is to stash
//! away a weak reference to display element.
//!
//! The menu items callback recover the strong reference, and execute the
//! operation.
//!
//! update() recover the strong reference, and use that to determine whether
//! the menu items should be enabled, or not.

typedef ref<weak_target_elementObj> weak_target_element;

//! Implement the copy_cut_paste_menu_item object.

class copy_cut_paste_implObj : public copy_cut_paste_menu_itemsObj {

public:

	//! The targeted display element
	const weak_target_element target_element;

	//! Handle for the menu item.
	const listitemhandle copy_handle;

	//! Handle for the menu item.
	const listitemhandle cut_handle;

	//! Handle for the menu item.
	const listitemhandle paste_handle;

	// This is constructed after the copy/cut/paste items are added
	// to the menu list. Save their handles.

	copy_cut_paste_implObj(const weak_target_element &target_element,
			       const std::vector<listitemhandle> &handles)
		: target_element{target_element},
		  copy_handle{handles.at(0)},
		  cut_handle{handles.at(1)},
		  paste_handle{handles.at(2)}
	{
	}

	//! Return value from get_status().

	//! get_status() recovers strong references to our objects. If
	//! successfull, get_status() returns:
	//!
	//! - The list layout manager, with the menu items.
	//!
	//! - The target display element.
	//!
	//! - A flag indicating whether "Paste" should be enabled. The
	//!   determination of whether "Copy" and "Cut" uses IN_THREAD and
	//!   non-IN_THREAD overloads.

	typedef std::optional<std::tuple<element_impl, bool>> get_status_t;

	//! Tell update() what's going on.

	get_status_t get_status()
	{
		get_status_t ret;

		auto e_impl=target_element->get_element_impl();

		if (e_impl)
		{
			auto mw=e_impl->get_window_handler().get_main_window();

			if (mw)
			{
				auto s=e_impl->get_screen();

				ret.emplace(e_impl,
					    s->selection_has_owner()
					    && mw->selection_can_be_received());
			}
		}

		return ret;
	}

	// Implement update().

	void update() override
	{
		auto s=get_status();

		if (!s)
			return;

		auto &[e_impl, pastable]=*s;

		auto cut_or_copy=e_impl->get_window_handler()
			.cut_or_copy_selection
			(cut_or_copy_op::available,
			 e_impl->default_cut_paste_selection());

		copy_handle->enabled(cut_or_copy);
		cut_handle->enabled(cut_or_copy);
		paste_handle->enabled(pastable);
	}

	//! Implement update()

	void update(ONLY IN_THREAD) override
	{
		auto s=get_status();

		if (!s)
			return;

		auto &[e_impl, pastable]=*s;

		auto cut_or_copy=e_impl->get_window_handler()
			.cut_or_copy_selection
			(IN_THREAD, cut_or_copy_op::available,
			 e_impl->default_cut_paste_selection());

		copy_handle->enabled(IN_THREAD, cut_or_copy);
		cut_handle->enabled(IN_THREAD, cut_or_copy);
		paste_handle->enabled(IN_THREAD, pastable);
	}
};

// Create the copy/cut/paste menu items.

// The key combinations are implemented directly in editorObj::implObj,
// so we specify that their shorcuts are inactive_shortcut.

static std::vector<list_item_param>
get_copy_cut_paste_popup_menu_items(const weak_target_element &me,
				    std::vector<listitemhandle> &handles)
{
	return {
		[me]
		(ONLY IN_THREAD,
		 const auto &status_info)
		{
			auto e_impl=me->get_element_impl();

			if (!e_impl)
				return;

			auto &wh=e_impl->get_window_handler();

			wh.cut_or_copy_selection
				(IN_THREAD,
				 cut_or_copy_op::copy,
				 e_impl->default_cut_paste_selection());
		},
		inactive_shortcut{"Ctrl-Ins"},
		{_("Copy")},

		[me]
		(ONLY IN_THREAD,
		 const auto &status_info)
		{
			auto e_impl=me->get_element_impl();

			if (!e_impl)
				return;

			auto &wh=e_impl->get_window_handler();

			wh.cut_or_copy_selection
				(IN_THREAD,
				 cut_or_copy_op::cut,
				 e_impl->default_cut_paste_selection());
		},
		inactive_shortcut{"Shift-Del"},
		{_("Cut")},

		[me]
		(ONLY IN_THREAD,
		 const auto &status_info)
		{
			auto e_impl=me->get_element_impl();

			if (!e_impl)
				return;

			auto &wh=e_impl->get_window_handler();

			if (!wh.get_autorestorable_focusable())
				return;

			wh.receive_selection
				(IN_THREAD,
				 e_impl->default_cut_paste_selection());
		},
		inactive_shortcut{"Shift-Ins"},
		{_("Paste")},

		new_items{handles}
	};
}

#if 0
{
#endif
}

copy_cut_paste_menu_items
listlayoutmanagerObj::append_copy_cut_paste(const element &parent)
{
	auto me=ref{this};

	list_lock lock{me};

	auto target_element=weak_target_element::create(parent->impl);

	std::vector<listitemhandle> handles;

	append_items(get_copy_cut_paste_popup_menu_items(target_element,
							 handles));

	return ref<copy_cut_paste_implObj>::create(target_element, handles);
}

copy_cut_paste_menu_items
listlayoutmanagerObj::append_copy_cut_paste(ONLY IN_THREAD,
					    const element &parent)
{
	auto me=ref{this};

	list_lock lock{me};

	auto target_element=weak_target_element::create(parent->impl);

	std::vector<listitemhandle> handles;

	append_items(IN_THREAD,
		     get_copy_cut_paste_popup_menu_items(target_element,
							 handles));

	return ref<copy_cut_paste_implObj>::create(target_element, handles);
}

LIBCXXW_NAMESPACE_END
