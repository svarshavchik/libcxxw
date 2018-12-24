/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup_imagebutton.H"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_comboboxlayoutmanager.H"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listcontainer_pseudo_impl.H"

#include "x/w/impl/focus/focusable_element.H"

#include "x/w/focusable_container.H"
#include "x/w/key_event.H"
#include "x/w/synchronized_axis.H"
#include "busy.H"
#include "capturefactory.H"
#include "run_as.H"

#include <x/weakcapture.H>
#include <x/visitor.H>
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

custom_comboboxlayoutmanagerObj
::custom_comboboxlayoutmanagerObj(const ref<implObj> &impl,
				  const ref<listlayoutmanagerObj::implObj>
				  &list_layout_impl)
	// Surprise! Our implementation object is attached to the combobox
	// container, but our superclass is a listlayoutmanager whose
	// implementation object is the popup's listlayoutmanager implementation
	// object.
	//
	// It is expected that most of the activity here will impact the
	// combobox popup, so naturally when we go out of scope and get
	// destroyed it'll be the popup's container that'll get tickled for
	// recalculation.
	: listlayoutmanagerObj(list_layout_impl),
	  impl(impl)
{
}

custom_comboboxlayoutmanagerObj::~custom_comboboxlayoutmanagerObj()=default;

element custom_comboboxlayoutmanagerObj::current_selection()
{
	// The current selection element is always position (0, 0)
	// in the internally-managed grid.

	return impl->get(0, 0);
}

const_element custom_comboboxlayoutmanagerObj::current_selection() const
{
	// The current selection element is always position (0, 0)
	// in the internally-managed grid.

	return impl->get(0, 0);
}

/////////////////////////////////////////////////////////////////////////////
//
// Internal object that collects keystrokes when the input focus is on the
// current selection element, for the lookup callback.

class LIBCXX_HIDDEN lookup_collectorObj : virtual public obj {

 public:

	std::u32string buffer;

	lookup_collectorObj()=default;
	~lookup_collectorObj()=default;

	// Called to process a key event.

	// Collect the key event into the buffer, and invoke the
	// combo-box's search callback.
	//
	// This is always invoked from the connection thread.

	inline bool process(ONLY IN_THREAD,
			    const all_key_events_t &e,
			    bool activated,
			    const custom_combobox_selection_search_t
			    &search_func,
			    bool selection_required,
			    const element &current_selection,
			    const custom_comboboxlayoutmanager &lm,
			    const busy &mcguffin)
	{
		size_t i=0;

		list_lock lock{lm};

		if (!std::visit(visitor{
		    [&](const key_event *ke)
		    {
			if (!ke->notspecial())
				return false;

			switch (ke->keysym) {
			case XK_Delete:
			case XK_KP_Delete:
				if (activated)
					buffer.clear();
				break;
			case XK_Up:
			case XK_KP_Up:
				if (activated)
				{
					auto selected=lm->selected();

					i=selected ? *selected:lm->size();

					while (i)
					{
						--i;
						if (!lm->enabled(i))
							continue;

						lm->autoselect(i);
						break;
					}
				}
				buffer.clear();
				// Set "activated" to false in order to bail
				// out, below, instead of calling search_func().
				// We handled everything here.
				activated=false;
				return true;
			case XK_Down:
			case XK_KP_Down:
				if (activated)
				{
					auto selected=lm->selected();

					i=selected ? *selected + 1 : 0;

					auto n=lm->size();

					while (i<n)
					{
						if (lm->enabled(i))
						{
							lm->autoselect(i);
							break;
						}
						++i;
					}
				}
				buffer.clear();
				// Set "activated" to false in order to bail
				// out, below, instead of calling search_func().
				// We handled everything here.
				activated=false;
				return true;
			default:

				if (!ke->unicode)
					return false;

				if (ke->unicode == '\n')
				{
					if (!activated)
						break;
					// Get the current selection, and
					// start the search on the next list item.

					auto selected=lm->selected();

					if (selected)
					{
						i=selected.value();
						++i;
					}
				}
				else
				{
					if (ke->unicode < ' ')
					{
						return false;
					}
					if (!activated)
						break;
					buffer.push_back(ke->unicode);
				}
			}
			return true;
		    },
		    [&](const std::u32string_view *str)
		    {
			    buffer += *str;
			    return true;
		    },
		    [](const all_key_events_is_not_copyable &)
		    {
			    return true;
		    }}, e))
		{
			return false;
		}

		if (!activated)
			return true;

		search_func(IN_THREAD,
			    custom_combobox_selection_search_info_t
			    {lock, lm, buffer, i, current_selection,
					    selection_required,
					    mcguffin});

		return true;
	}
};

typedef ref<lookup_collectorObj> lookup_collector;

/////////////////////////////////////////////////////////////////////////////

static custom_combobox_selection_changed_t noop_selection_changed=
	[]
	(THREAD_CALLBACK, const auto &ignore) {
};

static custom_combobox_selection_search_t noop_selection_search=
	[]
	(THREAD_CALLBACK, const auto &ignore)
{
};

new_custom_comboboxlayoutmanager
::new_custom_comboboxlayoutmanager(const custom_combobox_selection_factory_t
				   &selection_factory)
	: selection_factory{selection_factory},
	  selection_changed{noop_selection_changed},
	  selection_search{noop_selection_search},
	  synchronized_columns{synchronized_axis::create()}
{
}

new_custom_comboboxlayoutmanager::~new_custom_comboboxlayoutmanager()=default;

custom_combobox_selection_changed_t
new_custom_comboboxlayoutmanager::get_selection_changed() const
{
	return selection_changed;
}

new_listlayoutmanager combobox_new_listlayoutmanager(bool selection_required)
{
	new_listlayoutmanager style{combobox_list};

	style.background_color="combobox_background_color";
	style.selected_color="combobox_selected_color";
	style.highlighted_color="combobox_highlighted_color";
	style.current_color="combobox_current_color";

	if (!selection_required)
		style.selection_type=single_optional_selection_type;

	// Close the popup when a selection is made. We hook the
	// selection type callback.

	auto combobox_selection_type=style.selection_type;

	style.selection_type=
		[combobox_selection_type]
		(ONLY IN_THREAD,
		 const listlayoutmanager &ll,
		 size_t i,
		 const callback_trigger_t &trigger,
		 const busy &mcguffin)
		{
			elementObj::implObj &e=
				*ll->impl->container_impl;

			e.get_window_handler()
				.request_visibility(IN_THREAD, false);
			combobox_selection_type(IN_THREAD, ll, i, trigger,
						mcguffin);
		};

	return style;
}

create_peepholed_toplevel_listcontainer_popup_args
combobox_listcontainer_popup_args(const element_impl &parent_element,
				  const new_listlayoutmanager &style,
				  unsigned nesting_level)
{
	return {
		parent_element,

		"combo,popup_menu,dropdown_menu",
		"combobox",
		"combobox_popup_border",

		nesting_level,
		attached_to::combobox_above_or_below,
		exclusive_popup_type,
		style,
		"combobox_above_background_color",
		"combobox_below_background_color"
	};
}

focusable_container new_custom_comboboxlayoutmanager
::create(const container_impl &parent) const
{
	// Start by creating the popup first.

	new_listlayoutmanager style=
		combobox_new_listlayoutmanager(selection_required);
	style.synchronized_columns=synchronized_columns;

	custom_combobox_popup_containerptr popup_containerptr;

	auto [combobox_popup, popup_handler]=
		create_peepholed_toplevel_listcontainer_popup
		(combobox_listcontainer_popup_args
		 (ref(&parent->container_element_impl()), style,
		  // We're about to create the combobox container,
		  // with nesting_level of parent+1
		  //
		  // The current selection element in the combox
		  // container will be parent+2.
		  //
		  // Need to set the popup's nesting level to
		  // parent+3, so that it gets recalculated after
		  // the popup gets recalculated.

		  3),
		 [&]
		(const auto &peephole_container,
		 const popup_attachedto_info &attachedto_info)
			->create_popup_factory_ret_t
		 {
			 auto impl=ref<custom_combobox_popup_containerObj
				       ::implObj>::create(peephole_container);

			 auto textlist_impl=ref<list_elementObj::implObj>
				 ::create(list_element_impl_init_args
					  {
					   impl, style,
					   style.synchronized_columns
					  });

			 auto lm=ref<
				 peepholed_toplevel_listcontainer_layoutmanager_implObj
				 >::create(impl, impl,
					   list_element::create(textlist_impl));

			 auto container=custom_combobox_popup_container
			 ::create(impl,
				  lm,
				  attachedto_info);

			 popup_containerptr=container;

			 return {container, container};
		 },

		create_p_t_l_handler);

        custom_combobox_popup_container popup_container=popup_containerptr;
	ref<listlayoutmanagerObj::implObj>
		popup_listlayoutmanager=combobox_popup->get_layout_impl();

	// We can now start creating the combobox display element, starting
	// with the container where everything goes.
	auto combobox_container_impl=
		ref<custom_combobox_containerObj::implObj>
		::create(parent, popup_container,
			 popup_handler);

	// And the layout manager.
	auto lm=create_impl({combobox_container_impl});

	auto glm=lm->create_gridlayoutmanager();

	// The combo-box's internal grid layout manager will have one row
	// with the current selection element, and the button element.
	auto f=glm->append_row();

	// Invoke the callback to construct the current selection element,
	// which is automatically show()n.

	auto capture_current_selection=
		capturefactory::create(combobox_container_impl);

	auto focusable_selection=selection_factory(capture_current_selection);

	auto current_selection=capture_current_selection->get();

	current_selection->impl
		->set_background_color("combobox_background_color");

	current_selection->show_all();

	f->padding(0);
	f->border("combobox_border");
	f->valign(valign::middle);

	f->created_internally(current_selection);

	auto combobox_button=create_popup_imagebutton
		(f,
		 []
		 (const border_arg &focusoff_border,
		  const border_arg &focuson_border,
		  const container_impl &parent_container,
		  const child_element_init_params &init_params)
		 {
			 return ref<popup_imagebutton_focusframe_implObj>
			 ::create(focusoff_border,
				  focuson_border,
				  0,
				  0,
				  parent_container,
				  parent_container,
				  init_params);
		 },

		 popup_handler,

		 popup_imagebutton_config{"combobox_border",
				"combobox_background_color",
				"scroll-down1",
				"scroll-down2",
				"comboboxbuttonfocusoff_border",
				"comboboxbuttonfocuson_border"
		});

	// Point the popup container to the current selection element and
	// the combo-box button element, so both can be sized based on the
	// size of the combo-box's items.
	popup_container->impl
		->set_current_combobox_selection_element_and_button
		(current_selection);

	auto c=custom_combobox_container::create(combobox_container_impl,
						 lm,
						 focusable_selection,
						 combobox_button,
						 combobox_popup);


	// Install The combo-box popup's list layout manager
	// selection_changed callback, to forward to the
	// combo-box selection_changed callback (must be
	// weakly-captured to avoid a circular reference).

	{
		listimpl_info_t::lock lock{
			popup_listlayoutmanager->list_element_singleton
				->impl->textlist_info};

		lock->selection_changed=
			[=, current_selection=make_weak_capture
			 (current_selection, combobox_popup, lm)]
			(ONLY IN_THREAD, const auto &info)
			{
				auto got=current_selection.get();

				if (!got)
					return;

				auto &[e, combobox_popup, lm]=*got;

				// The busy mcguffin in info is the busy
				// mcguffin for the popup window. The callback
				// would probably want to install the busy
				// mcguffin for the window that
				// contains the combo-box.
				busy_impl yes_i_am{*e->impl};

				lm->selection_changed.get()
				(IN_THREAD,
				 custom_combobox_selection_changed_info_t{
					lm->create_public_object(),
						e,
						combobox_popup,
						info,
						yes_i_am});
			};
	}

	auto collector=lookup_collector::create();

	c->elementObj::impl->THREAD->run_as
		([=,
		  selection_required=this->selection_required,
		  selection_search=this->selection_search]
		 (ONLY IN_THREAD)
		 {
			 // Install:

			 // 1. The popup handler as the combobox container's
			 // attached_popup.

			 combobox_container_impl->data(IN_THREAD)
				 .attached_popup_impl=popup_handler;


			 auto &focusable_element=focusable_selection
				 ->get_impl()->get_focusable_element();

			 // Clear the search string collector buffer when
			 // the current selection focusable gains/loses
			 // input focus (the search starts afresh).

			 focusable_element
				 .on_keyboard_focus
				 (IN_THREAD,
				  [collector]
				  (THREAD_CALLBACK,
				   const auto &ignore,
				   const auto &ignore2)
				  {
					  collector->buffer.clear();
				  });

			 // Install an on_key_event, to collect the typed in
			 // text, and trigger a combo-box search.
			 focusable_element.on_key_event
				 (IN_THREAD,
				  [collector,
				   selection_search,
				   selection_required,
				   c=make_weak_capture
				   (current_selection, lm)]
				  (ONLY IN_THREAD,
				   const auto &key_event,
				   bool activated,
				   const auto &mcguffin)
				  {
					  bool processed=false;

					  auto got=c.get();

					  if (got)
					  {
						  auto &[current_selection,
							 lm]= *got;

						  processed=collector->process
							  (IN_THREAD,
							   key_event,
							   activated,
							   selection_search,
							   selection_required,
							   current_selection,
							   lm->create_public_object(),
							   mcguffin);
					  }
					  return processed;
				  });
		 });

	return c;
}

void custom_comboboxlayoutmanagerObj
::selection_changed(const custom_combobox_selection_changed_t &cb)
{
	impl->selection_changed=cb;
}

ref<custom_comboboxlayoutmanagerObj::implObj
    > new_custom_comboboxlayoutmanager::create_impl(const create_impl_info &i)
	const
{
	return ref<custom_comboboxlayoutmanagerObj::implObj>
		::create(i.container_impl, *this);
}

LIBCXXW_NAMESPACE_END
