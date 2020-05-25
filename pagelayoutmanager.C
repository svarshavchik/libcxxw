/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "pagelayoutmanager_impl.H"
#include "pagefactory_impl.H"
#include "messages.H"
#include "batch_queue.H"

LIBCXXW_NAMESPACE_START

page_lock::page_lock(const pagelayoutmanager &lm)
	: lock{lm->impl->info}
{
}

page_lock::~page_lock()=default;

pagelayoutmanagerObj::pagelayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl},
	  impl{impl}
{
}

pagelayoutmanagerObj::~pagelayoutmanagerObj()=default;

//////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN pagefactoryObj::implObj::appendObj : public implObj {

public:

	using implObj::implObj;

	void created_under_lock(const switch_element_info &info) override
	{
		lm->set_modified();
		lm->impl->append(info);
	}
};

pagefactory pagelayoutmanagerObj::append()
{
	return pagefactory::create
		(ref<pagefactoryObj::implObj::appendObj>::create(ref(this)));
}

//////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN pagefactoryObj::implObj::insertObj : public implObj {

public:

	using implObj::implObj;

	size_t n;

	// "under_lock" means something, here.

	void created_under_lock(const switch_element_info &info) override
	{
		lm->set_modified();
		lm->impl->insert(n, info);
		++n;
	}
};

pagefactory pagelayoutmanagerObj::insert(size_t n)
{
	auto f=ref<pagefactoryObj::implObj::insertObj>::create(ref(this));

	f->n=n;

	return pagefactory::create(f);
}

void pagelayoutmanagerObj::remove(size_t i)
{
	modified=true;
	impl->remove(i);
}

size_t pagelayoutmanagerObj::pages() const
{
	notmodified();
	return impl->pages();
}

element pagelayoutmanagerObj::get(size_t n) const
{
	notmodified();

	page_layout_info_t::lock lock{impl->info};

	if (n >= lock->elements.size())
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));

	return lock->elements.at(n).the_element;
}


std::optional<size_t> pagelayoutmanagerObj::lookup(const element &e) const
{
	notmodified();

	std::optional<size_t> ret;

	page_layout_info_t::lock lock{impl->info};

	auto iter=lock->element_index.find(e);

	if (iter != lock->element_index.end())
		ret=iter->second;

	return ret;
}

std::optional<size_t> pagelayoutmanagerObj::opened() const
{
	notmodified();

	page_layout_info_t::lock lock{impl->info};

	return lock->current_element;
}

void pagelayoutmanagerObj::open(size_t n)
{
	modified=true;
	page_layout_info_t::lock lock{impl->info};

	if (lock->elements.size() <= n)
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));
	lock->current_element=n;
}

void pagelayoutmanagerObj::close()
{
	modified=true;
	page_layout_info_t::lock lock{impl->info};

	lock->current_element.reset();
}

LIBCXXW_NAMESPACE_END
