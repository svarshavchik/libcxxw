/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemhandle_impl.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/extra_list_row_info.H"
#include "messages.H"
#include <x/weakptr.H>

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

// listitemhandle stores weak references only. Recover strong references.

struct listitemhandleObj::implObj::recover_weak_ref {

	ptr<listlayoutmanagerObj::implObj> listlayout_impl;

	extra_list_row_infoptr extra;

	recover_weak_ref(const implObj &me)
		: listlayout_impl{me.listlayout_impl.getptr()},
		  extra{me.extra.getptr()}
	{
	}

	operator bool() const
	{
		return listlayout_impl && extra;
	}
};

bool listitemhandleObj::implObj::enabled() const
{
	const recover_weak_ref recover{*this};

	if (recover)
	{
		listimpl_info_t::lock lock{recover.listlayout_impl
			->list_element_singleton->impl->textlist_info};

		return recover.extra->enabled(lock);
	}

	return false;
}

void listitemhandleObj::implObj::enabled(bool flag)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	recover.listlayout_impl->list_element_singleton->in_thread
		([me=ref{this}, flag]
		 (ONLY IN_THREAD)
		 {
			 me->enabled(IN_THREAD, flag);
		 });
}

void listitemhandleObj::implObj::enabled(ONLY IN_THREAD, bool flag)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	recover.listlayout_impl->list_element_singleton->impl
		->enabled(IN_THREAD, recover.extra, flag);
}

void listitemhandleObj::implObj::autoselect()
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	recover.listlayout_impl->list_element_singleton->in_thread
		([me=ref{this}]
		 (ONLY IN_THREAD)
		 {
			 me->autoselect(IN_THREAD);
		 });
}

void listitemhandleObj::implObj::autoselect(ONLY IN_THREAD,
					    const callback_trigger_t &trigger)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	auto lm=recover.listlayout_impl->create_listlayoutmanager();

	lm->notmodified();

	create_textlist_info_lock lock{IN_THREAD,
		*lm->impl->list_element_singleton->impl};

	extra_list_row_info extra{recover.extra};

	size_t n=extra->current_row_number(lock);

	if (n < lock->row_infos.size() &&
	    lock->row_infos.at(n).extra == extra)
	{
		lm->impl->list_element_singleton->impl
			->autoselect(IN_THREAD, lm, n, trigger);
	}
}

bool listitemhandleObj::implObj::selected() const
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return false;
	listimpl_info_t::lock lock{recover.listlayout_impl
		->list_element_singleton->impl->textlist_info};
	return recover.extra->selected(lock);
}

void listitemhandleObj::implObj::selected(bool selected_flag)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	recover.listlayout_impl->list_element_singleton->in_thread
		([me=ref{this}, selected_flag]
		 (ONLY IN_THREAD)
		 {
			 me->selected(IN_THREAD, selected_flag);
		 });
}

void listitemhandleObj::implObj::selected(ONLY IN_THREAD, bool selected_flag,
					  const callback_trigger_t &trigger)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	auto lm=recover.listlayout_impl->create_listlayoutmanager();

	lm->notmodified();

	create_textlist_info_lock lock{IN_THREAD,
		*lm->impl->list_element_singleton->impl};

	extra_list_row_info extra{recover.extra};

	size_t n=extra->current_row_number(lock);

	if (n < lock->row_infos.size() &&
	    lock->row_infos.at(n).extra == extra)
	{
		lm->selected(IN_THREAD, n, selected_flag, trigger);
	}
}


void listitemhandleObj::implObj
::on_status_update(const list_item_status_change_callback &cb)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	recover.listlayout_impl->run_as([cb, me=ref{this}]
		  (ONLY IN_THREAD)
		  {
			  me->on_status_update(IN_THREAD, cb);
		  });
}

void listitemhandleObj::implObj
::on_status_update(ONLY IN_THREAD,
		   const list_item_status_change_callback &cb)
{
	const recover_weak_ref recover{*this};

	if (!recover)
		return;

	listlayoutmanager llm=recover.listlayout_impl->create_public_object();

	llm->notmodified();
	recover.listlayout_impl->list_element_singleton->impl
		->on_status_update(IN_THREAD, llm,
				   recover.extra, cb);
}

listlayoutmanagerptr listitemhandleObj::implObj::submenu_listlayout() const
{
	listlayoutmanagerptr ptr;

	const recover_weak_ref recover{*this};

	if (!recover)
		return ptr;

	listimpl_info_t::lock
		lock{recover.listlayout_impl->list_element_singleton->impl
		->textlist_info};

	if (recover.extra->has_submenu(lock))
		ptr=recover.extra->submenu_layoutmanager(lock);

	return ptr;
}

LIBCXXW_NAMESPACE_END
