/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "popup/popup.H"
#include "popup/popup_attachedto_handler.H"
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

	const ref<containerObj::implObj> container_impl;

	//! Our extra_list_row_info

	//! NOTE: we can only access it after acquiring a textlist_info_lock!
	weakptr<extra_list_row_infoptr> extra_ptr;

	typedef list_elementObj::implObj::textlist_info_lock textlist_info_lock;

public:

	shortcut_implObj(const listlayoutmanager &lm,
			 const extra_list_row_info &extra)
		: container_impl(lm->layoutmanagerObj::impl->container_impl),
		  extra_ptr(extra)
	{
	}

	~shortcut_implObj()=default;

	//! Implement shortcut_window_handler()

	//! Inherited from shortcut_activation_element_implObj,

	generic_windowObj::handlerObj &shortcut_window_handler() override
	{
		return container_impl->get_window_handler();
	}

	///////////////////////////////////////////////////////////////
	//
	// Inherited from activated_in_threadObj

	//! Is the shortcut enabled?

	bool enabled(IN_THREAD_ONLY) override
	{
		bool enabled=false;

		container_impl->invoke_layoutmanager
			([&, this]
			 (const ref<listlayoutmanagerObj::implObj> &l_impl)
			 {
				 auto impl=l_impl->list_element_singleton->impl;

				 textlist_info_lock lock{IN_THREAD, *impl};

				 auto p=extra_ptr.getptr();

				 if (p)
					 enabled=p->enabled();
			 });

		return enabled;
	}

	//! Shortcut activated

	void activated(IN_THREAD_ONLY, const callback_trigger_t &trigger)
		override
	{
		listlayoutmanagerptr lm;

		container_impl->invoke_layoutmanager
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

		textlist_info_lock lock{IN_THREAD, *impl};

		auto p=extra_ptr.getptr();

		if (!p)
			return;

		lm->autoselect(p->current_row_number(IN_THREAD), trigger);
	}
};

/////////////////////////////////////////////////////////////////////////////


extra_list_row_infoObj::extra_list_row_infoObj()=default;

extra_list_row_infoObj::~extra_list_row_infoObj()
{
	// Must explicitly do this.

	if (current_shortcut)
		current_shortcut->uninstall_shortcut();
}

void extra_list_row_infoObj
::default_status_change_callback(const list_item_status_info_t &)
{
}

bool extra_list_row_infoObj::enabled() const
{
	return row_type == list_row_type_t::enabled;
}

void extra_list_row_infoObj::set_meta(const listlayoutmanager &lm,
				      const textlist_rowinfo &meta)
{
	if (meta.listitem_callback)
		status_change_callback= *meta.listitem_callback;
	else
		status_change_callback=default_status_change_callback;

	menu_item=meta.menu_item;

	if (!meta.listitem_shortcut || !*meta.listitem_shortcut)
	{
		if (current_shortcut)
		{
			current_shortcut->uninstall_shortcut();
			current_shortcut=nullptr;
		}
		return;
	}

	auto extra=ref(this);

	if (!current_shortcut)
		current_shortcut=shortcut_impl::create(lm, extra);

	// Our destructor explicitly calls uninstall_shortcut().
	current_shortcut->install_shortcut(*meta.listitem_shortcut,
					   current_shortcut);
}

void extra_list_row_infoObj::show_submenu(IN_THREAD_ONLY, const rectangle &r)
{
	if (!has_submenu())
		return;

	auto &popup=std::get<menu_item_submenu>(menu_item);

	popup.submenu_popup_handler
		->update_attachedto_element_position(IN_THREAD, r);
	popup.submenu_popup->show_all();
}

void extra_list_row_infoObj::toggle_submenu(IN_THREAD_ONLY, const rectangle &r)
{
	if (!has_submenu())
		return;

	auto &popup=std::get<menu_item_submenu>(menu_item);

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

listlayoutmanager extra_list_row_infoObj::submenu_layoutmanager()
{
	return std::get<menu_item_submenu>(menu_item).submenu_popup
		->get_layoutmanager();
}

LIBCXXW_NAMESPACE_END
