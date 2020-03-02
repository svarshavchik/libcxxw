/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "toolboxlayoutmanager_impl.H"
#include "x/w/toolboxfactory.H"
#include "x/w/impl/container.H"
#include "x/w/element.H"
#include "toolboxlayoutmanager/toolboxlayoutmanager_impl.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

toolboxlayoutmanagerObj::toolboxlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl}, impl{impl},
	  toolbox_lock{impl->info}
{
}

toolboxlayoutmanagerObj::~toolboxlayoutmanagerObj()=default;

new_toolboxlayoutmanager::new_toolboxlayoutmanager()=default;

new_toolboxlayoutmanager::~new_toolboxlayoutmanager()=default;

namespace {
#if 0
}
#endif

// Implement common factory virtual methods.

class LIBCXX_HIDDEN toolboxfactory_implObj : public toolboxfactoryObj {

 public:

	const toolboxlayoutmanager lm;

	toolboxfactory_implObj(const toolboxlayoutmanager &lm) : lm{lm}
	{
	}

	~toolboxfactory_implObj()=default;

	container_impl get_container_impl() override
	{
		return lm->impl->layout_container_impl;
	}

	elementObj::implObj &get_element_impl() override
	{
		return get_container_impl()->container_element_impl();
	}

	container_impl last_container_impl() override
	{
		return get_container_impl();
	}
};

//! Append factory.
class LIBCXX_HIDDEN toolboxfactory_append_implObj
	: public toolboxfactory_implObj {

 public:

	toolboxfactory_append_implObj(const toolboxlayoutmanager &lm)
		: toolboxfactory_implObj{lm}
	{
	}

	void created(const element &e) override
	{
		lm->toolbox_lock->elements.push_back(e);
	}
};

//! Insert factory.

class LIBCXX_HIDDEN toolboxfactory_insert_implObj
	: public toolboxfactory_implObj {

 public:

	mpobj<size_t> n;

	toolboxfactory_insert_implObj(const toolboxlayoutmanager &lm,
			       size_t n)
		: toolboxfactory_implObj{lm}, n{n}
	{
	}

	void created(const element &e) override
	{
		mpobj<size_t>::lock lock{n};

		if (*lock >= lm->toolbox_lock->elements.size())
			throw EXCEPTION(gettextmsg
					(_("Element #%1% does not exist"),
					 *lock));

		auto &es=lm->toolbox_lock->elements;
		es.insert(es.begin()+*lock, e);
		++*lock;
	}
};

#if 0
{
#endif
}

toolboxfactory toolboxlayoutmanagerObj::append_tools()
{
	return ref<toolboxfactory_append_implObj>::create(ref{this});
}

toolboxfactory toolboxlayoutmanagerObj::insert_tools(size_t n)
{
	return ref<toolboxfactory_insert_implObj>::create(ref{this}, n);
}

void toolboxlayoutmanagerObj::remove_tool(size_t n)
{
	remove_tools(n, 1);
}

void toolboxlayoutmanagerObj::remove_tools(size_t first_tool, size_t n)
{
	size_t s=size();

	if (first_tool >= s)
		throw EXCEPTION(gettextmsg(_("Element #%1% does not exist"),
					   first_tool));
	if (s-first_tool < n)
		throw EXCEPTION(gettextmsg(_("Element #%1% does not exist"),
					   s));

	auto &es=toolbox_lock->elements;

	auto b=es.begin();

	es.erase(b+first_tool, b+(first_tool+n));
}

size_t toolboxlayoutmanagerObj::size() const
{
	return toolbox_lock->elements.size();
}

LIBCXXW_NAMESPACE_END
