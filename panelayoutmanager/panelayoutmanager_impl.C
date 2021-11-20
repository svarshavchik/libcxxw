/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "panelayoutmanager/pane_slider.H"
#include "panelayoutmanager/pane_slider_focusframe.H"
#include "panelayoutmanager/pane_slider_impl_element.H"
#include "x/w/impl/focus/standard_focusframecontainer_element_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/focus/standard_focusframecontainer_element.H"
#include "x/w/impl/current_border_impl.H"
#include "screen.H"
#include "screen_positions_impl.H"
#include "straight_border_impl.H"
#include "generic_window_handler.H"
#include "x/w/impl/icon.H"
#include "cursor_pointer.H"
#include "x/w/impl/background_color.H"
#include "peephole/peephole.H"
#include "peephole/peepholed.H"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl_scrollbars.H"
#include "peephole/peephole_gridlayoutmanagerobj.H"
#include "grid_map_info.H"
#include "gridlayoutmanager_impl_elements.H"
#include "calculate_borders.H"
#include "messages.H"
#include "x/w/pane_appearance.H"
#include "x/w/pane_layout_appearance.H"
#include "x/w/panefactory.H"

LIBCXXW_NAMESPACE_START

panelayoutmanagerObj::implObj::implObj(const ref<panecontainer_implObj>
				       &pane_container_impl,
				       const const_pane_layout_appearance
				       &appearance,
				       const std::string &name,
				       const std::vector<dim_t> &restored_sizes)
	: gridlayoutmanagerObj::implObj{pane_container_impl, {}},
	  pane_container_impl{pane_container_impl},
	  appearance{appearance}, name{name},
	  restored_sizes_thread_only{restored_sizes}
{
}

panelayoutmanagerObj::implObj::~implObj()=default;

size_t panelayoutmanagerObj::implObj::size(const grid_map_t::lock &lock) const
{
	// Empty: slider
	//
	// One pane: pane+slider
	//
	// Two panes: pane+slider+pane
	//
	// Three panes: pane+slider+pane+slider+pane
	//

	auto s=total_size(lock);

	if (s <= 1)
		return 0;

	return (s+1)/2;
}

elementptr panelayoutmanagerObj::implObj
::get_pane_element(const grid_map_t::lock &grid_lock, size_t n)
{
	elementptr e;

	if (n < size(grid_lock))
	{
		pane_peephole_container container=get_element(grid_lock, n*2);

		e=container->get_peephole()->peepholed_element;
	}

	return e;
}

layoutmanager panelayoutmanagerObj::implObj::create_public_object()
{
	return create_panelayoutmanager();
}

panelayoutmanager panelayoutmanagerObj::implObj::create_panelayoutmanager()
{
	return panelayoutmanager::create(ref{this});
}

void panelayoutmanagerObj::implObj::create_slider(const gridfactory &f)
{
	auto &e=pane_container_impl->container_element_impl();

	// Create the implementation object for the slider element.
	auto slider_border=e.create_border(appearance->slider);

	// Start with a focus frame.

	auto ff=ref<pane_slider_focusframeObj>::create
		(f->get_container_impl(),
		 appearance,
		 e.get_window_handler()
		 .create_icon({slider_cursor()})->create_cursor());


	auto slider_impl=create_pane_slider_impl(ff, slider_border);

	auto slider=pane_slider::create(slider_impl);

	auto slider_container=create_focusframe_container_owner(ff, ff, slider,
								slider_impl);

	slider_container->label_for(slider);

	initialize_factory_for_slider(f);

	f->created_internally(slider_container);
}

void panelayoutmanagerObj::implObj
::initialize_factory_for_slider(const gridfactory &f)
{
	f->padding(0);
	f->border(appearance->border);
}

create_pane_info_t panelayoutmanagerObj::implObj
::create_pane_peephole(panefactory_implObj &factory)
{
	auto [metrics, reference_size] =
		initial_peephole_metrics(factory.appearance->size);

	// Container for the peephole and its scrollbar.

	auto peephole_container_impl=ref<pane_peephole_containerObj::implObj>
		::create(pane_container_impl,
			 child_element_init_params
			 {{}, {}, factory.appearance->background_color});

	// The peephole element.

	auto peephole_impl=ref<pane_peepholeObj::implObj>
		::create(peephole_container_impl,
			 child_element_init_params
			 {{}, metrics});

	return {peephole_container_impl, peephole_impl, reference_size};
}

namespace {
#if 0
}
#endif

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

	size_t peepholed_rows(ONLY IN_THREAD) const override
	{
		return 0;
	}
};

#if 0
class my_peephole_gridlayoutmanagerObj : public peephole_gridlayoutmanagerObj {

public:

	using peephole_gridlayoutmanagerObj::peephole_gridlayoutmanagerObj;

	bool rebuild_elements_and_update_metrics(ONLY IN_THREAD,
						 grid_map_t::lock &grid_lock,
						 bool already_sized) override
	{

		auto flag=peephole_gridlayoutmanagerObj
			::rebuild_elements_and_update_metrics(IN_THREAD,
							      grid_lock,
							      already_sized);

		std::cout << std::endl;

		for (const auto &e : grid_elements(IN_THREAD)->all_elements)
		{
			auto c=dynamic_cast<containerObj::implObj *>
				(&*e.child_element->impl);

			if (c)
				c->invoke_layoutmanager
					([&]
					 (const auto &lm)
					{
						std::cout << lm->objname()
							  << " ("
							  << &*lm
							  << ")";
					});
			else
				std::cout << "element";

			std::cout << ": " << e.pos->horiz_pos.start << ", "
				  << e.pos->vert_pos.start
				  << ": "
				  << e.axises->horiz.minimum()
				  << "x"
				  << e.axises->vert.minimum()
				  << std::endl;
		}
		return flag;
	}

};
#endif


#if 0
{
#endif
}

pane_peephole_container panelayoutmanagerObj::implObj
::created_pane_peephole(const panelayoutmanager &public_object,
			const create_pane_info_t &pane_info,
			const const_pane_appearance &appearance,
			panefactoryObj &pfactory,
			const element &e,
			size_t position,
			grid_map_t::lock &lock)
{
	auto s=size(lock);

	if (position > s)
		throw EXCEPTION(gettextmsg(_("Pane #%1% does not exist"),
					   position));

	// Reference sizes are no longer valid, there's a new element in
	// the pane.
	reference_size_set(lock)=false;

	// The players here are as follows:
	//
	// 1) Pane container, with a panelayoutmanager (a grid layout manager
	// in disguise).
	//
	// 2) pane_info.peephole_container_impl. A child element of my
	// container_impl, represents a pane. This is a container that will
	// use the peephole_gridlayoutmanagerObj, to manage the layout of
	// a peephole with scrollbars.
	//
	// 3) pane_info.peephole_impl. A child element of
	// pane_info.peephole_container_impl. Uses the
	// peepholelayoutmanagerObj::implObj::scrollbarsObj layout manager.
	//
	// 4) "e" parameter, will be the peepholed element placed into the
	// pane_info.peephole_impl.

	auto [style, horizontal_scrollbar_visibility,
	      vertical_scrollbar_visibility]=
		pane_peephole_style(appearance->pane_scrollbar_visibility);

	style.horizontal_alignment=appearance->horizontal_alignment;
	style.vertical_alignment=appearance->vertical_alignment;

	auto peepholed_element=ref<pane_peepholed_elementObj>
		::create(e, pane_info.peephole_impl);

	const auto &[layout_impl, pane_container_grid_impl, grid]=
		create_peephole_with_scrollbars
		([&]
		 (const auto &info, const auto &scrollbars)
		 {
			 return ref<peepholelayoutmanagerObj::implObj
				    ::scrollbarsObj>
				 ::create(info, scrollbars,
					  pane_info.peephole_impl,
					  peepholed_element);
		 },
		 [&]
		 (const ref<peepholelayoutmanagerObj::implObj> &layout_impl)
		 -> peephole_element_factory_ret_t
		 {
			 auto peephole_in_pane=
				 peephole::create(pane_info.peephole_impl,
						  layout_impl);

			 return {
				 peephole_in_pane,
				 peephole_in_pane,
				 std::nullopt,
				 std::nullopt,
				 {},
				 appearance->left_padding,
				 appearance->right_padding,
				 appearance->top_padding,
				 appearance->bottom_padding,
			 };
		 },
#if 0
		 []
		 (const peephole_gridlayoutmanagerObj::init_args &init_args)
		 {
			 return ref<my_peephole_gridlayoutmanagerObj>::create(init_args);
		 },
#else
		 create_peephole_gridlayoutmanager,
#endif
		 {
		  pane_info.peephole_container_impl,
		  std::nullopt,
		  style,
		  horizontal_scrollbar_visibility,
		  vertical_scrollbar_visibility,
		  appearance->horizontal_scrollbar,
		  appearance->vertical_scrollbar,
		 });

	// Ok, we can now create the container.
	auto pane=pane_peephole_container::create(pane_info
						  .peephole_container_impl,
						  pane_container_grid_impl,
						  pane_info.reference_size);

	// How the new pane gets inserted into the pane container depends
	// on the existing contents of the pane.

	if (s == 0)
	{
		// Very first pane. What we have now is the stub slider.

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
					     &which_slider,
					     grid_map_t::lock &grid_lock)
{
	// Reference sizes are no longer valid, we're adjusting the
	// panes' sizes. Whatever they'll end up as, they will become the
	// new reference sizes.
	reference_size_set(grid_lock)=false;

	auto ret=find_pane_peephole_containers(grid_lock, which_slider);

	if (ret)
	{
		auto &[before, after] = *ret;

		return { element_size(IN_THREAD, before->get_peephole()->impl),
			 element_size(IN_THREAD, after->get_peephole()->impl) };
	}

	return {};
}

void panelayoutmanagerObj::implObj::slide_start(ONLY IN_THREAD,
						const ref<elementObj::implObj>
						&which_slider)
{
	coord_t reference_height=
		coord_t::truncate(pane_container_impl->font_height(IN_THREAD));

	grid_map_t::lock grid_lock{grid_map};

	// Simulate a slide

	auto original_sizes=start_sliding(IN_THREAD, which_slider, grid_lock);

	sliding(IN_THREAD, grid_lock, which_slider, original_sizes,
		reference_height, reference_height, 0, 0);
}

void panelayoutmanagerObj::implObj::slide_end(ONLY IN_THREAD,
					      const ref<elementObj::implObj>
					      &which_slider)
{
	coord_t reference_height=
		coord_t::truncate(pane_container_impl->font_height(IN_THREAD));

	grid_map_t::lock grid_lock{grid_map};

	// Simulate a slide

	auto original_sizes=start_sliding(IN_THREAD, which_slider, grid_lock);

	sliding(IN_THREAD, grid_lock, which_slider, original_sizes, 0, 0,
		reference_height, reference_height);
}

std::optional<std::tuple<pane_peephole_container, pane_peephole_container>>
panelayoutmanagerObj::implObj::find_pane_peephole_containers(grid_map_t::lock &grid_lock,
							     const ref<elementObj::implObj> &s)
{
	std::optional<std::tuple<element, element>> ret;

	// Must have at least two panes.

	if (size(grid_lock) > 1)
	{
		// Look up the slider's position.

		auto res=lookup_element(grid_lock, s);

		if (res)
		{
			// Found it, return the slider's neighbors.

			pane_peephole_container above=
				get_element(grid_lock, (*res)-1);

			pane_peephole_container below=
				get_element(grid_lock, (*res)+1);

			ret.emplace(above, below);
		}
	}
	return ret;
}

void panelayoutmanagerObj::implObj
::remove_pane(const panelayoutmanager &public_object,
	      size_t pane_number,
	      grid_map_t::lock &lock)
{
	size_t s=size(lock);

	if (pane_number >= s)
		return;

	// Reference sizes are no longer valid, after removing one of the
	// panes.
	reference_size_set(lock)=false;

	if (s == 2)
	{
		if (pane_number == 1)
		{
			remove_element(lock, 2);
			return; // Leaving the slider after the remaining pane.
		}

		remove_element(lock, 1);
		remove_element(lock, 0);

		// Need to create the stub slider.

		auto factory=create_slider_factory(&*public_object, 1);
		create_slider(factory);

		focusable f=get_element(lock, 1);
		f->get_focus_after(get_element(lock, 0));

		return;
	}

	if (pane_number == 0)
	{
		if (s == 1)
		{
			// Do not remove the slider.
			remove_elements(lock, 0, 1);
		}
		else
		{
			remove_elements(lock, 0, 2);
		}
		return;
	}

	remove_element(lock, pane_number*2);
	remove_element(lock, pane_number*2-1);
}

void panelayoutmanagerObj::implObj
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	{
		grid_map_t::lock grid_lock{grid_map};

		// Reference sizes are no longer valid. New them.
		reference_size_set(grid_lock)=false;
	}
	gridlayoutmanagerObj::implObj::theme_updated(IN_THREAD, new_theme);
}

bool panelayoutmanagerObj::implObj
::rebuild_elements_and_update_metrics(ONLY IN_THREAD,
				      grid_map_t::lock &grid_lock,
				      bool already_sized)
{
	// Must rebuild the elements first, before calling
	// adjust_panes_for_current_size(), because any changes to the
	// elements results in the grid border elements getting rebuilt,
	// and adjust_panes_for_current_size() computes this.

	auto flag=gridlayoutmanagerObj::implObj
		::rebuild_elements_and_update_metrics
		(IN_THREAD, grid_lock, already_sized);

	// Empty container, before getting shown, doesn't matter
	if (already_sized)
		adjust_panes_for_current_size(IN_THREAD, grid_lock,
					      get_element_impl()
					      .data(IN_THREAD).current_position
					      );

	return flag;
}

bool panelayoutmanagerObj::implObj
::reposition_child_elements(ONLY IN_THREAD,
			    const rectangle &position,
			    grid_map_t::lock &grid_lock)
{
	adjust_panes_for_current_size(IN_THREAD, grid_lock, position);

	return gridlayoutmanagerObj::implObj::reposition_child_elements
		(IN_THREAD, position, grid_lock);
}

void panelayoutmanagerObj::implObj
::adjust_panes_for_current_size(ONLY IN_THREAD,
				grid_map_t::lock &grid_lock,
				const rectangle &position)
{
	// Get the current size of the container.
	auto pane_size=size_from_position(position);

	auto &reference_size_is_set_now=reference_size_set(grid_lock);

	size_t n=total_size(grid_lock);
	dim_squared_t fixed_overhead{0};

	std::vector<pane_peephole_container> pane_peephole_containers;
	pane_peephole_containers.reserve(n/2+1);

	// If there's only one element, it's a slider. Otherwise
	// all elements in odd positions are sliders, and even positions
	// are peephole containers.

	if (n > 1)
	{
		for (size_t i=0; i<n; i += 2)
		{
			pane_peephole_container pp=get_element(grid_lock, i);

			auto pc_size=element_size(IN_THREAD, pp->impl);
			auto pp_size=element_size(IN_THREAD, pp->get_peephole()
						  ->impl);

			// The peephole container's size should be larger than
			// the peephole's size.
			if (pc_size < pp_size)
				pc_size=pp_size;

			// The fixed overhead is the difference between the two.
			//
			// The fixed overhead gets added to the total fixed
			// overhead, and gets stored in the pane. If it changes
			// we'll need to reset the reference size of all the
			// panes.
			//
			// This happens when the peephole changes its
			// horizontal scrollbar's visibility, which it punts
			// to the process_element_position_finalized phase
			// which happens after initial metrics.
			//
			// 1) The peephole's metrics get calculated
			//
			// 2) We get here
			//
			// 3) process_element_position_finalized phase runs
			//    and the peephole determines whether or not
			//    the horizontal scrollbar should be visible.
			//
			// This affects the fixed overhead, so we store it
			// and if we detect that the fixed overhead changed
			// then reference_size_is_set_now no more.

			auto overhead=pc_size-pp_size;

			fixed_overhead += overhead;

			if (pp->fixed_overhead(IN_THREAD) != overhead)
			{
				reference_size_is_set_now=false;
				pp->fixed_overhead(IN_THREAD)=overhead;
			}
			pane_peephole_containers.push_back(pp);
		}
	}

	// We want to run through the following calculations if either:
	//
	// 1) The size of the pane changed, pane_size is not equal to
	//    most_recent_pane_size
	//
	// 2) Something was changed in the pane, and the reference size
	//    is not set any more. This happens when a pane element gets
	//    added or removed.

	if (pane_size == most_recent_pane_size &&
	    reference_size_is_set_now)
		return;

	// If we restored previous pane sizes, we'll install them as
	// existing reference sizes.
	if (!name.empty())
	{
		size_t n=size(grid_lock);

		if (n > 0)
		{
			if (n == restored_sizes(IN_THREAD).size())
			{
				for (size_t i=0; i<n; ++i)
				{
					pane_peephole_container pp=
						get_element(grid_lock, i*2);

					pp->reference_size(IN_THREAD)=
						restored_sizes(IN_THREAD)[i];
				}
				reference_size_is_set_now=true;
			}
			restored_sizes(IN_THREAD).clear();
		}
	}

	dim_squared_t total_reference_size;

	// First pass. We scan through the entire container, both panes
	// and the dividing sliders. The total size of the sliders is
	// fixed_overhead.
	//
	// Add up all reference_sizes, this is the total_reference_size.
	//
	// If reference_size_is_set_now we can simply use the reference_size
	// that's saved in each pane_peephole_container, otherwise we set
	// its reference_size now.

	for (size_t i= n > 1 ? 1:0; i<n; i += 2)
	{
		// If there's only one element, it's a slider. Otherwise
		// all elements in odd positions are sliders...

		fixed_overhead+=element_size(IN_THREAD,
					     get_element(grid_lock, i)->impl);
	}

	for (const auto &pp:pane_peephole_containers)
	{
		if (!reference_size_is_set_now)
		{
			auto s=element_size(IN_THREAD,
					    pp->get_peephole()->impl);

			pp->reference_size(IN_THREAD)=s;
		}
		total_reference_size += pp->reference_size(IN_THREAD);
	}

	// Also include the real estate occupied by the borders in the
	// pane containers.

	for (const auto &ae:grid_elements(IN_THREAD)->all_elements)
	{
		auto overhead=border_overhead(IN_THREAD,
					      ae.pos->horiz_pos.start,
					      ae.pos->vert_pos.start,
					      ae.child_element);

		fixed_overhead+=overhead;
	}

	// Edge case. If we have a goose egg, if somehow all panes have no size
	// we should just scale them equally.
	if (total_reference_size == 0)
	{
		for (const auto &ppc : pane_peephole_containers)
			ppc->reference_size(IN_THREAD)=1;

		total_reference_size=pane_peephole_containers.size();
	}

	reference_size_is_set_now=true;

	// Subtract fixed_overhead from pane_size. This should be the total
	// size of all pane elements.

	dim_squared_t resize_panes_to=dim_squared_t::truncate(pane_size);

	if (resize_panes_to < fixed_overhead)
		resize_panes_to=0;
	else
		resize_panes_to -= fixed_overhead;

	// resize_panes_to should be the total size of all panes.
	// The total reference size of the panes is total_reference_size.

	dim_squared_t nominator=0;

	for (const auto &ppc : pane_peephole_containers)
	{
		nominator += ppc->reference_size(IN_THREAD) * resize_panes_to;

		dim_t ps=dim_t::truncate(nominator /
					 total_reference_size);

		resize_peephole_to(IN_THREAD, ppc, ps);

		nominator = dim_t::truncate(nominator % total_reference_size);
	}
	most_recent_pane_size=pane_size;
}

void panelayoutmanagerObj::implObj
::do_sliding(ONLY IN_THREAD,
	     grid_map_t::lock &grid_lock,
	     const ref<elementObj::implObj> &which_slider,
	     const pane_slider_original_sizes &original_sizes,

	     coord_t current_pos,
	     coord_t original_pos)
{
	// Figure out how to resize the panes.

	auto ret=find_pane_peephole_containers(grid_lock, which_slider);

	if (!ret)
		return;

	// Here are their original sizes.

	dim_t first_pane=original_sizes.first_pane;
	dim_t second_pane=original_sizes.second_pane;

	// Adjust their original size by how far the pointer traveled
	// from its grab point.

	if (current_pos < original_pos)
	{
		dim_t move_start=dim_t::truncate(original_pos-current_pos);

		if (move_start > first_pane)
			move_start=first_pane;

		first_pane=first_pane - move_start;

		second_pane=dim_t::truncate(second_pane+move_start);
	}
	else
	{
		dim_t move_end=dim_t::truncate(current_pos-original_pos);

		if (move_end > second_pane)
			move_end=second_pane;

		second_pane=second_pane - move_end;

		first_pane=dim_t::truncate(first_pane+move_end);
	}

	// Now, update the panes' metrics, and the grid layout manager will,
	// eventually, do the rest.

	auto &[before, after]=*ret;

	if (element_size(IN_THREAD, before->get_peephole()->impl) +
	    element_size(IN_THREAD, after->get_peephole()->impl) !=
	    first_pane+second_pane)
		return;

	reference_size_set(grid_lock)=false;
	resize_peephole_to(IN_THREAD, before, first_pane);
	resize_peephole_to(IN_THREAD, after, second_pane);
}

void panelayoutmanagerObj::implObj::save(ONLY IN_THREAD,
					 const screen_positions &pos)
{
	if (name.empty())
		return;

	grid_map_t::lock grid_lock{grid_map};

	auto writelock=pos->impl->create_writelock_for_saving("pane", name);

	size_t s=size(grid_lock);

	for (size_t i=0; i<s; ++i)
	{
		pane_peephole_container container=get_element(grid_lock, i*2);

		writelock->create_child()->element({"size"})
			->text(container->peephole_size.get())
			->parent()->parent();
	}
}

/////////////////////////////////////////////////////////////////////////
//
// Vertical panel implementation methods.

typedef panelayoutmanagerObj::implObj::vertical vertical;

template<>
gridfactory panelayoutmanagerObj::implObj::orientation<vertical>
::create_slider_factory(gridlayoutmanagerObj *public_object,
			size_t pos)
{
	return insert_row(public_object,
			  pos == 0 ? 0: // Initial slider
			  1+(pos-1)*2 // Subsequent sliders
			  );
}

template<>
const std::string &panelayoutmanagerObj::implObj::orientation<vertical>
::slider_cursor() const
{
	return appearance->slider_vert;
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
::total_size(const grid_map_t::lock &lock) const
{
	return (*lock)->rows();
}

template<>
element panelayoutmanagerObj::implObj::orientation<vertical>
::get_element(const grid_map_t::lock &lock, size_t n)
{
	return (*lock)->get(n, 0);
}

template<>
std::optional<size_t> panelayoutmanagerObj::implObj::orientation<vertical>
::lookup_element(const grid_map_t::lock &grid_lock,
		 const ref<elementObj::implObj> &e)
{
	std::optional<size_t> res;

	auto ret=lookup_row_col(grid_lock, e);

	if (ret)
		res=std::get<0>(*ret);

	return res;
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::remove_elements(grid_map_t::lock &grid_lock, size_t n, size_t c)
{
	remove_rows(grid_lock, n, c);
}

template<>
std::tuple<peephole_style, scrollbar_visibility, scrollbar_visibility>
panelayoutmanagerObj::implObj::orientation<vertical>
::pane_peephole_style(scrollbar_visibility pane_scrollbar_visibility)
{
	peephole_style style{peephole_algorithm::stretch_peephole,
			     peephole_algorithm::automatic};

	return {style, scrollbar_visibility::never,
			pane_scrollbar_visibility};
}

template<>
std::tuple<metrics::horizvert_axi, dim_t>
panelayoutmanagerObj::implObj::orientation<vertical>
::initial_peephole_metrics(const dim_arg &size)
{
	auto theme=pane_container_impl->container_element_impl()
		.get_screen()->impl->current_theme.get();

	auto s=theme->get_theme_dim_t(size, themedimaxis::height);

	return { { {0, 0, 0}, {s, s, s} }, s};
}


template<>
dim_t panelayoutmanagerObj::implObj::orientation<vertical>
::element_size(ONLY IN_THREAD,
	       const ref<elementObj::implObj> &e)
{
	return e->get_horizvert(IN_THREAD)->vert.minimum();
}

template<>
dim_t panelayoutmanagerObj::implObj::orientation<vertical>
::size_from_position(const rectangle &position)
{
	return position.height;
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::resize_peephole_to(ONLY IN_THREAD, const pane_peephole_container &ppc,
		     dim_t s)
{
	auto hv=ppc->get_peephole()->elementObj::impl->get_horizvert(IN_THREAD);

	hv->set_element_metrics(IN_THREAD, hv->horiz, {s, s, s});
	ppc->peephole_size=s;
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::sliding(ONLY IN_THREAD,
	  grid_map_t::lock &grid_lock,
	  const ref<elementObj::implObj> &which_slider,
	  const pane_slider_original_sizes &original_sizes,

	  coord_t original_x,
	  coord_t original_y,

	  coord_t current_x,
	  coord_t current_y)
{
	do_sliding(IN_THREAD, grid_lock, which_slider, original_sizes,
		   current_y, original_y);
}

template<>
void panelayoutmanagerObj::implObj::orientation<vertical>
::set_element_metrics(ONLY IN_THREAD,
		      const metrics::axis &h,
		      const metrics::axis &v)
{
	gridlayoutmanagerObj::implObj::set_element_metrics
		(IN_THREAD, h,
		 get_element_impl().get_horizvert(IN_THREAD)->vert);
}

template<>
dim_t panelayoutmanagerObj::implObj::orientation<vertical>
::border_overhead(ONLY IN_THREAD,
		  metrics::grid_xy x,
		  metrics::grid_xy y,
		  const element &e)
{
	if (IS_BORDER_RESERVED_COORD(x))
		return 0;

	if (!IS_BORDER_RESERVED_COORD(y))
		return 0;

	return e->impl->get_horizvert(IN_THREAD)->vert.minimum();
}

/////////////////////////////////////////////////////////////////////////
//
// Horizontal panel implementation methods.

typedef panelayoutmanagerObj::implObj::horizontal horizontal;

template<>
gridfactory panelayoutmanagerObj::implObj::orientation<horizontal>
::create_slider_factory(gridlayoutmanagerObj *public_object,
			size_t pos)
{
	return insert_columns(public_object,
			      0,
			      pos == 0 ? 0: // Initial slider
			      1+(pos-1)*2 // Subsequent sliders
			      );
}

template<>
const std::string &panelayoutmanagerObj::implObj::orientation<horizontal>
::slider_cursor() const
{
	return appearance->slider_horiz;
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
::total_size(const grid_map_t::lock &lock) const
{
	return (*lock)->cols(0);
}

template<>
element panelayoutmanagerObj::implObj::orientation<horizontal>
::get_element(const grid_map_t::lock &lock, size_t n)
{
	return (*lock)->get(0, n);
}

template<>
std::optional<size_t> panelayoutmanagerObj::implObj::orientation<horizontal>
::lookup_element(const grid_map_t::lock &grid_lock,
		 const ref<elementObj::implObj> &e)
{
	std::optional<size_t> res;

	auto ret=lookup_row_col(grid_lock, e);

	if (ret)
		res=std::get<1>(*ret);

	return res;
}

template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::remove_elements(grid_map_t::lock &grid_lock, size_t n, size_t c)
{
	remove(grid_lock, 0, n, c);
}

template<>
std::tuple<peephole_style, scrollbar_visibility, scrollbar_visibility>
panelayoutmanagerObj::implObj::orientation<horizontal>
::pane_peephole_style(scrollbar_visibility pane_scrollbar_visibility)
{
	peephole_style style{peephole_algorithm::automatic,
			     peephole_algorithm::stretch_peephole};

	return {style, pane_scrollbar_visibility,
			scrollbar_visibility::never};
}

template<>
std::tuple<metrics::horizvert_axi, dim_t>
panelayoutmanagerObj::implObj::orientation<horizontal>
::initial_peephole_metrics(const dim_arg &size)
{
	auto theme=pane_container_impl->container_element_impl()
		.get_screen()->impl->current_theme.get();

	auto s=theme->get_theme_dim_t(size, themedimaxis::width);

	return { { {s, s, s}, {0, 0, 0} }, s};
}

template<>
dim_t panelayoutmanagerObj::implObj::orientation<horizontal>
::element_size(ONLY IN_THREAD,
	       const ref<elementObj::implObj> &e)
{
	return e->get_horizvert(IN_THREAD)->horiz.minimum();
}

template<>
dim_t panelayoutmanagerObj::implObj::orientation<horizontal>
::size_from_position(const rectangle &position)
{
	return position.width;
}


template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::resize_peephole_to(ONLY IN_THREAD, const pane_peephole_container &ppc,
		     dim_t s)
{
	auto hv=ppc->get_peephole()->elementObj::impl->get_horizvert(IN_THREAD);

	hv->set_element_metrics(IN_THREAD, {s, s, s}, hv->vert);
	ppc->peephole_size=s;
}

template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::sliding(ONLY IN_THREAD,
	  grid_map_t::lock &grid_lock,
	  const ref<elementObj::implObj> &which_slider,
	  const pane_slider_original_sizes &original_sizes,

	  coord_t original_x,
	  coord_t original_y,

	  coord_t current_x,
	  coord_t current_y)
{
	do_sliding(IN_THREAD, grid_lock, which_slider, original_sizes,
		   current_x, original_x);
}

template<>
void panelayoutmanagerObj::implObj::orientation<horizontal>
::set_element_metrics(ONLY IN_THREAD,
		      const metrics::axis &h,
		      const metrics::axis &v)
{
	gridlayoutmanagerObj::implObj::set_element_metrics
		(IN_THREAD, get_element_impl().get_horizvert(IN_THREAD)->horiz,
		 v);
}

template<>
dim_t panelayoutmanagerObj::implObj::orientation<horizontal>
::border_overhead(ONLY IN_THREAD,
		  metrics::grid_xy x,
		  metrics::grid_xy y,
		  const element &e)
{
	if (IS_BORDER_RESERVED_COORD(y))
		return 0;

	if (!IS_BORDER_RESERVED_COORD(x))
		return 0;

	return e->impl->get_horizvert(IN_THREAD)->horiz.minimum();
}

LIBCXXW_NAMESPACE_END
