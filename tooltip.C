/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "element_screen.H"
#include "screen.H"
#include "connection_thread.H"
#include "generic_window_handler.H"
#include "popup/popup_handler.H"
#include "popup/popup_impl.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/label.H"
#include "x/w/text_param.H"
#include "x/w/tooltip.H"
#include "x/w/pictformat.H"
#include "gridlayoutmanager.H"
#include "grabbed_pointer.H"
#include "defaulttheme.H"
#include "x/w/gridfactory.H"
#include "background_color.H"
#include <x/property_value.H>

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
tooltip_delay(LIBCXX_NAMESPACE_STR "::w::tooltip_delay", 2000);

namespace {
#if 0
}
#endif

//! Subclass popupObj::handlerObj for a tooltip window.

//! Implements recalculate_popup_position() by position the popup next to
//! the pointer.

class LIBCXX_HIDDEN tooltip_handlerObj : public popupObj::handlerObj {

	//! typedef alias
	typedef popupObj::handlerObj superclass_t;

 public:
	//! Original pointer coordinate that tooltip position is based on.
	const coord_t pointer_x;

	//! Original pointer coordinate that tooltip position is based on.
	const coord_t pointer_y;

	//! The pointer is actively grabbed while the tooltip is shown.
	const ref<obj> grab;

	//! Constructor
	tooltip_handlerObj(IN_THREAD_ONLY,
			   const ref<generic_windowObj::handlerObj> &parent,
			   const color_arg &background_color,
			   const ref<obj> &grab,
			   coord_t pointer_x,
			   coord_t pointer_y);

	//! Destructor
	~tooltip_handlerObj();

	//! Implement recalculate_popup_position()
	popup_position_affinity recalculate_popup_position(IN_THREAD_ONLY,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override;

	const char *label_theme_font() const override
	{
		return "tooltip";
	}


	//! Override default_wm_class_instance()

	//! Returns "tooltip".

	const char *default_wm_class_instance() const override
	{
		return "tooltip";
	}

	ref<obj> get_opened_mcguffin(IN_THREAD_ONLY) override
	{
		return ref<obj>::create(); // Dummy stub.
	}

	void released_opened_mcguffin(IN_THREAD_ONLY) override
	{
		// Dummy stub
	}

#ifdef TOOLTIP_HANDLER_EXTRA_METHODS
	TOOLTIP_HANDLER_EXTRA_METHODS
#endif
};

tooltip_handlerObj::tooltip_handlerObj(IN_THREAD_ONLY,
				       const ref<generic_windowObj::handlerObj>
				       &parent,
				       const color_arg &background_color,
				       const ref<obj> &grab,
				       coord_t pointer_x,
				       coord_t pointer_y)
	: superclass_t(IN_THREAD, parent, background_color),
	pointer_x(pointer_x),
	pointer_y(pointer_y),
	grab(grab)
{
	wm_class_resource(IN_THREAD)=parent->wm_class_resource(IN_THREAD);
}

tooltip_handlerObj::~tooltip_handlerObj()=default;

popup_position_affinity
tooltip_handlerObj::recalculate_popup_position(IN_THREAD_ONLY,
					       rectangle &r,
					       dim_t screen_width,
					       dim_t screen_height)
{
	auto s=get_screen()->impl;

	auto current_theme=s->current_theme.get();

	dim_t offset_x=current_theme->get_theme_width_dim_t("tooltip_x_offset");
	dim_t offset_y=current_theme->get_theme_height_dim_t("tooltip_y_offset");

	coord_t x=coord_t::truncate(pointer_x+offset_x);

	coord_t y=coord_t::truncate(coord_t{
			coord_t::truncate(pointer_y-offset_y)
				}-r.height);

	if (y < 0)
		y=coord_t::truncate(pointer_y+offset_y);

	auto a=popup_position_affinity::right;

	if (dim_t::truncate(x + r.width) > screen_width)
	{
		x=coord_t::truncate(coord_t{
				coord_t::truncate(x - offset_x)
					} - r.width);
		a=popup_position_affinity::left;
	}
	r.x=x;
	r.y=y;

	return a;
}

class LIBCXX_HIDDEN tooltip_factory_impl : public tooltip_factory {

	IN_THREAD_ONLY;

	const ref<elementObj::implObj> parent_element;

 public:
	tooltip_factory_impl(IN_THREAD_ONLY,
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

	// Actively grab the pointer before the tooltip is shown.
	auto grab=parent_element->grab_pointer(IN_THREAD);

	if (!grab)
		return; // Didn't grab the pointer.

	auto popup_handler=ref<tooltip_handlerObj>::create
		(IN_THREAD, parent_window, "transparent",
		 grab,
		 coord_t::truncate(parent_element->data(IN_THREAD)
				   .last_motion_x
				   + parent_element_absolute_location.x),
		 coord_t::truncate(parent_element->data(IN_THREAD)
				   .last_motion_y
				   + parent_element_absolute_location.y));

	auto popup_impl=ref<popupObj::implObj>::create(popup_handler,
						       parent_window);



	auto tooltip_popup=popup::create(popup_impl,
					 ref<gridlayoutmanagerObj::implObj>
					 ::create(popup_handler));

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

	// Now that the tooltip is visible, allow pointer events going
	// forward.
	grab->allow_events(IN_THREAD);
}
#if 0
{
#endif
}

///////////////////////////////////////////////////////////////////////////

void elementObj::create_tooltip(const text_param &text)
{
	create_custom_tooltip
		([=]
		 (const auto &factory)
		 {
			 factory([&]
				 (const auto &c)
				 {
					 gridlayoutmanager l=
						 c->get_layoutmanager();

					 auto f=l->append_row();
					 f->padding(0);
					 f->create_label(text);
				 },
				 new_gridlayoutmanager());
		 });
}

void elementObj::create_tooltip(const text_param &text,
				double width)
{
	create_custom_tooltip
		([=]
		 (const auto &factory)
		 {
			 factory([&]
				 (const auto &c)
				 {
					 gridlayoutmanager l=
						 c->get_layoutmanager();

					 auto f=l->append_row();
					 f->padding(0);
					 f->create_label(text, width);
				 },
				 new_gridlayoutmanager());
		 });
}

std::chrono::milliseconds
elementObj::implObj::hover_action_delay(IN_THREAD_ONLY)
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

void elementObj::implObj::hover_action(IN_THREAD_ONLY)
{
	tooltip_factory_impl create_a_tooltip(IN_THREAD, ref(this));

	data(IN_THREAD).tooltip_factory(create_a_tooltip);
}

void elementObj::implObj::hover_cancel(IN_THREAD_ONLY)
{
	// Cancel the pending tooltip. If it's already shown, discard it.
	// And discard the timer's mcguffin, if we were getting set to do it.

	data(IN_THREAD).tooltip=nullptr;

}

LIBCXXW_NAMESPACE_END
