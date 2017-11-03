/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlistlayoutmanager/textlist_impl.H"
#include "textlistlayoutmanager/extra_list_row_info.H"
#include "textlistlayoutmanager/textlistlayoutmanager_impl.H"
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
// textlistlayoutmanager. We can capture the reference to the container
// with the textlistlayoutmanager. Since its the parent container, and
// implementation objects hold references to their parent implementation
// object, we're clear.

class extra_list_row_infoObj::shortcut_implObj
	: public shortcut_activation_element_implObj,
	  public activated_in_threadObj {

	//! The parent container with the textlistlayoutmanager

	const ref<containerObj::implObj> container_impl;

	//! Our extra_list_row_info

	//! NOTE: we can only access it after acquiring a textlist_info_lock!
	weakptr<extra_list_row_infoptr> extra_ptr;

	typedef textlistObj::implObj::textlist_info_lock textlist_info_lock;

public:

	shortcut_implObj(const textlistlayoutmanager &lm,
			 const extra_list_row_info &extra)
		: container_impl(lm->layoutmanagerObj::impl->container_impl),
		  extra_ptr(extra)
	{
	}

	~shortcut_implObj()=default;

	//! Implement shortcut_window_handler()

	//! Inherited from shortcut_activation_element_implObj,

	generic_windowObj::handlerObj &shortcut_window_handler()
	{
		return container_impl->get_window_handler();
	}

	///////////////////////////////////////////////////////////////
	//
	// Inherited from activated_in_threadObj

	//! Is the shortcut enabled?

	bool enabled(IN_THREAD_ONLY)
	{
		bool enabled=false;

		container_impl->invoke_layoutmanager
			([&, this]
			 (const ref<textlistlayoutmanagerObj::implObj> &l_impl)
			 {
				 auto impl=l_impl->textlist_element->impl;

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
		textlistlayoutmanagerptr lm;

		container_impl->invoke_layoutmanager
			([&]
			 (const auto &impl)
			 {
				 lm=impl->create_public_object();
			 });

		if (!lm)
			return;

		auto impl=lm->impl->textlist_element->impl;

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

void extra_list_row_infoObj::default_status_change_callback(list_lock &, size_t,
							    bool)
{
}

bool extra_list_row_infoObj::enabled() const
{
	return row_type == list_row_type_t::enabled;
}

void extra_list_row_infoObj::set_shortcut(const textlistlayoutmanager &lm,
					  const shortcut &sc)
{
	auto extra=ref(this);

	if (!sc)
	{
		if (current_shortcut)
		{
			current_shortcut->uninstall_shortcut();
			current_shortcut=nullptr;
		}
		return;
	}

	if (!current_shortcut)
		current_shortcut=shortcut_impl::create(lm, extra);

	// Our destructor explicitly calls uninstall_shortcut().
	current_shortcut->install_shortcut(sc, current_shortcut);
}

LIBCXXW_NAMESPACE_END
