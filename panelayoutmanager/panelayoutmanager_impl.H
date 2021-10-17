/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef panelayoutmanager_impl_h
#define panelayoutmanager_impl_h

#include "x/w/panelayoutmanager.H"
#include "x/w/panefactoryfwd.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/scrollbar.H"
#include "panelayoutmanager/create_pane_info.H"
#include "panelayoutmanager/pane_sliderobj.H"
#include "panelayoutmanager/pane_slider_original_sizes.H"
#include "panelayoutmanager/pane_peephole_containerfwd.H"
#include "panelayoutmanager/pane_peephole_container_impl.H"
#include "panelayoutmanager/pane_peephole_impl.H"
#include "panelayoutmanager/panecontainer_impl.H"
#include "panelayoutmanager/panefactory_impl.H"
#include "gridlayoutmanager.H"
#include "metrics_grid.H"
#include "x/w/peephole_style.H"
#include "x/w/impl/current_border_implfwd.H"
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

	//! Flag that indicates whether the pane elements reference size is set.

	//! Accessed only while holding a grid lock.

	bool reference_size_set_under_lock=false;

	//! reference_size_set is accessible only while holding the grid lock.

	inline bool &reference_size_set(grid_map_t::lock &)
	{
		return reference_size_set_under_lock;
	}
 public:

	//! My container
	const ref<panecontainer_implObj> pane_container_impl;

	//! This pane's appearance

	const const_pane_layout_appearance appearance;

	//! This pane's name
	const std::string name;

 private:
	std::vector<dim_t> restored_sizes_thread_only;
 public:
	THREAD_DATA_ONLY(restored_sizes);

	class horizontal;
	class vertical;

	//! Enumerator for the template that instantiates subclasses

	//! Two subclasses implement the virtual methods, implementing
	//! horizontal and vertical panes.

	template<typename> class orientation;

	//! Constructor
	implObj(const ref<panecontainer_implObj> &pane_container_impl,
		const const_pane_layout_appearance &appearance,
		const std::string &name,
		const std::vector<dim_t> &restored_sizes);

	//! Destructor
	~implObj();

	//! Implement create_public_object()

	//! By calling create_panelayoutmanager().
	layoutmanager create_public_object() override;

	//! Create a panelayoutmanager
	panelayoutmanager create_panelayoutmanager();

	//! Logical size of the container, how many panes.

	size_t size(const grid_map_t::lock &grid_lock) const;

	//! Return the requested element.
	elementptr get_pane_element(const grid_map_t::lock &lock, size_t);

	//! Remove an existing pane.
	void remove_pane(const panelayoutmanager &public_object,
			 size_t pane_number,
			 grid_map_t::lock &grid_lock);

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
		create_slider_factory(gridlayoutmanagerObj *public_object,
				      size_t pos)=0;

	//! Create the slider element.

	void create_slider(const gridfactory &slider_factory);

	//! How many panes and sliders there are in the container.

	//! The real size of the container, including the slider elements.

	virtual size_t total_size(const grid_map_t::lock &lock) const=0;

	//! Return the given pane or slider.

	virtual element get_element(const grid_map_t::lock &lock, size_t n)=0;

	//! Find the element in the pane container.
	virtual std::optional<size_t>
		lookup_element(const grid_map_t::lock &grid_lock,
			       const ref<elementObj::implObj> &e)=0;

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
				      const const_pane_appearance &appearance,
				      panefactoryObj &factory,
				      const element &e,
				      size_t position,
				      grid_map_t::lock &lock);

	//! Callback from a slider element: sliding has started.

	pane_slider_original_sizes
		start_sliding(ONLY IN_THREAD,
			      const ref<elementObj::implObj> &which_slider,
			      grid_map_t::lock &grid_lock);

	//! Slide panes.

	virtual void sliding(ONLY IN_THREAD,
			     grid_map_t::lock &grid_lock,
			     const ref<elementObj::implObj> &which_slider,
			     const pane_slider_original_sizes &original_sizes,

			     coord_t original_x,
			     coord_t original_y,

			     coord_t current_x,
			     coord_t current_y)=0;

	//! Adjust the size of the peephole.

	//! sliding() and adjust_panes_for_current_size() pass in a peephole
	//! and its designates size.
	virtual void resize_peephole_to(ONLY IN_THREAD,
					const pane_peephole_container &ppc,
					dim_t s)=0;

	//! Key up/left
	void slide_start(ONLY IN_THREAD,
			 const ref<elementObj::implObj> &which_slider);

	//! Key down_right
	void slide_end(ONLY IN_THREAD,
		       const ref<elementObj::implObj> &which_slider);

	//! Move the slider

	//! The dimension-specific sliding() calls do_sliding() with the
	//! relevant dimension setting the original_pos and current_pos.

	void do_sliding(ONLY IN_THREAD,
			grid_map_t::lock &grid_lock,
			const ref<elementObj::implObj> &which_slider,
			const pane_slider_original_sizes &original_sizes,

			coord_t current_pos,
			coord_t original_pos);

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

	//! Returns a ref to slider_horiz or slider_vert in the
	//! appearance object.
	virtual const std::string &slider_cursor() const=0;

	//! Set factory for creating a new slider element.

	void initialize_factory_for_slider(const gridfactory &);

	//! Set factory for creating a new pane element.

	void initialize_factory_for_pane(const gridfactory &);

	//! Compute initial pane metrics.

	//! Invoke from create_pane_peephole() to set the initial metrics
	//! for the new pane's peephole, and return its reference size,
	//! discretely.

	virtual std::tuple<metrics::horizvert_axi, dim_t>
		initial_peephole_metrics(const dim_arg &size)=0;

	//! Style for panes' peepholes, and scroll-bar visibilities.

	virtual std::tuple<peephole_style,
			   scrollbar_visibility,
			   scrollbar_visibility>
		pane_peephole_style(scrollbar_visibility)=0;

	//! Given a slider element, find for me the two panes next to it.

	std::optional<std::tuple<pane_peephole_container,
				 pane_peephole_container>
		      > find_pane_peephole_containers
		(grid_map_t::lock &grid_lock,
		 const ref<elementObj::implObj> &slider);

	//! Return the size of the pane, or slider.

	//! sliding() adjusts the panes by updating their metrics (same
	//! minimum, preferred, and maximum). This returns the size of the
	//! given element by returning the element's minimum metric.

	virtual dim_t element_size(ONLY IN_THREAD,
				   const ref<elementObj::implObj> &e)=0;

 public:

	//! Override theme_updated().

	//! reference_size_set is no longer. It's a brand new world.
	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
		override;

	//! Override  rebuild_elements_and_update_metrics().

	//! We override set_element_metrics(), and it's our responsibility
	//! to resize the individual panes to match whatever the container's
	//! current size is, now. After calling grid layout manager's
	//! rebuild_elements_and_update_metrics() we take a look at our
	//! pane elements, and adjust_panes_for_current_size().
	bool rebuild_elements_and_update_metrics(ONLY IN_THREAD,
						 grid_map_t::lock &grid_lock,
						 bool already_sized) override;

	//! Override reposition_child_elements().

	//! We also adjust_panes_for_current_size() here.
	bool reposition_child_elements(ONLY IN_THREAD,
				       const rectangle &position,
				       grid_map_t::lock &grid_lock) override;

	//! Implement save()

	void save(ONLY IN_THREAD, const screen_positions &) override;
 private:

	//! Recalculate the size of each individual pane.

	//! If not reference_size_set, take each element_size() and save it
	//! as the reference size.
	//!
	//! Add up all element_size()s. Compute the total size that all panes
	//! must add up to. This would be the container's current size less
	//! the overhead. The overhead consists of the sizes of all dividing
	//! sliders; the extra space in each pane above each pane's
	//! peephole's size; and the size of the border elements in the grid.
	//!
	//! After subtracting all of that, the rest is what we have to work
	//! with. Take all elements' reference sizes, and scale them so that
	//! their total size is this size.
	void adjust_panes_for_current_size(ONLY IN_THREAD,
					   grid_map_t::lock &grid_lock,
					   const rectangle &position);

	//! The most recent size of the pane container.

	//! Used for optimization purposes by adjust_panes_for_current_size()
	//! so that it does its work only when needed.
	dim_t most_recent_pane_size=0;

	//! Return the size of the container given its position.

	//! adjust_panes_for_current_size() uses it to figure out the
	//! most_recent_pane_size().

	virtual dim_t size_from_position(const rectangle &)=0;

	//! Compute this border's element's overhead.

	//! adjust_panes_for_current_size() needs to account for the borders
	//! that exist between the panes and sliders. There are other borders
	//! in the grid besides those. Given the border's element x and y
	//! coordinates, determine whether this border should be accounted
	//! for. If not, return 0. Otherwise return this border's thickness
	//! (height for vertical panes, width for horizontal panes).
	virtual dim_t border_overhead(ONLY IN_THREAD,
				      metrics::grid_xy x,
				      metrics::grid_xy y,
				      const element &e)=0;
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
		create_slider_factory(gridlayoutmanagerObj *public_object,
				      size_t pos) override;

	//! Implement create_factory_for_pos()

	gridfactory create_factory_for_pos(const panelayoutmanager &lm,
					   size_t pos) override;
	//! Implement create_pane_slider_impl().

	ref<pane_sliderObj::implObj>
	create_pane_slider_impl(const container_impl &,
				const current_border_impl &) override;

	//! Implement total_size()

	size_t total_size(const grid_map_t::lock &lock) const override;

	//! Implement get_element()

	element get_element(const grid_map_t::lock &lock, size_t n) override;

	//! Implement pane_peephole_style().

	std::tuple<peephole_style,
		   scrollbar_visibility,
		   scrollbar_visibility>
	pane_peephole_style(scrollbar_visibility) override;

	//! Which pointer to use for the slider.

	const std::string &slider_cursor() const override;

	//! Implement initial_peephole_metrics().

	std::tuple<metrics::horizvert_axi, dim_t>
		initial_peephole_metrics(const dim_arg &size) override;

	//! Find the element in the pane container.
	std::optional<size_t> lookup_element(const grid_map_t::lock &grid_lock,
					     const ref<elementObj::implObj> &e)
		override;

	//! Remove the element.
	void remove_elements(grid_map_t::lock &, size_t n, size_t c) override;

	//! Override set element metrics

	//! Override grid layout manager's vertical metric, for vertical
	//! panes, and horizontal metrics, for horizontal panes.
	//!
	//! A panecontainer_implObj subclass overrides those metrics to the
	//! size specified when creating the pane container.

	void set_element_metrics(ONLY IN_THREAD,
				 const metrics::axis &h,
				 const metrics::axis &v) override;

	//! Implement element_size().

	dim_t element_size(ONLY IN_THREAD,
			   const ref<elementObj::implObj> &e) override;

	//! Implement size_from_position

	dim_t size_from_position(const rectangle &) override;

	//! Slide panes.

	void sliding(ONLY IN_THREAD,
		     grid_map_t::lock &grid_lock,
		     const ref<elementObj::implObj> &which_slider,
		     const pane_slider_original_sizes &original_sizes,

		     coord_t original_x,
		     coord_t original_y,

		     coord_t current_x,
		     coord_t current_y) override;

	//! Implement resize_peephole_to().
	void resize_peephole_to(ONLY IN_THREAD,
				const pane_peephole_container &ppc,
				dim_t s) override;

	//! Implement border_overhead().
	dim_t border_overhead(ONLY IN_THREAD,
			      metrics::grid_xy x,
			      metrics::grid_xy y,
			      const element &e) override;
};

LIBCXXW_NAMESPACE_END

#endif
