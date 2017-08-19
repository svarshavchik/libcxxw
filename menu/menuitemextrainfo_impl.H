/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menuitemextrainfo_impl_h
#define menuitemextrainfo_impl_h

#include "menu/menuitemextrainfoobj.H"
#include "container.H"
#include "container_elementfwd.H"
#include "themedim_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

class LIBCXX_HIDDEN menuitemextrainfoObj::implObj :
	public themedim_elementObj<container_elementObj<child_elementObj>> {

	//! Alias

	typedef themedim_elementObj<
		container_elementObj<child_elementObj>> superclass_t;

public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent);

	//! Destructor
	~implObj();

	//! Override label_theme_font.

	const char *label_theme_font() const override;
};

LIBCXXW_NAMESPACE_END

#endif
