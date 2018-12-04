/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dialog.H"
#include "x/w/element_state.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/container.H"
#include "main_window.H"
#include "dialog_impl.H"
#include "dialog_handler.H"
#include "x/w/impl/layoutmanager.H"
#include "connection_thread.H"
#include "batch_queue.H"

#include <x/weakcapture.H>

LIBCXXW_NAMESPACE_START

// Most existing users of a standard_dialog_elements_t should
// be passing in an rvalue ref that gets moved into glm->create(), but if
// we get something other than an rvalue ref, delegate this to the proper
// constructor.

void main_windowObj::initialize_theme_dialog(const std::string_view &name,
					     gridtemplate &info)
{
	gridlayoutmanager glm=get_layoutmanager();

	glm->create(name, info);
}

dialogObj::dialogObj(const dialog_args &args)
	: impl{args.impl}, dialog_window{args.dialog_window}
{
}

dialogObj::~dialogObj()=default;

///////////////////////////////////////////////////////////////////////////

dialog main_windowObj
::do_create_dialog(const std::string_view &dialog_id,
		   const function<void (const dialog &)> &creator,
		   const new_layoutmanager &layout_factory,
		   bool modal)
{
	return create_custom_dialog
		(dialog_id,
		 [&]
		 (const dialog_args &args)
		 {
			 auto d=dialog::create(args);

			 creator(d);
			 return d;
		 }, modal, layout_factory);
}

void main_windowObj::do_create_dialog(const std::string_view &dialog_id,
				      const function<external_dialog_creator_t>
				      &creator,
				      const new_layoutmanager &layout_factory,
				      bool modal)
{
	// Keep a batch queue in scope for the duration of the creation,
	// so everything gets buffered up.

	auto s=get_screen();

	auto queue=s->connref->impl->thread->get_batch_queue();

	auto handler=ref<dialogObj::handlerObj>
		::create(s->connref->impl->thread,
			 impl->handler, "dialog_background", modal);

	handler->set_window_type("dialog,normal");

	ptr<dialogObj::implObj> dialog_impl;

	auto [ignored, lm]=create_main_window_impl
		(handler, layout_factory,
		 [&,this](const auto &args)
		 {
			 auto impl=ref<dialogObj::implObj>::create
				 (handler, this->impl->handler, args);

			 dialog_impl=impl;

			 return impl;
		 });

	// Call the creator, to flesh out what's in the dialog.

	ref<dialogObj::implObj> ref_dialog_impl{dialog_impl};

	auto mw=ptrref_base::objfactory<main_window>
		::create(ref_dialog_impl, lm->impl);

	auto d=creator(dialog_args{ref_dialog_impl, mw});

	std::string dialog_ids{dialog_id.begin(), dialog_id.end()};

	// Insert or replace this dialog_id.

	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	auto iter=lock->find(dialog_ids);
	if (lock->find(dialog_ids) != lock->end())
		lock->erase(iter);

	lock->emplace(std::piecewise_construct,
		      std::forward_as_tuple(dialog_ids),
		      std::forward_as_tuple(d));
}

void main_windowObj::remove_dialog(const std::string_view &dialog_id)
{
	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	auto p=lock->find(std::string{dialog_id.begin(), dialog_id.end()});

	if (p != lock->end())
		lock->erase(p);
}

functionref<void (THREAD_CALLBACK, const busy &)
	      > main_windowObj::destroy_when_closed(const std::string_view
						    &dialog_id)
{
	return [me=make_weak_capture(ref(this)),
		dialog_id=std::string{dialog_id.begin(),
				      dialog_id.end()}]
		(THREAD_CALLBACK, const busy &)
	{
		auto got=me.get();

		if (got)
		{
			auto &[me]=*got;

			me->remove_dialog(dialog_id);
		}
	};
}

dialogptr main_windowObj::get_dialog(const std::string_view &dialog_id)
	const
{
	dialogptr d;

	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	auto p=lock->find(std::string{dialog_id.begin(), dialog_id.end()});

	if (p != lock->end())
		d=p->second;

	return d;
}

std::unordered_set<std::string> main_windowObj::dialogs() const
{
	std::unordered_set<std::string> s;

	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	for (const auto &dialogs:*lock)
		s.insert(dialogs.first);

	return s;
}

LIBCXXW_NAMESPACE_END
