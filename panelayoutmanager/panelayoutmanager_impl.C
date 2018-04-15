/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "panelayoutmanager/pane_slider.H"
#include "panelayoutmanager/pane_slider_focusframe.H"
#include "panelayoutmanager/pane_slider_impl_element.H"
#include "focus/standard_focusframecontainer_element.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/focus/focusable.H"
#include "focus/focusframecontainer.H"
#include "x/w/impl/current_border_impl.H"
#include "screen.H"
#include "straight_border_impl.H"
#include "generic_window_handler.H"
#include "icon.H"
#include "cursor_pointer.H"
#include "x/w/impl/background_color.H"
#include "peephole/peephole.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "messages.H"
#include "x/w/panefactory.H"

LIBCXXW_NAMESPACE_START

panelayoutmanagerObj::implObj::implObj(const ref<panecontainer_implObj>
				       &pane_container_impl,
				       const pane_style &style)
	: gridlayoutmanagerObj::implObj{pane_container_impl},
	pane_container_impl{pane_container_impl},
	style{style}
{
}

panelayoutmanagerObj::implObj::~implObj()=default;

size_t panelayoutmanagerObj::implObj::size()
{
	grid_map_t::lock lock{grid_map};

	// Empty: slider+canvas
	//
	// One pane: pane+slider+canvas
	//
	// Two panes: pane+slider+pane+canvas;
	//
	// Three panes: pane+slider+pane+slider+pane+canvas;
	//

	auto s=total_size(lock);

	if (s <= 2)
		return 0;

	return s/2;
}

elementptr panelayoutmanagerObj::implObj::get_pane_element(size_t n)
{
	elementptr e;

	grid_map_t::lock lock{grid_map};

	if (n < size())
	{
		pane_peephole_container container=get_element(lock, n*2);

		auto peephole=container->get_peephole();

		e=peephole->get_peepholed();
	}

	return e;
}

layoutmanager panelayoutmanagerObj::implObj::create_public_object()
{
	return panelayoutmanager::create(ref(this));
}

void panelayoutmanagerObj::implObj::create_slider(const gridfactory &f)
{
	// Start with a focus frame.

	auto ff=ref<pane_slider_focusframeObj>::create
		(f->get_container_impl(),
		 pane_container_impl->container_element_impl()
		 .get_window_handler()
		 .create_icon({slider_cursor()})->create_cursor(),
		 style.slider_background_color);

	// Create the implementation object for the slider element.
	auto slider_border=pane_container_impl->container_element_impl()
		.get_screen()->impl->get_cached_border(style.slider);

	auto slider_impl=create_pane_slider_impl(ff, slider_border);

	auto slider=pane_slider::create(slider_impl);

	auto slider_container=create_focusframe_container_owner(ff, slider,
								slider_impl);

	slider_container->label_for(slider);

	initialize_factory_for_slider(f);
	f->created_internally(slider_container);
}

void panelayoutmanagerObj::implObj
::initialize_factory_for_slider(const gridfactory &f)
{
	f->padding(0);
	f->border(style.border);
}

create_pane_info_t panelayoutmanagerObj::implObj
::create_pane_peephole(panefactory_implObj &factory)
{
	panefactory_implObj::new_pane_properties_t::lock
		lock{factory.new_pane_properties};

	// Container for the peephole and its scrollbar.

	auto peephole_container_impl=ref<pane_peephole_containerObj::implObj>
		::create(pane_container_impl,
			 child_element_init_params
			 {{}, {}, lock->background_color});

	// The peephole element.

	auto peephole_impl=ref<pane_peepholeObj::implObj>
		::create(peephole_container_impl,
			 child_element_init_params
			 {{}, initial_peephole_metrics(lock->dimension)});

	return {peephole_container_impl, peephole_impl};
}

// Helper object for the pane element's peephole.

// The peephole layout manager uses the peepholed interface to access the
// element in the peephole.

class LIBCXX_HIDDEN pane_peepholed_elementObj : public peepholedObj {


 public:
	// The element in the peephole.

	const element peepholed_element;

	// The peephole implementation object.

	const ref<pane_peepholeObj::implObj> pane_peephole_impl;

	pane_peepholed_elementObj(const element &peepholed_element,
				  const ref<pane_peepholeObj::implObj>
				  &pane_peephole_impl)
		: peepholed_element{peepholed_element},
		pane_peephole_impl{pane_peephole_impl}
		{
		}

	element get_peepholed_element() override
	{
		return peepholed_element;
	}

	dim_t horizontal_increment(ONLY IN_THREAD) const override
	{
		return pane_peephole_impl->font_nominal_width(IN_THREAD);
	}

	dim_t vertical_increment(ONLY IN_THREAD) const override
	{
		return pane_peephole_impl->font_height(IN_THREAD);
	}
};

pane_peephole_container panelayoutmanagerObj::implObj
::created_pane_peephole(const panelayoutmanager &public_object,
			const create_pane_info_t &info,
			const create_pane_properties_t &properties,
			panefactoryObj &pfactory,
			const element &e,
			size_t position,
			grid_map_t::lock &lock)
{
	auto s=size();

	if (position > s)
		throw EXCEPTION(gettextmsg(_("Pane #%1% does not exist"),
					   position));

	// The players here are as follows:
	//
	// 1) Pane container, with a panelayoutmanager (a grid layout manager
	// in disguise).
	//
	// 2) info.peephole_container_impl. A child element of my
	// container_impl, represents a pane. This is a container that will
	// use the peephole_gridlayoutmanagerObj, to manage the layout of
	// a peephole with scrollbars.
	//
	// 3) info.peephole_impl. A child element of
	// info.peephole_container_impl. Uses the
	// peepholeObj::layoutmanager_implObj::scrollbarsObj layout manager.
	//
	// 4) "e" parameter, will be the peepholed element placed into the
	// info.pepehole_impl.

	auto pane_container_grid_impl=
		ref<peephole_gridlayoutmanagerObj>
		::create(info.peephole_container_impl);

	auto pane_container_grid=
		pane_container_grid_impl->create_gridlayoutmanager();

	auto pane_container_grid_factory=pane_container_grid->append_row();

	pane_container_grid_factory
		->left_padding(properties.left_padding_set)
		.right_padding(properties.right_padding_set)
		.top_padding(properties.top_padding_set)
		.bottom_padding(properties.bottom_padding_set)
		.halign(halign::fill)
		.valign(valign::fill);

	auto [style, horizontal_scrollbar_visibility,
	      vertical_scrollbar_visibility]=
		pane_peephole_style(properties.pane_scrollbar_visibility);

	style.horizontal_alignment=properties.horizontal_alignment;
	style.vertical_alignment=properties.vertical_alignment;

	auto scrollbars=
		create_peephole_scrollbars(info.peephole_container_impl,
					   std::nullopt);

	auto peepholed_element=ref<pane_peepholed_elementObj>
		::create(e, info.peephole_impl);

	auto layout_impl=ref<peepholeObj::layoutmanager_implObj::scrollbarsObj>
		::create(info.peephole_impl,
			 style,
			 peepholed_element,
			 scrollbars,
			 horizontal_scrollbar_visibility,
			 vertical_scrollbar_visibility);

	layout_impl->initialize_scrollbars();

	// Trigger recalculation by crating the public layout manager object.
	auto public_layout=layout_impl->create_public_object();

	// Ok, we can now create the container.
	auto pane=pane_peephole_container::create(info.peephole_container_impl,
						  pane_container_grid_impl);


	// Install the peephole_impl into the pane.

	pane_container_grid_factory
		->created_internally(peephole::create(info.peephole_impl,
						      layout_impl));

	// And install the scrollbars
	install_peephole_scrollbars(pane_container_grid,
				    scrollbars.vertical_scrollbar,
				    vertical_scrollbar_visibility,
				    pane_container_grid_factory,
				    scrollbars.horizontal_scrollbar,
				    horizontal_scrollbar_visibility,
				    pane_container_grid->append_row());

	// How the new pane gets inserted into the pane container depends
	// on the existing contents of the pane.

	if (s == 0)
	{
		// Very first pane. What we have now is the stub slider, and
		// the padding canvas.

		auto f=create_factory_for_pos(public_object, 0);
		initialize_factory_for_pane(f);
		f->created_internally(pane);
	} else if (s == 1)
	{
		// One existing pane. The slider is after the first one.

		if (position == 0)
		{
			// Inserting new pane #0. Insert it, insert a new
			// slider between the two panes, remove the stub
			// pane.

			auto f=create_factory_for_pos(public_object, 0);

			initialize_factory_for_pane(f);
			f->created_internally(pane);

			f=create_slider_factory(&*public_object, 1);
			create_slider(f);

			// The former sole pane had a slider after it, which
			// is now on row 3, and needs to be removed.
			remove_element(lock, 3);
		}
		else
		{
			// Adding 2nd pane, just add it after the stub slider.

			auto f=create_factory_for_pos(public_object, 2);
			initialize_factory_for_pane(f);
			f->created_internally(pane);
		}
	}
	else
	{
		// Two or more existing panes.

		if (position == 0)
		{
			auto f=create_factory_for_pos(public_object, 0);
			initialize_factory_for_pane(f);
			f->created_internally(pane);
			f=create_slider_factory(&*public_object, 1);
			create_slider(f);
		}
		else
		{
			auto f=create_slider_factory(&*public_object, position);
			create_slider(f);

			f=create_factory_for_pos(public_object, position*2);
			initialize_factory_for_pane(f);
			f->created_internally(pane);
		}
	}

	set_peephole_scrollbar_focus_order
		(scrollbars.horizontal_scrollbar,
		 scrollbars.vertical_scrollbar);
	request_extra_space_to_canvas();

	// Now, make sure that the tabbing order remains consistent.

	if (s == 0)
	{
		// First element. Use the initial slider as the anchor to
		// set the new container's tab order.

		pane->get_focus_before(get_element(lock, 1));
	}
	else if (s == 1)
	{
		// Second element.

		if (position == 0)
		{
			// New element and slider got inserted at the beginning
			// before the first pane. Use the first pane
			// as the anchor to set the new slider and the new pane.
			focusable slider=get_element(lock, 1);

			slider->get_focus_before(get_element(lock, 2));

			pane->get_focus_before(slider);
		}
		else
		{
			// New pane added after the stub slider. Easy enough.
			pane->get_focus_after(get_element(lock, 1));
		}
	}
	else
	{
		// Two or more panes.
		//
		// If the new pane was inserted at the beginning of the
		// container, use the former initial pane as the anchor to
		// set the tabbing order for the new slider and the new pane.
		if (position == 0)
		{
			focusable f=get_element(lock, 1);

			f->get_focus_before(get_element(lock, 2));

			pane->get_focus_before(f);
		}
		else
		{
			// Otherwise a new slider and pane got added. Use
			// their preceding pane as an anchor for the tabbing
			// order.

			size_t slider_pos=position*2-1;

			focusable f=get_element(lock, slider_pos);

			f->get_focus_after(get_element(lock, slider_pos-1));

			pane->get_focus_after(f);
		}
	}

	return pane;
}

void panelayoutmanagerObj::implObj
::initialize_factory_for_pane(const gridfactory &f)
{
	initialize_factory_for_slider(f); // Same border and no padding.

	// Fill new element to pane's entire width and height
	// pane_peephole_style() sets the appropriate algorithm to
	// peephole_algorithm::stretch_peephole, so the element in
	// the peephole gets stretched to the pane's height or width
	// automatically, and the peephole layout manager
	// takes care of setting the peephole's metrics to the peepholed
	// element's metrics.
	//
	// The end result is that the pane's grid layout manager sees the
	// metrics of all elements in the pane, and computes the pane's
	// fixed dimension accordingly, and then sizes all the elements the
	// same.

	f->halign(halign::fill);
	f->valign(valign::fill);
}

pane_slider_original_sizes
panelayoutmanagerObj::implObj::start_sliding(ONLY IN_THREAD,
					     const ref<elementObj::implObj>
					     &which_slider)
{
	auto ret=find_panes(which_slider);

	if (ret)
	{
		auto &[before, after] = *ret;

		return original_sizes(IN_THREAD, before->impl, after->impl);
	}

	return {};
}

void panelayoutmanagerObj::implObj::slide_start(ONLY IN_THREAD,
						const ref<elementObj::implObj>
						&which_slider)
{
	coord_t reference_height=
		coord_t::truncate(pane_container_impl->font_height(IN_THREAD));

	grid_map_t::lock lock{grid_map};

	// Simulate a slide

	auto original_sizes=start_sliding(IN_THREAD, which_slider);

	sliding(IN_THREAD, which_slider, original_sizes,
		reference_height, reference_height, 0, 0);
}

void panelayoutmanagerObj::implObj::slide_end(ONLY IN_THREAD,
					      const ref<elementObj::implObj>
					      &which_slider)
{
	coord_t reference_height=
		coord_t::truncate(pane_container_impl->font_height(IN_THREAD));

	grid_map_t::lock lock{grid_map};

	// Simulate a slide

	auto original_sizes=start_sliding(IN_THREAD, which_slider);

	sliding(IN_THREAD, which_slider, original_sizes, 0, 0,
		reference_height, reference_height);
}

std::optional<std::tuple<element, element>>
panelayoutmanagerObj::implObj::find_panes(const ref<elementObj::implObj> &s)
{
	std::optional<std::tuple<element, element>> ret;

	grid_map_t::lock lock{grid_map};

	// Must have at least two panes.

	if (size() > 1)
	{
		// Look up the slider's position.

		auto res=lookup_element(s);

		if (res)
		{
			// Found it, return the slider's neighbors.

			pane_peephole_container above=
				get_element(lock, (*res)-1);

			pane_peephole_container below=
				get_element(lock, (*res)+1);

			ret.emplace(above->get_peephole(),
				    below->get_peephole());
		}
	}
	return ret;
}

void panelayoutmanagerObj::implObj
::remove_pane(const panelayoutmanager &public_object,
	      size_t pane_number)
{
	grid_map_t::lock lock{grid_map};

	size_t s=size();

	if (pane_number >= s)
		return;

	if (s == 2)
	{
		if (pane_number == 1)
		{
			remove_element(lock, 2);
			request_extra_space_to_canvas();
			return; // Leaving the slider after the remaining pane.
		}

		remove_element(lock, 1);
		remove_element(lock, 0);

		// Need to create the stub slider.

		auto factory=create_slider_factory(&*public_object, 1);
		create_slider(factory);

		focusable f=get_element(lock, 1);
		f->get_focus_after(get_element(lock, 0));

		request_extra_space_to_canvas();
		return;
	}

	if (s == 0)
	{
		remove_element(lock, 0);
		remove_element(lock, 1);
		request_extra_space_to_canvas();
		return;
	}

	remove_element(lock, pane_number*2);
	remove_element(lock, pane_number*2-1);
	request_extra_space_to_canvas();
}

/////////////////////////////////////////////////////////////////////////
//
// Vertical panel implementation methods.

typedef panelayoutmanagerObj::implObj::vertical vertical;

template<>
gridfactory panelayoutmanagerObj::implObj::orientation<vertical>
::create_slider_factory(layoutmanagerObj *public_object,
			size_t pos)
{
	return insert_row(public_object,
			  pos == 0 ? 0: // Initial slider
			  1+(pos-1)*2 // Subsequent sliders
			  );
}

template<>
const char *panelayoutmanagerObj::implObj::orientation<vertical>
::slider_cursor() const
{
	return "slider-vert";
}

template<>
ref<pane_sliderObj::implObj>
panelayoutmanagerObj::implObj::orientation<vertical>
::create_pane_slider_impl(const container_impl &parent_container,
			  const current_border_impl &slider_border)
{
	return ref<pane_slider_impl_elementObj
		   <horizontal_straight_borderObj>>
		::create(parent_container, slider_border);
}

template<>
gridfactory panelayoutmanagerObj::implObj::orientation<vertical>
::create_factory_for_pos(const panelayoutmanager &lm, size_t pos)
{
	return insert_row(&*lm, pos);
}

template<>
size_t panelayoutmanagerObj::implObj::orientation<vertical>
::total_size(grid_map_t::lock &lock)
{
	return rows();
}

template<>
element panelayoutmanagerObj::implObj::orientation<vertical>
::get_element(grid_map_t::lock &lock, size_t n)
{
	return get(n, 0);
}

template<>
std::optional<size_t> panelayoutmanagerObj::implObj::orientation<vertical>
::lookup_element(const ref<elementObj::implObj> &e)
{
	std::optional<size_t> res;

	auto ret=lookup_row_col(e);

	if (ret)
		res=std::get<0>(*ret);

	return res;
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::remove_elements(grid_map_t::lock &lock, size_t n, size_t c)
{
	remove_rows(n, c);
}

template<>
std::tuple<peephole_style, scrollbar_visibility, scrollbar_visibility>
panelayoutmanagerObj::implObj::orientation<vertical>
::pane_peephole_style(scrollbar_visibility pane_scrollbar_visibility)
{
	peephole_style style;

	style.width_algorithm=peephole_algorithm::stretch_peephole;

	return {style, scrollbar_visibility::never,
			pane_scrollbar_visibility};
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::request_extra_space_to_canvas()
{
	remove_all_defaults();
	requested_row_height(rows()-1, 100);
}

template<>
metrics::horizvert_axi
panelayoutmanagerObj::implObj::orientation<vertical>
::initial_peephole_metrics(const dim_arg &size)
{
	auto theme=pane_container_impl->container_element_impl()
		.get_screen()->impl->current_theme.get();

	auto s=theme->get_theme_dim_t(size, themedimaxis::height);

	return { {0, 0, 0}, {s, s, s} };
}

template<>
pane_slider_original_sizes panelayoutmanagerObj::implObj::orientation<vertical>
::original_sizes(ONLY IN_THREAD,
		 const ref<elementObj::implObj> &before,
		 const ref<elementObj::implObj> &after)
{
	return {before->data(IN_THREAD).current_position.height,
			after->data(IN_THREAD).current_position.height};
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::sliding(ONLY IN_THREAD,
	  const ref<elementObj::implObj> &which_slider,
	  const pane_slider_original_sizes &original_sizes,

	  coord_t original_x,
	  coord_t original_y,

	  coord_t current_x,
	  coord_t current_y)
{
	// Figure out how to resize the panes.
	grid_map_t::lock lock{grid_map};

	auto ret=find_panes(which_slider);

	if (!ret)
		return;

	// Here are their original sizes.

	dim_t first_pane=original_sizes.first_pane;
	dim_t second_pane=original_sizes.second_pane;

	// Adjust their original size by how far the pointer traveled
	// from its grab point.

	if (current_y < original_y)
	{
		dim_t up=dim_t::truncate(original_y-current_y);

		if (up > first_pane)
			up=first_pane;

		first_pane=first_pane - up;

		second_pane=dim_t::truncate(second_pane+up);
	}
	else
	{
		dim_t down=dim_t::truncate(current_y-original_y);

		if (down > second_pane)
			down=second_pane;

		second_pane=second_pane - down;

		first_pane=dim_t::truncate(first_pane+down);
	}

	// Now, update the panes' metrics, and the grid layout manager will,
	// eventually, do the rest.

	auto &[before, after]=*ret;

	auto before_hv=before->impl->get_horizvert(IN_THREAD);
	auto after_hv=after->impl->get_horizvert(IN_THREAD);

	before_hv->set_element_metrics(IN_THREAD, before_hv->horiz,
				       {first_pane, first_pane, first_pane});
	after_hv->set_element_metrics(IN_THREAD, after_hv->horiz,
				      {second_pane, second_pane, second_pane});
}

/////////////////////////////////////////////////////////////////////////
//
// Horizontal panel implementation methods.

typedef panelayoutmanagerObj::implObj::horizontal horizontal;

template<>
gridfactory panelayoutmanagerObj::implObj::orientation<horizontal>
::create_slider_factory(layoutmanagerObj *public_object,
			size_t pos)
{
	return insert_columns(public_object,
			      0,
			      pos == 0 ? 0: // Initial slider
			      1+(pos-1)*2 // Subsequent sliders
			      );
}

template<>
const char *panelayoutmanagerObj::implObj::orientation<horizontal>
::slider_cursor() const
{
	return "slider-horiz";
}

template<>
ref<pane_sliderObj::implObj>
panelayoutmanagerObj::implObj::orientation<horizontal>
::create_pane_slider_impl(const container_impl &parent_container,
			  const current_border_impl &slider_border)
{
	return ref<pane_slider_impl_elementObj
		   <vertical_straight_borderObj>>
		::create(parent_container, slider_border);
}

template<>
gridfactory panelayoutmanagerObj::implObj::orientation<horizontal>
::create_factory_for_pos(const panelayoutmanager &lm, size_t pos)
{
	return insert_columns(&*lm, 0, pos);
}

template<>
size_t panelayoutmanagerObj::implObj::orientation<horizontal>
::total_size(grid_map_t::lock &lock)
{
	return cols(0);
}

template<>
element panelayoutmanagerObj::implObj::orientation<horizontal>
::get_element(grid_map_t::lock &lock, size_t n)
{
	return get(0, n);
}

template<>
std::optional<size_t> panelayoutmanagerObj::implObj::orientation<horizontal>
::lookup_element(const ref<elementObj::implObj> &e)
{
	std::optional<size_t> res;

	auto ret=lookup_row_col(e);

	if (ret)
		res=std::get<1>(*ret);

	return res;
}

template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::remove_elements(grid_map_t::lock &lock, size_t n, size_t c)
{
	remove(lock, 0, n, c);
}

template<>
std::tuple<peephole_style, scrollbar_visibility, scrollbar_visibility>
panelayoutmanagerObj::implObj::orientation<horizontal>
::pane_peephole_style(scrollbar_visibility pane_scrollbar_visibility)
{
	peephole_style style;

	style.height_algorithm=peephole_algorithm::stretch_peephole;
	return {style, pane_scrollbar_visibility,
			scrollbar_visibility::never};
}

template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::request_extra_space_to_canvas()
{
	remove_all_defaults();
	requested_col_width(cols(0)-1, 100);
}

template<>
metrics::horizvert_axi
panelayoutmanagerObj::implObj::orientation<horizontal>
::initial_peephole_metrics(const dim_arg &size)
{
	auto theme=pane_container_impl->container_element_impl()
		.get_screen()->impl->current_theme.get();

	auto s=theme->get_theme_dim_t(size, themedimaxis::width);

	return { {s, s, s}, {0, 0, 0} };
}

template<>
pane_slider_original_sizes panelayoutmanagerObj::implObj::orientation<horizontal>
::original_sizes(ONLY IN_THREAD,
		 const ref<elementObj::implObj> &before,
		 const ref<elementObj::implObj> &after)
{
	return {before->data(IN_THREAD).current_position.width,
			after->data(IN_THREAD).current_position.width};
}

template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::sliding(ONLY IN_THREAD,
	  const ref<elementObj::implObj> &which_slider,
	  const pane_slider_original_sizes &original_sizes,

	  coord_t original_x,
	  coord_t original_y,

	  coord_t current_x,
	  coord_t current_y)
{
	// Figure out how to resize the panes.
	grid_map_t::lock lock{grid_map};

	auto ret=find_panes(which_slider);

	if (!ret)
		return;

	// Here are their original sizes.

	dim_t first_pane=original_sizes.first_pane;
	dim_t second_pane=original_sizes.second_pane;

	// Adjust their original size by how far the pointer traveled
	// from its grab point.

	if (current_x < original_x)
	{
		dim_t left=dim_t::truncate(original_x-current_x);

		if (left > first_pane)
			left=first_pane;

		first_pane=first_pane - left;

		second_pane=dim_t::truncate(second_pane+left);
	}
	else
	{
		dim_t right=dim_t::truncate(current_x-original_x);

		if (right > second_pane)
			right=second_pane;

		second_pane=second_pane - right;

		first_pane=dim_t::truncate(first_pane+right);
	}

	// Now, update the panes' metrics, and the grid layout manager will,
	// eventually, do the rest.

	auto &[before, after]=*ret;

	auto before_hv=before->impl->get_horizvert(IN_THREAD);
	auto after_hv=after->impl->get_horizvert(IN_THREAD);

	before_hv->set_element_metrics(IN_THREAD,
				       {first_pane, first_pane, first_pane},
				       before_hv->vert);
	after_hv->set_element_metrics(IN_THREAD,
				      {second_pane, second_pane, second_pane},
				      after_hv->vert);
}

LIBCXXW_NAMESPACE_END
