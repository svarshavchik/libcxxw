/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemhandle_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

static auto get_extra(const ref<listlayoutmanagerObj::implObj> &layout_impl,
		      size_t n)
{
	listimpl_info_t::lock lock{layout_impl->list_element_singleton->impl
				   ->textlist_info};

	if (n >= lock->row_infos.size())
		throw EXCEPTION(gettextmsg(_("Item %1% does not exist"),
					   n));

	return lock->row_infos.at(n).extra;
}

listitemhandleObj::implObj::implObj(const ref<listlayoutmanagerObj::implObj>
				    &layout_impl,
				    size_t n)
	: implObj{layout_impl, get_extra(layout_impl, n)}
{
}

listitemhandleObj::implObj::implObj(const ref<listlayoutmanagerObj::implObj>
				    &layout_impl,
				    const extra_list_row_info &extra)
	: listlayout_impl{layout_impl}, extra{extra}
{
}

listitemhandleObj::implObj::~implObj()=default;

bool listitemhandleObj::implObj::enabled()
{
	auto p=listlayout_impl.getptr();

	if (p)
	{
		listimpl_info_t::lock lock{p->list_element_singleton->impl
					   ->textlist_info};

		return extra->enabled(lock);
	}

	return false;
}

void listitemhandleObj::implObj::enabled(bool flag)
{
	auto p=listlayout_impl.getptr();

	if (!p)
		return;

	p->list_element_singleton->in_thread
		([me=ref{this}, flag]
		 (ONLY IN_THREAD)
		 {
			 me->enabled(IN_THREAD, flag);
		 });
}

void listitemhandleObj::implObj::enabled(ONLY IN_THREAD, bool flag)
{
	auto p=listlayout_impl.getptr();

	if (!p)
		return;

	listimpl_info_t::lock lock{p->list_element_singleton->impl
				   ->textlist_info};

	p->list_element_singleton->impl->enabled(IN_THREAD, lock, extra, flag);
}

LIBCXXW_NAMESPACE_END
