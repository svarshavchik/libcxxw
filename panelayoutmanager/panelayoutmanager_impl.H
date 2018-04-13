/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef panelayoutmanager_impl_h
#define panelayoutmanager_impl_h

#include "x/w/panelayoutmanager.H"
#include "x/w/panefactoryfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/scrollbar.H"
#include "panelayoutmanager/create_pane_info.H"
#include "panelayoutmanager/create_pane_properties.H"
#include "panelayoutmanager/pane_sliderobj.H"
#include "panelayoutmanager/pane_slider_original_sizes.H"
#include "panelayoutmanager/pane_peephole_containerfwd.H"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "panelayoutmanager/pane_peephole_impl.H"
#include "panelayoutmanager/panecontainer_impl.H"
#include "panelayoutmanager/panefactory_impl.H"
#include "gridlayoutmanager.H"
#include "peephole/peephole_style.H"
#include "current_border_implfwd.H"
#include <tuple>

LIBCXXW_NAMESPACE_START

/*!
Internal implementation object.

The pane layout manager is based on the grid layout manager, that
contains a sequence of:

\code
[ pane element ]
[ slider ]
[ pane element ]
[ slider ]
[ pane element ]
\endcode

This is for a vertical pane.

Sliders get automatically inserted in between elements.

In all cases, there is an extra row in the grid layout manager that
contains a canvas, so the above picture is really.

\code
[ pane element ]
[ slider ]
[ pane element ]
[ slider ]
[ pane element ]
[ canvas ]
\endcode

The canvas's metrics are always zero min/pref/max width & height, but
the grid is configured to give the canvas element 100% of any extra
space.

The following ignores the existing of the trailing canvas element.

The initial empty container consists of a placeholder slider in row #0.
This is because the pane container is a focusable container and it must
have at least one focusable element. Sliders are focusable elements.

When there's one pane element, the slider element follows it.
The second pane element gets added after the slider. From that point on:

- adding a new element adds a slider followed by the new element; unless
the new element is the first element, in which case the element gets
added first, followed by a new slider.

- when there are three or more existing elements, deleting the first
pane element deletes it and the following slider; deleting any other
element deletes it and the slider immediately before it.

When there are two elements in the pane:

- deleting the 2nd pane element deletes only the pane element itself.

- deleting the 1st pane element deletes it and the lone slider, then a
new slider gets inserted after the remaining element.

The end result is that the following convention is always followed:

- If there are no pane elements, the container has only a slider element.

- If there is one pane element, the pane element is followed by a slider
element.

- If there are two or more pane elements they get separated by sliders.

*/

class LIBCXX_HIDDEN panelayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

	//! Alias

	typedef gridlayoutmanagerObj::implObj superclass_t;
 public:

	//! My container
	const ref<panecontainer_implObj> pane_container_impl;

	//! This pane's style

	const pane_style style;

	class horizontal;
	class vertical;

	//! Enumerator for the template that instantiates subclasses

	//! Two subclasses implement the virtual methods, implementing
	//! horizontal and vertical panes.

	template<typename> class orientation;

	//! Constructor
	implObj(const ref<panecontainer_implObj> &pane_container_impl,
		const pane_style &style);

	//! Destructor
	~implObj();

	//! Create a panelayoutmanager

	layoutmanager create_public_object() override;

	//! Logical size of the container, how many panes.

	size_t size();

	//! Return the requested element.
	elementptr get_pane_element(size_t);

	//! Remove an existing pane.
	void remove_pane(const panelayoutmanager &public_object,
			 size_t pane_number);

	/*!

	  Creates a factory for a slider element at this position

	  This called to create a factory for a new slider element, a
	  part of the process for adding a new pane. When the existing
	  container has only one existing element, the resulting factory
	  gets used only to create the new pane element.

	  The resulting factory is used as follows:

	  When creating a new container: the resulting factory gets passed
	  to create_slider().

	  When creating the first element in the pane: the resulting
	  factory gets used to create only the new element.

	  When creating a new element in position 0, and the pane already
	  has at least one element: the resulting factory gets used to create
	  the new element, then it gets passed to create_slider(). If there
	  are now two panes, the slider after the now-2nd element gets
	  removed.

	  When creating a new element in position 1 and the pane has one
	  element: the resulting factory get used to create the new element.

	  Otherwise a new element is getting added to the existing pane
	  with at least two elements, and the new element is not in
	  position 0: the resulting factory gets passed to create_slider()
	  then it gets used to create the new element.

	*/

	virtual gridfactory
		create_slider_factory(layoutmanagerObj *public_object,
				      size_t pos)=0;

	//! Create the slider element.

	void create_slider(const gridfactory &slider_factory);

	//! How many panes and sliders there are in the container.

	//! The real size of the container, including the slider elements.

	virtual size_t total_size(grid_map_t::lock &lock)=0;

	//! Return the given pane or slider.

	virtual element get_element(grid_map_t::lock &lock, size_t n)=0;

	//! Find the element in the pane container.
	virtual std::optional<size_t>
		lookup_element(const ref<elementObj::implObj> &e)=0;

	//! Remove one element.

	inline void remove_element(grid_map_t::lock &lock, size_t n)
	{
		return remove_elements(lock, n, 1);
	}

	virtual void remove_elements(grid_map_t::lock &, size_t n, size_t c)=0;

	//! Start of the process of creating a new pane.

	//! Invoked by the factory as part of the process of creating a new
	//! pane element. Returns a (mostly) opaque \c create_pane_info_t
	//! object.
	//!
	//! The new element in the pane will be in the
	//! peephole_impl value.

	create_pane_info_t create_pane_peephole(panefactory_implObj &factory);

	//! Finish the process of creating a new pane.

	//! After the new element gets created, this finishes the job. This
	//! is called from a panefactory (subclass). Receives the public
	//! layout manager object as its first parameter. The remaining
	//! parameters:
	//!
	//! - what create_pane_peephole() originally returned.
	//!
	//! - the factory that's creating the new pane.
	//!
	//! - the new element created by the factory,
	//!
	//! - the position, specified by the factory.
	//!
	//! - the factory's internal lock.

	pane_peephole_container
		created_pane_peephole(const panelayoutmanager &public_object,
				      const create_pane_info_t &info,
				      const create_pane_properties_t &properties,
				      panefactoryObj &factory,
				      const element &e,
				      size_t position,
				      grid_map_t::lock &lock);

	//! Adjusted the requested size for the trailing canvas element.

	//! After adding or removing panes, adjust the requested row or
	//! column sizes so that the canvas always gets any extra real
	//! estate.
	virtual void request_extra_space_to_canvas()=0;


	//! Callback from a slider element: sliding has started.

	pane_slider_original_sizes
		start_sliding(ONLY IN_THREAD,
			      const ref<elementObj::implObj> &which_slider);

	//! Slide panes.

	virtual void sliding(ONLY IN_THREAD,
			     const ref<elementObj::implObj> &which_slider,
			     const pane_slider_original_sizes &original_sizes,

			     coord_t original_x,
			     coord_t original_y,

			     coord_t current_x,
			     coord_t current_y)=0;

	//! Key up/left
	void slide_start(ONLY IN_THREAD,
			 const ref<elementObj::implObj> &which_slider);

	//! Key down_right
	void slide_end(ONLY IN_THREAD,
		       const ref<elementObj::implObj> &which_slider);

 protected:

	//! Create the appropriate factory for the new element.

	//! Either create a new row, or a new columns.

	virtual gridfactory create_factory_for_pos(const panelayoutmanager &lm,
						   size_t pos)=0;

	//! Construct the pane_slider implementation object.

	virtual ref<pane_sliderObj::implObj>
		create_pane_slider_impl(const container_impl &,
					const current_border_impl &)=0;

	//! Which pointer to use for the slider.

	virtual const char *slider_cursor() const=0;

	//! Set factory for creating a new slider element.

	void initialize_factory_for_slider(const gridfactory &);

	//! Set factory for creating a new pane element.

	void initialize_factory_for_pane(const gridfactory &);

	//! Compute initial pane metrics.

	//! Invoke from create_pane_peephole() to set the initial metrics
	//! for the new pane's peephole.

	virtual metrics::horizvert_axi
		initial_peephole_metrics(const dim_arg &size)=0;

	//! Style for panes' peepholes, and scroll-bar visibilities.

	virtual std::tuple<peephole_style,
			   scrollbar_visibility,
			   scrollbar_visibility>
		pane_peephole_style(scrollbar_visibility)=0;

	//! Given a slider element, find for me the two panes next to it.

	std::optional<std::tuple<element, element>
		      > find_panes(const ref<elementObj::implObj> &slider);

	//! Return original sizes of two panes.

	virtual pane_slider_original_sizes
		original_sizes(ONLY IN_THREAD,
			       const ref<elementObj::implObj> &before,
			       const ref<elementObj::implObj> &after)=0;
};

/////////////////////////////////////////////////////////////////////////

//! Horizontal and vertical implementations.

template<typename>
class panelayoutmanagerObj::implObj::orientation
	: public implObj {

 public:

	using implObj::implObj;

	//! Implement create_slider_factory()

	gridfactory
		create_slider_factory(layoutmanagerObj *public_object,
				      size_t pos) override;

	//! Implement create_factory_for_pos()

	gridfactory create_factory_for_pos(const panelayoutmanager &lm,
					   size_t pos) override;
	//! Implement create_pane_slider_impl().

	ref<pane_sliderObj::implObj>
	create_pane_slider_impl(const container_impl &,
				const current_border_impl &) override;

	//! Implement total_size()

	size_t total_size(grid_map_t::lock &lock) override;

	//! Implement get_element()

	element get_element(grid_map_t::lock &lock, size_t n) override;

	//! Implement pane_peephole_style().

	std::tuple<peephole_style,
		   scrollbar_visibility,
		   scrollbar_visibility>
	pane_peephole_style(scrollbar_visibility) override;

	//! Implement request_extra_space_to_canvas().

	void request_extra_space_to_canvas() override;

	//! Which pointer to use for the slider.

	const char *slider_cursor() const override;

	//! Implement initial_peephole_metrics().

	metrics::horizvert_axi
		initial_peephole_metrics(const dim_arg &size) override;

	//! Find the element in the pane container.
	std::optional<size_t> lookup_element(const ref<elementObj::implObj> &e)
		override;

	//! Remove the element.
	void remove_elements(grid_map_t::lock &, size_t n, size_t c) override;

	//! Capture original sizes of two panes.

	pane_slider_original_sizes
	original_sizes(ONLY IN_THREAD,
		       const ref<elementObj::implObj> &before,
		       const ref<elementObj::implObj> &after) override;

	//! Slide panes.

	void sliding(ONLY IN_THREAD,
		     const ref<elementObj::implObj> &which_slider,
		     const pane_slider_original_sizes &original_sizes,

		     coord_t original_x,
		     coord_t original_y,

		     coord_t current_x,
		     coord_t current_y) override;
};

LIBCXXW_NAMESPACE_END

#endif
