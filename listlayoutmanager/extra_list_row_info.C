/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "popup/popup.H"
#include "popup/popup_handler.H"
#include "shortcut/shortcut_activation_element_impl.H"
#include "activated_in_thread.H"
#include "generic_window_handler.H"
#include "x/w/shortcut.H"

#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START

////////////////////////////////////////////////////////////////////////////
//
// Some work is needed to implement a keyboard shortcut for this list item.
//
// We need to save a ref to the shortcut's extra_list_row_info, which would
// obviously be a weak reference.
//
// When executing the shortcut we also need to instantiate the
// listlayoutmanager. We can capture the reference to the container
// with the listlayoutmanager. Since its the parent container, and
// implementation objects hold references to their parent implementation
// object, we're clear.

class extra_list_row_infoObj::shortcut_implObj
	: public shortcut_activation_element_implObj,
	  public activated_in_threadObj {

	//! The parent container with the listlayoutmanager

	const container_impl list_container_impl;

	//! Our extra_list_row_info

	//! NOTE: we can only access it after acquiring a textlist_info_lock!
	weakptr<extra_list_row_infoptr> extra_ptr;

	typedef list_elementObj::implObj::textlist_info_lock textlist_info_lock;
	typedef list_elementObj::implObj
	::create_textlist_info_lock create_textlist_info_lock;

public:

	shortcut_implObj(const listlayoutmanager &lm,
			 const extra_list_row_info &extra)
		: list_container_impl{lm->layoutmanagerObj::impl
			->layout_container_impl},
		  extra_ptr(extra)
	{
	}

	~shortcut_implObj()=default;

	//! Implement shortcut_window_handler()

	//! Inherited from shortcut_activation_element_implObj,

	generic_windowObj::handlerObj &shortcut_window_handler() override
	{
		return list_container_impl->get_window_handler();
	}

	///////////////////////////////////////////////////////////////
	//
	// Inherited from activated_in_threadObj

	//! Is the shortcut enabled?

	bool enabled(ONLY IN_THREAD) override
	{
		bool enabled=false;

		list_container_impl->invoke_layoutmanager
			([&, this]
			 (const ref<listlayoutmanagerObj::implObj> &l_impl)
			 {
				 auto impl=l_impl->list_element_singleton->impl;

				 create_textlist_info_lock lock{IN_THREAD,
								*impl};

				 auto p=extra_ptr.getptr();

				 if (p)
					 enabled=p->enabled(lock);
			 });

		return enabled;
	}

	//! Shortcut activated

	void activated(ONLY IN_THREAD, const callback_trigger_t &trigger)
		override
	{
		listlayoutmanagerptr lm;

		list_container_impl->invoke_layoutmanager
			([&]
			 (const auto &impl)
			 {
				 lm=impl->create_public_object();
			 });

		if (!lm)
			return;

		auto impl=lm->impl->list_element_singleton->impl;

		// We must acquire a list lock first.

		list_lock ll{lm};

		// Now, make sure we recalculate the list,
		// if needed.

		textlist_info_lock lock{IN_THREAD, ll, *impl};

		auto p=extra_ptr.getptr();

		if (!p)
			return;

		lm->autoselect(IN_THREAD,
			       p->current_row_number(IN_THREAD), trigger);
	}
};

/////////////////////////////////////////////////////////////////////////////


extra_list_row_infoObj::extra_list_row_infoObj()=default;

extra_list_row_infoObj::~extra_list_row_infoObj()
{
	// Must explicitly do this.

	// No need for locking any more. This is the destructor.
	if (data_under_lock.current_shortcut)
		data_under_lock.current_shortcut->uninstall_shortcut();
}

bool extra_list_row_infoObj::enabled(listimpl_info_t::lock &lock) const
{
	return data(lock).row_type == list_row_type_t::enabled;
}

void extra_list_row_infoObj::set_meta(const listlayoutmanager &lm,
				      list_row_info_t &row_info,
				      listimpl_info_t::lock &lock,
				      const textlist_rowinfo &meta)
{
	data(lock).status_change_callback=meta.listitem_callback;

	data(lock).menu_item=meta.menu_item;
	row_info.indent=meta.indent_level;

	if (meta.inactive_shortcut ||
	    !meta.listitem_shortcut || !*meta.listitem_shortcut)
	{
		if (data(lock).current_shortcut)
		{
			data(lock).current_shortcut->uninstall_shortcut();
			data(lock).current_shortcut=nullptr;
		}
		return;
	}

	auto extra=ref{this};

	if (!data(lock).current_shortcut)
		data(lock).current_shortcut=shortcut_impl::create(lm, extra);

	// Our destructor explicitly calls uninstall_shortcut().

	// Install a global shortcut. This must be a shortcut for a
	// menu item.
	data(lock).current_shortcut
		->install_shortcut(*meta.listitem_shortcut,
				   data(lock).current_shortcut, true);
}

void extra_list_row_infoObj::show_submenu(ONLY IN_THREAD,
					  listimpl_info_t::lock &lock,
					  const rectangle &r)
{
	if (!has_submenu(lock))
		return;

	auto &popup=std::get<menu_item_submenu>(data(lock).menu_item);

	popup.submenu_popup_handler
		->update_attachedto_element_position(IN_THREAD, r);
	popup.submenu_popup->show_all();
}

void extra_list_row_infoObj::toggle_submenu(ONLY IN_THREAD,
					    listimpl_info_t::lock &lock,
					    const rectangle &r)
{
	if (!has_submenu(lock))
		return;

	auto &popup=std::get<menu_item_submenu>(data(lock).menu_item);

	popup.submenu_popup_handler
		->update_attachedto_element_position(IN_THREAD, r);

	if (popup.submenu_popup->elementObj::impl->data(IN_THREAD)
	    .requested_visibility)
		popup.submenu_popup->elementObj::impl
			->request_visibility(IN_THREAD, false);
	else
		popup.submenu_popup->elementObj::impl
			->request_visibility_recursive(IN_THREAD, true);
}

listlayoutmanager extra_list_row_infoObj
::submenu_layoutmanager(listimpl_info_t::lock &lock)
{
	return std::get<menu_item_submenu>(data(lock).menu_item).submenu_popup
		->get_layoutmanager();
}

LIBCXXW_NAMESPACE_END
