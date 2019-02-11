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
#include "gridlayoutmanager.H"
#include "defaulttheme.H"
#include "x/w/gridfactory.H"
#include "x/w/impl/background_color.H"
#include <x/property_value.H>

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
tooltip_delay(LIBCXX_NAMESPACE_STR "::w::tooltip_delay", 2000);

namespace {
#if 0
}
#endif

//! Subclass popupObj::handlerObj for a tooltip window.

class LIBCXX_HIDDEN tooltip_handlerObj :
	public popupObj::handlerObj {

	//! typedef alias
	typedef popupObj::handlerObj superclass_t;

 public:
	//! Constructor
	tooltip_handlerObj(ONLY IN_THREAD,
			   const ref<generic_windowObj::handlerObj> &parent,
			   coord_t pointer_x,
			   coord_t pointer_y);

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

#ifdef TOOLTIP_HANDLER_EXTRA_METHODS
	TOOLTIP_HANDLER_EXTRA_METHODS
#endif
};

tooltip_handlerObj::tooltip_handlerObj(ONLY IN_THREAD,
				       const ref<generic_windowObj::handlerObj>
				       &parent,
				       coord_t pointer_x,
				       coord_t pointer_y)
	: superclass_t{popup_handler_args
		       {
			exclusive_popup_type,
			"tooltip",
			parent,
			popup_attachedto_info::create
			(rectangle{pointer_x, pointer_y, 0, 0},
			 attached_to::above_or_below),
			0}}
{
	wm_class_resource(IN_THREAD)=parent->wm_class_resource(IN_THREAD);
}

tooltip_handlerObj::~tooltip_handlerObj()=default;

class LIBCXX_HIDDEN tooltip_factory_impl : public tooltip_factory {

	ONLY IN_THREAD;

	const ref<elementObj::implObj> parent_element;

 public:
	tooltip_factory_impl(ONLY IN_THREAD,
			     const ref<elementObj::implObj> &parent_element)
		: IN_THREAD(IN_THREAD),
		parent_element(parent_element)
		{
		}

	~tooltip_factory_impl()=default;

	void create(const function<void (const container &)> &creator,
		    const new_layoutmanager &layout_manager) const override;
};

void tooltip_factory_impl::create(const function<void (const container &)>
				  &creator,
				  const new_layoutmanager &layout_manager)
	const
{
	ref<generic_windowObj::handlerObj>
		parent_window{&parent_element->get_window_handler()};

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

	auto popup_handler=ref<tooltip_handlerObj>::create
		(IN_THREAD,
		 parent_window,
		 coord_t::truncate(x+offset_x),
		 coord_t::truncate(y-offset_y));

	auto popup_impl=ref<popupObj::implObj>::create(popup_handler,
						       parent_window);

	auto tooltip_popup=popup::create(popup_impl,
					 new_gridlayoutmanager{}
					 .create(popup_handler));

	gridlayoutmanager glm=tooltip_popup->get_layoutmanager();

	auto f=glm->append_row();

	f->rounded_border_and_padding("tooltip_border");

	f->create_container([&, this]
			    (const auto &container)
			    {
				    container->set_background_color
					    ("tooltip_background_color");
				    creator(container);
			    }, layout_manager);

	parent_element->data(IN_THREAD).tooltip=tooltip_popup;

	tooltip_popup->show_all();
}
#if 0
{
#endif
}

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

	if (d.tooltip)
		// Tooltip already created.
		return std::chrono::milliseconds{0};

	return std::chrono::milliseconds(tooltip_delay.get());
}

void elementObj::implObj::hover_action(ONLY IN_THREAD)
{
	if (!data(IN_THREAD).tooltip_factory)
		return;

	tooltip_factory_impl create_a_tooltip(IN_THREAD, ref(this));

	data(IN_THREAD).tooltip_factory(IN_THREAD, create_a_tooltip);
}

void elementObj::implObj::hover_cancel(ONLY IN_THREAD)
{
	// Cancel the pending tooltip. If it's already shown, discard it.
	// And discard the timer's mcguffin, if we were getting set to do it.

	data(IN_THREAD).tooltip=nullptr;

}

LIBCXXW_NAMESPACE_END
