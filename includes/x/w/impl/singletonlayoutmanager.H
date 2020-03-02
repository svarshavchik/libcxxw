/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_singletonlayoutmanager_h
#define x_w_impl_singletonlayoutmanager_h

#include <x/w/impl/layoutmanager.H>
#include <x/w/singletonlayoutmanagerobj.H>
#include <x/mpobj.H>
#include <deque>

LIBCXXW_NAMESPACE_START

//! \ref singletonlayoutmanager "Singleton layout manager" implementation.

class singletonlayoutmanagerObj::implObj :
	public layoutmanagerObj::implObj {

	//! The container for the current element, and/or new elements.

	//! created() pushes new elements at the end. The connection thread
	//! when it gets around to it will remove the old widget.
	typedef mpobj<std::deque<element>> current_element_t;

	//! My container's current position
	rectangle container_updated_position;

	//! The current widget.

	current_element_t current_element;

	const halign element_halign;
	const valign element_valign;

public:
	//! Constructor
	implObj(const container_impl &container_impl,
		const element &initial_element,
		halign element_halign,
		valign element_valign);

	//! Destructor
	~implObj();

	//! Implement create_public_object().

	layoutmanager create_public_object() override;

	//! Inherited from layoutmanagerObj::implObj

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &e)> &)
		override;

	//! Inherited from layoutmanagerObj::implObj
	size_t num_children(ONLY IN_THREAD) override;

	//! New element, replace the existing one.
	void created(const element &e);

	//! Return the existing widget.

	element get();

	//! Return the existing widget.

	element get(ONLY IN_THREAD);
 private:
	//! Return list_element's implementation object.

	//! Invoking initialize_if_needed()
	element get_list_element(ONLY IN_THREAD);

	//! Return list_element's implementation object.

	//! Invoking initialize_if_needed()
	element_impl get_list_element_impl(ONLY IN_THREAD);

 public:

	//! Override initialize()
	void initialize(ONLY IN_THREAD) override;

	//! Override theme_updated()
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;

	//! Left margin padding.

	//! The default implementation returns 0
	virtual dim_t get_left_padding(ONLY IN_THREAD);

	//! Right margin padding.

	//! The default implementation returns 0
	virtual dim_t get_right_padding(ONLY IN_THREAD);

	//! Top margin padding.

	//! The default implementation returns 0
	virtual dim_t get_top_padding(ONLY IN_THREAD);

	//! Bottom margin padding.

	//! The default implementation returns 0
	virtual dim_t get_bottom_padding(ONLY IN_THREAD);

	//! Implement recalculate()

	//! Computes metrics for the container based on the child element's
	//! metrics and padding and calls update_metrics().
	//!
	//! Calls process_updated_position().
	void recalculate(ONLY IN_THREAD) override;

	//! Adjust factored in child element's metrics.

	//! Before they're used to set our own metrics, this allows the subclass
	//! to adjust them.

	virtual void adjust_child_horiz_vert_metrics(ONLY IN_THREAD,
						     metrics::axis &child_horiz,
						     metrics::axis &child_vert);

	//! Set the container's default metrics.

	//! Takes the metrics computed by recalculate(), and sets its as the
	//! container's metrics.
	virtual void update_metrics(ONLY IN_THREAD,
				    const metrics::axis &horiz,
				    const metrics::axis &vert);

	//! Calculate and position the sole child widget.

	//! The container's metrics should include any padding, so this
	//! would normally end up positioning the current widget according
	//! to the specified padding. If the container is taller, the
	//! widget is vertically centered. If the container is wider, the
	//! widget maintains the set left padding, and the right padding
	//! gets increased.

	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;

private:
	//! Internal recalculate() implementation.
	void recalculate(ONLY IN_THREAD, const element_impl &);
};

LIBCXXW_NAMESPACE_END

#endif
