/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dialog.H"
#include "x/w/element_state.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/container.H"
#include "screen_positions_impl.H"
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

void main_windowObj::generate(const std::string_view &name,
			      uielements &info)
{
	gridlayout()->generate(name, info);
}

dialogObj::dialogObj(const dialog_args &args)
	: impl{args.impl}, dialog_window{args.dialog_window}
{
}

dialogObj::~dialogObj()=default;

void dialogObj::set_dialog_position(dialog_position pos)
{
	dialog_window->in_thread
		([handler=impl->handler, pos]
		 (ONLY IN_THREAD)
		 {
			 // We can reset the dialog position only if
			 // the dialog is not visible and is not in the process
			 // of being shown.
			 if (handler->data(IN_THREAD)
			     .reported_inherited_visibility ||
			     handler->data(IN_THREAD).requested_visibility)
				 return;

			 handler->suggested_position(IN_THREAD).reset();
			 handler->my_position(IN_THREAD)=pos;

			 // Reset the state of the dialog, so it gets
			 // resized to the default position the next time
			 // it's made visible.
			 handler->ok_to_publish_hints(IN_THREAD)=false;
			 handler->preferred_dimensions_set(IN_THREAD)=false;
		 });
}

void standard_dialog_args::restore(dialog_position my_position,
				   const std::string_view &name_arg)
{
	name=name_arg;
	position=my_position;
}

void standard_dialog_args::restore(dialog_position my_position)
{
	position=my_position;
}

///////////////////////////////////////////////////////////////////////////

dialog main_windowObj
::do_create_dialog(const create_dialog_args &args,
		   const function<void (const dialog &)> &creator)
{
	return create_custom_dialog
		(args,
		 [&]
		 (const dialog_args &args)
		 {
			 auto d=dialog::create(args);

			 creator(d);
			 return d;
		 });
}

void main_windowObj::do_create_dialog(const create_dialog_args &args,
				      const function<external_dialog_creator_t>
				      &creator)
{
	// Keep a batch queue in scope for the duration of the creation,
	// so everything gets buffered up.

	auto s=get_screen();

	auto queue=s->connref->impl->thread->get_batch_queue();

	std::optional<rectangle> initial_pos;

	if (args.position == dialog_position::default_position)
	{
		auto window_info=impl->handler->positions
			->impl->find_window_position(args.name);

		if (window_info)
			initial_pos=window_info->coordinates;
	}

	auto handler=ref<dialogObj::handlerObj>
		::create(impl->handler,
			 args.position,
			 initial_pos,
			 args.name,
			 impl->handler->positions,
			 args.dialog_background,
			 args.appearance,
			 "dialog,normal",
			 args.modal, args.urgent,
			 args.grab_input_focus);

	ptr<dialogObj::implObj> dialog_impl;

	std::optional<new_gridlayoutmanager> default_lm;

	auto [ignored, lm]=create_main_window_impl
		(handler, std::nullopt, std::nullopt, std::nullopt,
		 main_window_config{},
		 args.dialog_layout ? args.dialog_layout->get()
		 : static_cast<const new_layoutmanager &>
		 (default_lm.emplace()),
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

	std::string dialog_ids{args.dialog_id.begin(), args.dialog_id.end()};

	// Insert or replace this dialog_id.

	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	auto iter=lock->find(dialog_ids);
	if (iter != lock->end())
		lock->erase(iter);

	lock->emplace(std::piecewise_construct,
		      std::forward_as_tuple(dialog_ids),
		      std::forward_as_tuple(d));
}

void main_windowObj::remove_dialog(const std::string_view &dialog_id)
{
	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	// TODO: std::string should not be necessary in C++20
	auto p=lock->find(std::string{dialog_id.begin(), dialog_id.end()});

	if (p != lock->end())
		lock->erase(p);
}

ok_cancel_dialog_callback_t
main_windowObj::destroy_when_closed(const std::string_view &dialog_id)
{
	return [dialog_id=std::string{dialog_id.begin(),
				      dialog_id.end()}]
		(THREAD_CALLBACK, const auto &args)
	       {
		       if (args.dialog_main_window)
			       args.dialog_main_window->remove_dialog
				       (dialog_id);
	       };
}

dialogptr main_windowObj::get_dialog(const std::string_view &dialog_id)
	const
{
	dialogptr d;

	implObj::all_dialogs_t::lock lock{impl->all_dialogs};

	// TODO: std::string should not be necessary in C++20
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
