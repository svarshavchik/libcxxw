/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "switchlayoutmanager_impl.H"
#include "switchfactory_impl.H"
#include "messages.H"
#include "batch_queue.H"

LIBCXXW_NAMESPACE_START

switch_lock::switch_lock(const switchlayoutmanager &lm)
	: lock{lm->impl->info}
{
}

switch_lock::~switch_lock()=default;

switchlayoutmanagerObj::switchlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl),
	  impl(impl)
{
}

switchlayoutmanagerObj::~switchlayoutmanagerObj()=default;

//////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN switchfactoryObj::implObj::appendObj : public implObj {

public:

	using implObj::implObj;

	void created_under_lock(const switch_element_info &info) override
	{
		lm->impl->append(info);
	}
};

switchfactory switchlayoutmanagerObj::append()
{
	return switchfactory::create
		(ref<switchfactoryObj::implObj::appendObj>::create(ref(this)));
}

//////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN switchfactoryObj::implObj::insertObj : public implObj {

public:

	using implObj::implObj;

	size_t n;

	// "under_lock" means something, here.

	void created_under_lock(const switch_element_info &info) override
	{
		lm->impl->insert(n, info);
		++n;
	}
};

switchfactory switchlayoutmanagerObj::insert(size_t n)
{
	auto f=ref<switchfactoryObj::implObj::insertObj>::create(ref(this));

	f->n=n;

	return switchfactory::create(f);
}

void switchlayoutmanagerObj::remove(size_t i)
{
	impl->remove(i);
}

size_t switchlayoutmanagerObj::size() const
{
	switch_layout_info_t::lock lock{impl->info};

	return lock->elements.size();
}

element switchlayoutmanagerObj::get(size_t n) const
{
	switch_layout_info_t::lock lock{impl->info};

	if (n >= lock->elements.size())
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));

	return lock->elements.at(n).the_element;
}


std::optional<size_t> switchlayoutmanagerObj::lookup(const element &e) const
{
	std::optional<size_t> ret;

	switch_layout_info_t::lock lock{impl->info};

	auto iter=lock->element_index.find(e);

	if (iter != lock->element_index.end())
		ret=iter->second;

	return ret;
}

std::optional<size_t> switchlayoutmanagerObj::switched() const
{
	switch_layout_info_t::lock lock{impl->info};

	return lock->current_element;
}

void switchlayoutmanagerObj::switch_to(size_t n)
{
	switch_layout_info_t::lock lock{impl->info};

	if (lock->elements.size() <= n)
		throw EXCEPTION(gettextmsg(_("There are only %1% switchable"
					     " elements"),
					   lock->elements.size()));
	lock->current_element=n;
}

void switchlayoutmanagerObj::switch_off()
{
	queue->run_as([impl=this->impl]
		      (IN_THREAD_ONLY)
		      {
			      switch_layout_info_t::lock lock{impl->info};

			      lock->current_element.reset();
			      impl->needs_recalculation(IN_THREAD);
		      });
}

LIBCXXW_NAMESPACE_END
