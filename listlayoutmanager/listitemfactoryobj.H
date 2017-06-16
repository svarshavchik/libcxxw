/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemfactoryobj_h
#define listitemfactoryobj_h

#include "x/w/factoryobj.H"
#include "x/w/gridfactoryfwd.H"
#include "x/w/listlayoutmanager.H"
#include "listlayoutmanager/listitemfactoryfwd.H"
#include "listlayoutmanager/listlayoutstyle.H"

LIBCXXW_NAMESPACE_START

//! The factory object for the list layout manager

//! \see listitemfactory
class LIBCXX_HIDDEN listitemfactoryObj : public factoryObj {

	//! Mutex-protected container for the elements for the new list item

	typedef mpobj<listlayoutstyle::new_list_items_t> new_list_items_t;

	//! New elements for the new list items
	new_list_items_t new_list_items;

public:

	//! My list layout manager.
	const listlayoutmanager me;

	//! Constructor
	listitemfactoryObj(const listlayoutmanager &me);

	//! Destructor
	~listitemfactoryObj();

	//! New element for the new list item was created.

	void created(const element &e) override;

	//! Create a new item from these elements.

	virtual void create_item(const listlayoutstyle::new_list_items_t
				 &new_item)=0;
};

LIBCXXW_NAMESPACE_END

#endif
