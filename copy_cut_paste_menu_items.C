/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/copy_cut_paste_menu_items.H"
#include "x/w/focusable.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/main_window.H"
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

// Implement the copy_cut_paste_menu_item object.

class copy_cut_paste_implObj : public copy_cut_paste_menu_itemsObj {

public:

	// Weakly capture the parent element and the list layout manager.

	copy_cut_paste_implObj(const element_impl &e_impl,
			       const ref<listlayoutmanagerObj::implObj>
			       &lm_impl)
		: e_implptr{e_impl}, lm_implptr{lm_impl}
	{
	}

	weakptr<element_implptr> e_implptr;
	weakptr<ptr<listlayoutmanagerObj::implObj>> lm_implptr;

	// get_status() recovers strong references to our objects. If
	// successfull, get_status() returns:
	//
	// - The list layout manager, with the menu items.
	//
	// - The parent display element.
	//
	// - A flag indicating whether "Paste" should be enabled. The
	//   status of "Copy" and "Cut" depends upon whether this is
	//   called IN_THREAD, or not.

	typedef std::optional<std::tuple<listlayoutmanager,
					 element_impl, bool>> get_status_t;

	get_status_t get_status()
	{
		get_status_t ret;

		auto lm_impl=lm_implptr.getptr();
		auto e_impl=e_implptr.getptr();

		if (lm_impl && e_impl)
		{
			auto mw=e_impl->get_window_handler().get_main_window();

			if (mw)
			{
				auto s=e_impl->get_screen();

				listlayoutmanager l=
					lm_impl->create_public_object();

				ret.emplace(l,
					    e_impl,
					    s->selection_has_owner()
					    && mw->selection_can_be_received());
			}
		}

		return ret;
	}

	// Implement update().

	void update(size_t n) override
	{
		auto s=get_status();

		if (!s)
			return;

		auto &[l, e_impl, pastable]=*s;

		auto cut_or_copy=e_impl->get_window_handler()
			.cut_or_copy_selection
			(cut_or_copy_op::available,
			 e_impl->default_cut_paste_selection());

		l->enabled(n, cut_or_copy);
		l->enabled(n+1, cut_or_copy);
		l->enabled(n+2, pastable);
	}

	void update(ONLY IN_THREAD, size_t n) override
	{
		auto s=get_status();

		if (!s)
			return;

		auto &[l, e_impl, pastable]=*s;

		auto cut_or_copy=e_impl->get_window_handler()
			.cut_or_copy_selection
			(IN_THREAD, cut_or_copy_op::available,
			 e_impl->default_cut_paste_selection());

		l->enabled(IN_THREAD, n, cut_or_copy);
		l->enabled(IN_THREAD, n+1, cut_or_copy);
		l->enabled(IN_THREAD, n+2, pastable);
	}

	element_implptr get_element_impl()
	{
		return e_implptr.getptr();
	}
};

// Create the copy/cut/paste menu items.

// The key combinations are implemented directly in editorObj::implObj,
// so we specify that their shorcuts are inactive_shortcut.

static std::vector<list_item_param>
get_copy_cut_paste_popup_menu_items(const ref<copy_cut_paste_implObj> &me)
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
	};
}

#if 0
{
#endif
}

copy_cut_paste_menu_items
listlayoutmanagerObj::append_copy_cut_paste(const element &parent)
{
	auto me=ref<copy_cut_paste_implObj>::create(parent->impl, impl);

	append_items(get_copy_cut_paste_popup_menu_items(me));

	return me;
}

copy_cut_paste_menu_items
listlayoutmanagerObj::append_copy_cut_paste(ONLY IN_THREAD,
					    const element &parent)
{
	auto me=ref<copy_cut_paste_implObj>::create(parent->impl, impl);

	append_items(IN_THREAD, get_copy_cut_paste_popup_menu_items(me));

	return me;
}

LIBCXXW_NAMESPACE_END
