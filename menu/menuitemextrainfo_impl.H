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
#include "shortcut/shortcut_activation_elementfwd.H"
#include "x/w/shortcutfwd.H"
#include "activated_in_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

class LIBCXX_HIDDEN menuitemextrainfoObj::implObj :
	public shortcut_activation_elementObj<
	themedim_elementObj<container_elementObj<child_elementObj>>> {

	//! Alias for the non-shortcut parent element.
	typedef themedim_elementObj<
		container_elementObj<child_elementObj>
		> nonshortcut_superclass_t;

	//! Alias

	typedef shortcut_activation_elementObj<nonshortcut_superclass_t
					       > superclass_t;

public:
	//! Parent container with the list layout manager.

	const ref<containerObj::implObj> listlayoutmanager_container;

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent);

	//! Destructor
	~implObj();

	//! Override label_theme_font.

	const char *label_theme_font() const override;

	//! Set up a shortcut

	//! Wrapper for install_shortcut() that gets inherited from the
	//! superclass. The superclass's uninstall_shortcut() gets called
	//! directly.

	void install_shortcut(const shortcut &new_shortcut,
			      const activated_in_thread &public_object);
};

LIBCXXW_NAMESPACE_END

#endif
