/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemlayoutmanager_impl_h
#define listitemlayoutmanager_impl_h

#include "listlayoutmanager/listitemlayoutmanager.H"
#include "themedimfwd.H"
#include "connection_threadfwd.H"
#include "layoutmanager.H"
#include "x/w/elementfwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! List item layout manager implementation object.

//! \see listlayoutmanager

class LIBCXX_HIDDEN listitemlayoutmanagerObj::implObj
	: public layoutmanagerObj::implObj {

	//! The current element in this list item cell.
	element list_element_thread_only;

	//! Horizontal padding.
	const themedim left_padding;

	//! Horizontal padding.
	const themedim right_padding;

	//! Top and bottom padding
	const themedim v_padding;

	//! Return list_element's implementation object.

	//! Invoking initialize_if_needed()
	ref<elementObj::implObj> get_list_element_impl(IN_THREAD_ONLY);

 public:
	THREAD_DATA_ONLY(list_element);

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl,
		const element &initial_element,
		const themedim &left_padding,
		const themedim &right_padding,
		const themedim &v_padding);

	//! Destructor
	~implObj();

	//! Inherited from layoutmanagerObj::implObj

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void (const element &e)> &)
		override;

	//! Inherited from layoutmanagerObj::implObj

	layoutmanager create_public_object() override;

	//! Inherited from layoutmanagerObj::implObj

	void recalculate(IN_THREAD_ONLY) override;

	//! Inherited from layoutmanagerObj::implObj

	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position)
		override;

	//! Inherited from layoutmanagerObj::implObj

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;
};

LIBCXXW_NAMESPACE_END

#endif
