/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen.H"
#include "connection_thread.H"
#include "generic_window_handler.H"
#include "popup/popup_handler.H"
#include "popup/popup_attachedto_info.H"
#include "popup/popup_impl.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/text_param.H"
#include "x/w/tooltip.H"
#include "x/w/pictformat.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/impl/borderlayoutmanager.H"
#include "defaulttheme.H"
#include "x/w/gridfactory.H"
#include "x/w/impl/background_color.H"
#include <x/property_value.H>
#include "messages.H"

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
tooltip_delay(LIBCXX_NAMESPACE_STR "::w::tooltip_delay", 2000);

void tooltip_border::set_theme_border(const std::string &border)
{
	this->border=border;
	this->hpad=border + "_padding_h";
	this->vpad=border + "_padding_v";
}

tooltip_border tooltip_factory::default_alpha_border()
{
	tooltip_border b;

	b.set_theme_border("tooltip_border");

	return b;
}

tooltip_border tooltip_factory::default_nonalpha_border()
{
	tooltip_border b;

	b.set_theme_border("tooltip_border_square");
	return b;
}

namespace {
#if 0
}
#endif

//! Subclass popupObj::handlerObj for a tooltip window.

class LIBCXX_HIDDEN tooltip_handlerObj :
	public bordercontainer_elementObj<popupObj::handlerObj> {

	//! typedef alias
	typedef bordercontainer_elementObj<popupObj::handlerObj> superclass_t;

 public:
	//! Constructor
	tooltip_handlerObj(ONLY IN_THREAD,
			   const border_arg &tooltip_border,
			   const dim_arg &tooltip_border_hpad,
			   const dim_arg &tooltip_border_vpad,
			   const ref<generic_windowObj::handlerObj> &parent,
			   const rectangle &where,
			   attached_to how);

	//! Destructor
	~tooltip_handlerObj();

	const char *label_theme_font() const override
	{
		return "tooltip";
	}

	void set_default_wm_hints(ONLY IN_THREAD,
				  xcb_icccm_wm_hints_t &hints) override
	{
		// No default input flag.
	}

	// We will go away as soon as a key is pressed

	bool popup_accepts_key_events(ONLY IN_THREAD) override
	{
		return false;
	}

	// We will go away as soon as a key is pressed

	bool popup_accepts_button_events(ONLY IN_THREAD) override
	{
		return false;
	}

	void creating_focusable_element() override
	{
		throw EXCEPTION(_("Focusable display elements cannot "
				  "be created in a tooltip"));
	}

#ifdef TOOLTIP_HANDLER_EXTRA_METHODS
	TOOLTIP_HANDLER_EXTRA_METHODS
#endif
};

tooltip_handlerObj::tooltip_handlerObj(ONLY IN_THREAD,
				       const border_arg &tooltip_border,
				       const dim_arg &tooltip_border_hpad,
				       const dim_arg &tooltip_border_vpad,
				       const ref<generic_windowObj::handlerObj>
				       &parent,
				       const rectangle &where,
				       attached_to how)
	: superclass_t{*parent,
		       tooltip_border, tooltip_border,
		       tooltip_border, tooltip_border,
		       richtextptr{},
		       0,
		       tooltip_border_hpad,
		       tooltip_border_vpad,
		       popup_handler_args
		       {
			exclusive_popup_type,
			"tooltip",
			parent,
			popup_attachedto_info::create(where, how),
			0,
			"tooltip,popup_menu,dropdown_menu",
			"",
		       }}
{
	wm_class_resource(IN_THREAD)=parent->wm_class_resource(IN_THREAD);
}

tooltip_handlerObj::~tooltip_handlerObj()=default;

//! Implement tooltip_factory's create().

//! Common logic shared by regular and static tooltip creators.
//!
//! A subclass is responsible for implementing create_tooltip_handler() to
//! give us a popup handler object as a starting point, and implementing
//! created_popup(), which gets called before create() returns.
//!
//! Regular and static tooltips construct the popup handler slightly
//! differently. created_popup() is also responsible for installing
//! the new popup into parent_element's data.attached_popup.

class LIBCXX_HIDDEN tooltip_factory_impl : public tooltip_factory {

 protected:
	const ref<elementObj::implObj> parent_element;

 public:
	tooltip_factory_impl(const ref<elementObj::implObj> &parent_element)
		: parent_element(parent_element)
	{
	}

	~tooltip_factory_impl()=default;

	void create(const function<void (const container &)> &creator,
		    const new_layoutmanager &layout_manager,
		    const tooltip_border &alpha_border,
		    const tooltip_border &nonalpha_border) const override;

	virtual ref<tooltip_handlerObj>
		create_tooltip_handler(const ref<generic_windowObj::handlerObj>
				       &parent_window,
				       const border_arg &tooltip_border,
				       const dim_arg &tooltip_border_hpad,
				       const dim_arg &tooltip_border_vpad)
		const=0;

	virtual void created_popup(const popup &) const=0;
};

void tooltip_factory_impl::create(const function<void (const container &)>
				  &creator,
				  const new_layoutmanager &layout_manager,
				  const tooltip_border &alpha_border,
				  const tooltip_border &nonalpha_border)
	const
{
	ref<generic_windowObj::handlerObj>
		parent_window{&parent_element->get_window_handler()};

	const auto &border=
		parent_window->drawable_pictformat->alpha_depth > 0
		? alpha_border:nonalpha_border;

	auto popup_handler=create_tooltip_handler(parent_window,
						  border.border,
						  border.hpad,
						  border.vpad);

	auto popup_impl=ref<popupObj::implObj>::create(popup_handler,
						       parent_window);

	popupptr tooltip_popup;

	layout_manager.create
		(popup_handler,
		 make_function<void (const container &c)>
		 ([&]
		  (const container &c)
		  {
			  c->set_background_color("tooltip_background_color");
			  auto real_container_impl=c->get_layout_impl();

			  auto border_layout_impl=
				  ref<borderlayoutmanagerObj::implObj>::create
				  (popup_handler, popup_handler,
				   c,
				   halign::fill,
				   valign::fill);

			  auto p=popup::create(popup_impl,
					       border_layout_impl,
					       real_container_impl);

			  creator(p);
			  tooltip_popup=p;
		  }));

	created_popup(tooltip_popup);
}

//! Implement tooltip_factory_impl for regular popups.

//! Implements create_tooltip_handler() and created_popup() for regular
//! popups.

class LIBCXX_HIDDEN popup_tooltip_factory :
	public tooltip_factory_impl {

 protected:
	ONLY IN_THREAD;

 public:
	popup_tooltip_factory(ONLY IN_THREAD,
			      const ref<elementObj::implObj> &parent_element)
		: tooltip_factory_impl{parent_element},
		IN_THREAD{IN_THREAD}
		{
		}

	 ~popup_tooltip_factory()=default;

	ref<tooltip_handlerObj>
		create_tooltip_handler(const ref<generic_windowObj::handlerObj>
				       &parent_window,
				       const border_arg &tooltip_border,
				       const dim_arg &tooltip_border_hpad,
				       const dim_arg &tooltip_border_vpad)
		const override;

	void created_popup(const popup &tooltip_popup) const override
	{
		parent_element->data(IN_THREAD).attached_popup=tooltip_popup;

		tooltip_popup->show_all();
	}
};

ref<tooltip_handlerObj>
popup_tooltip_factory::create_tooltip_handler
(const ref<generic_windowObj::handlerObj> &parent_window,
 const border_arg &tooltip_border,
 const dim_arg &tooltip_border_hpad,
 const dim_arg &tooltip_border_vpad) const
{
	// Compute the current pointer coordinates.

	auto parent_element_absolute_location=
		parent_element->get_absolute_location_on_screen(IN_THREAD);

	coord_t x{coord_t::truncate(parent_element->data(IN_THREAD)
				    .last_motion_x
				    + parent_element_absolute_location.x)};
	coord_t y{coord_t::truncate(parent_element->data(IN_THREAD)
				    .last_motion_y
				    + parent_element_absolute_location.y)};

	auto current_theme=
		parent_window->get_screen()->impl->current_theme.get();

	dim_t offset_x=current_theme->get_theme_dim_t("tooltip_x_offset",
						      themedimaxis::width);
	dim_t offset_y=current_theme->get_theme_dim_t("tooltip_y_offset",
						      themedimaxis::height);


	// And construct an attached_to::tooltip tooltip_handlerObj,
	// at these coordinates.
	return ref<tooltip_handlerObj>::create
		(IN_THREAD,
		 tooltip_border,
		 tooltip_border_hpad,
		 tooltip_border_vpad,
		 parent_window,
		 rectangle{coord_t::truncate(x+offset_x),
				coord_t::truncate(y-offset_y),
				   0, 0},
		 attached_to::tooltip);
}

#if 0
{
#endif
}

static_tooltip_config::~static_tooltip_config()=default;

///////////////////////////////////////////////////////////////////////////

void elementObj::create_tooltip(const text_param &text)
{
	create_tooltip(text, {});
}

void elementObj::create_tooltip(const text_param &text,
				const label_config &config)
{
	create_custom_tooltip
		([=]
		 (THREAD_CALLBACK, const auto &factory)
		 {
			 factory([&]
				 (const auto &c)
				 {
					 gridlayoutmanager l=
						 c->get_layoutmanager();

					 auto f=l->append_row();
					 f->padding(0);
					 f->create_label(text, config);
				 },
				 new_gridlayoutmanager());
		 });
}

std::chrono::milliseconds
elementObj::implObj::hover_action_delay(ONLY IN_THREAD)
{
	auto &d=data(IN_THREAD);

	if (!d.tooltip_factory)
		// No tooltip for this element.
		return std::chrono::milliseconds{0};

	if (d.attached_popup)
		// Tooltip already created.
		return std::chrono::milliseconds{0};

	return std::chrono::milliseconds(tooltip_delay.get());
}

void elementObj::implObj::hover_action(ONLY IN_THREAD)
{
	if (!data(IN_THREAD).tooltip_factory)
		return;

	popup_tooltip_factory create_a_tooltip(IN_THREAD, ref(this));

	data(IN_THREAD).tooltip_factory(IN_THREAD, create_a_tooltip);
}

void elementObj::implObj::hover_cancel(ONLY IN_THREAD)
{
	if (!data(IN_THREAD).attached_popup ||
	    data(IN_THREAD).attached_popup->impl->handler->attachedto_info->how
	    != attached_to::tooltip)
		return;
	data(IN_THREAD).attached_popup=nullptr;

}

//////////////////////////////////////////////////////////////////////////////

namespace {
#if 0
}
#endif

class LIBCXX_HIDDEN static_popup_tooltip_factory :
	public tooltip_factory_impl {

 public:
	const static_tooltip_config &config;

	const rectangle parent_element_position;

	static_popup_tooltip_factory(const ref<elementObj::implObj>
				     &parent_element,
				     const static_tooltip_config &config,
				     const rectangle &parent_element_position,
				     popupptr &created_tooltip_popup);

	ref<tooltip_handlerObj>
		create_tooltip_handler(const ref<generic_windowObj::handlerObj>
				       &parent_window,
				       const border_arg &tooltip_border,
				       const dim_arg &tooltip_border_hpad,
				       const dim_arg &tooltip_border_vpad)
		const override;

	popupptr &created_tooltip_popup;

	void created_popup(const popup &tooltip_popup) const override
	{
		created_tooltip_popup=tooltip_popup;
	}
};

static_popup_tooltip_factory::static_popup_tooltip_factory
(const ref<elementObj::implObj> &parent_element,
 const static_tooltip_config &config,
 const rectangle &parent_element_position,
 popupptr &created_tooltip_popup)
	: tooltip_factory_impl{parent_element},
	  config{config},
	  parent_element_position{parent_element_position},
	  created_tooltip_popup{created_tooltip_popup}
{
}

ref<tooltip_handlerObj>
static_popup_tooltip_factory
::create_tooltip_handler(const ref<generic_windowObj::handlerObj>
			 &parent_window,
			 const border_arg &tooltip_border,
			 const dim_arg &tooltip_border_hpad,
			 const dim_arg &tooltip_border_vpad) const
{
	return ref<tooltip_handlerObj>::create
		(parent_window->thread(),
		 tooltip_border,
		 tooltip_border_hpad,
		 tooltip_border_vpad,
		 parent_window,
		 parent_element_position,
		 config.affinity);
}

#if 0
{
#endif
}

container elementObj::do_create_static_tooltip(const function<void
					       (const container &)> &creator)
{
	return do_create_static_tooltip(creator, new_gridlayoutmanager{});
}

container elementObj::do_create_static_tooltip(const function<void
					       (const container &)> &creator,
					       const new_layoutmanager &nlm)
{
	return do_create_static_tooltip(creator, nlm, {});
}

container elementObj::do_create_static_tooltip(const function<void
					       (const container &)> &creator,
					       const new_layoutmanager &nlm,
					       const static_tooltip_config
					       &config)
{
	popupptr created_tooltip_popup;

	static_popup_tooltip_factory factory{impl, config,
					     rectangle{},
					     created_tooltip_popup};

	factory.create(creator, nlm,
		       config.alpha_border,
		       config.nonalpha_border);

	popup p{created_tooltip_popup};

	impl->get_window_handler().thread()->run_as
		([impl=this->impl, p]
		 (ONLY IN_THREAD)
		 {
			 impl->data(IN_THREAD).attached_popup=p;
			 impl->update_attachedto_info(IN_THREAD);
		 });

	return p;
}

container elementObj::do_create_static_tooltip(ONLY IN_THREAD,
					       const function<void
					       (const container &)> &creator)
{
	return do_create_static_tooltip(IN_THREAD, creator,
					new_gridlayoutmanager{});
}

container elementObj::do_create_static_tooltip(ONLY IN_THREAD,
					       const function<void
					       (const container &)> &creator,
					       const new_layoutmanager &nlm)
{
	return do_create_static_tooltip(IN_THREAD, creator, nlm, {});
}

container elementObj::do_create_static_tooltip(ONLY IN_THREAD,
					       const function<void
					       (const container &)> &creator,
					       const new_layoutmanager &nlm,
					       const static_tooltip_config
					       &config)
{
	popupptr created_tooltip_popup;

	static_popup_tooltip_factory
		factory{impl, config,
			impl->get_absolute_location_on_screen(IN_THREAD),
			created_tooltip_popup};

	factory.create(creator, nlm,
		       config.alpha_border,
		       config.nonalpha_border);

	popup p{created_tooltip_popup};

	impl->data(IN_THREAD).attached_popup=p;

	return p;
}

LIBCXXW_NAMESPACE_END
