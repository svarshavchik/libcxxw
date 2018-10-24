/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/element.H"
#include "screen.H"
#include "batch_queue.H"
#include "x/w/impl/background_color.H"
#include "generic_window_handler.H"
#include "x/w/impl/draw_info.H"
#include "busy.H"
#include "menu/menu_popup.H"
#include "activated_in_thread.H"
#include "x/w/picture.H"
#include "x/w/main_window.H"
#include "x/w/container.H"
#include "shortcut/independent_shortcut_activation.H"
#include "activated_in_thread.H"
#include "run_as.H"
#include <x/weakptr.H>
#include <x/weakcapture.H>
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

elementObj::elementObj(const ref<implObj> &impl)
	: impl(impl)
{
}

elementObj::~elementObj()
{
	impl->removed_from_container();
}

ref<obj> elementObj::connection_mcguffin() const
{
	return get_screen()->connection_mcguffin();
}

main_windowptr elementObj::get_main_window() const
{
	return impl->get_window_handler().get_main_window();
}

screen elementObj::get_screen()
{
       return impl->get_screen();
}

const_screen elementObj::get_screen() const
{
       return impl->get_screen();
}

bool elementObj::selection_has_owner() const
{
	return selection_has_owner(impl->default_cut_paste_selection());
}

bool elementObj::selection_has_owner(const std::string_view &selection) const
{
	return get_screen()->selection_has_owner(selection);
}

bool elementObj::selection_can_be_received() const
{
	auto mw=get_main_window();

	return mw && mw->selection_can_be_received();
}

void elementObj::receive_selection()
{
	selection_has_owner(impl->default_cut_paste_selection());
}

void elementObj::receive_selection(const std::string_view &selection)
{
	in_thread([me=ref{this}, selection=std::string{selection}]
		  (ONLY IN_THREAD)
		  {
			  me->receive_selection(IN_THREAD, selection);
		  });
}

void elementObj::receive_selection(ONLY IN_THREAD)
{
	return receive_selection(IN_THREAD,
				 impl->default_cut_paste_selection());
}

void elementObj::receive_selection(ONLY IN_THREAD,
				   const std::string_view &selection)
{
	auto mw=get_main_window();

	if (mw)
		mw->receive_selection(IN_THREAD, selection);
}

bool elementObj::cut_or_copy_selection(cut_or_copy_op op)
{
	return cut_or_copy_selection
		(op, impl->default_cut_paste_selection());
}

bool elementObj::cut_or_copy_selection(cut_or_copy_op op,
				       const std::string_view &selection)
{
	auto mw=get_main_window();

	return mw && mw->cut_or_copy_selection(op, selection);
}

bool elementObj::cut_or_copy_selection(ONLY IN_THREAD, cut_or_copy_op op)
{
	return cut_or_copy_selection
		(IN_THREAD, op, impl->default_cut_paste_selection());
}

bool elementObj::cut_or_copy_selection(ONLY IN_THREAD, cut_or_copy_op op,
				       const std::string_view &selection)
{
	auto mw=get_main_window();

	return mw && mw->cut_or_copy_selection(IN_THREAD, op, selection);
}

void elementObj::show_all()
{
	impl->request_visibility_recursive(true);
}

void elementObj::hide_all()
{
	impl->request_visibility_recursive(false);
}

void elementObj::show()
{
	impl->request_visibility(true);
}

void elementObj::hide()
{
	impl->request_visibility(false);
}

void elementObj::stop_message(const text_param &t)
{
	impl->stop_message(t);
}

void elementObj::exception_message(const exception &e)
{
	impl->exception_message(e);
}

void elementObj::set_background_color(const color_arg &name)
{
	impl->set_background_color(name);
}

void elementObj::remove_background_color()
{
	impl->remove_background_color();
}

void elementObj::on_state_update(const functionref<element_state_callback_t>
				 &cb)
{
	impl->on_state_update(cb);
}

void elementObj::on_state_update(ONLY IN_THREAD,
				 const functionref<element_state_callback_t>
				 &cb)
{
	impl->on_state_update(IN_THREAD, cb);
}

void elementObj::on_metrics_update(const functionref<metrics_update_callback_t>
				   &cb)
{
	impl->on_metrics_update(cb);
}

void elementObj::on_pointer_focus(const functionref<focus_callback_t> &cb)
{
	impl->on_pointer_focus(cb);
}

void elementObj::on_button_event(const functionref<button_event_callback_t> &cb)
{
	in_thread([cb, impl=this->impl]
		  (ONLY IN_THREAD)
		  {
			  impl->data(IN_THREAD).on_button_event_callback=cb;
		  });
}

void elementObj::create_custom_tooltip(const functionref<void
				       (THREAD_CALLBACK,
					const tooltip_factory &)>
				       &tooltip_factory) const
{
	impl->THREAD->run_as
		([impl=this->impl, tooltip_factory]
		 (ONLY IN_THREAD)
		 {
			 impl->data(IN_THREAD).tooltip_factory=tooltip_factory;
		 });
}


void elementObj::remove_tooltip() const
{
	impl->THREAD->run_as
		([impl=this->impl]
		 (ONLY IN_THREAD)
		 {
			 impl->data(IN_THREAD).tooltip_factory=nullptr;
		 });
}

container elementObj
::do_create_popup_menu(const function<void (const listlayoutmanager &)>
		       &creator)
	const
{
	return contextmenu_popup(impl, creator);
}

void elementObj::install_contextpopup_callback
(const functionref<install_contextpopup_callback_t> &callback)
{
	install_contextpopup_callback(callback, {});
}

namespace {
#if 0
}
#endif

//! Shortcut activation for context popups.

//! Invokes the context popup callback.

class LIBCXX_HIDDEN contextpopup_shortcut_activatorObj
	: public activated_in_threadObj {

 public:
	const functionref<install_contextpopup_callback_t> callback;

	const weakptr<elementptr> weake;

	contextpopup_shortcut_activatorObj(const functionref<
					   install_contextpopup_callback_t>
					   &callback,
					   const element &e)
		: callback{callback}, weake{e}
	{
	}

	~contextpopup_shortcut_activatorObj()=default;

	//! Shortcut activated.

	void activated(ONLY IN_THREAD, const callback_trigger_t &trigger)
		override
	{
		auto e=weake.getptr();

		if (!e)
			return;

		element stronge{e};

		auto window=stronge->get_main_window();

		if (!window)
			return;

		busy_impl yes_i_am{*stronge->impl};

		callback(IN_THREAD, stronge, trigger, yes_i_am);
	}

	//! If the attached-to element is visible, the shortcut is enabled.

	bool enabled(ONLY IN_THREAD) override
	{
		auto eptr=weake.getptr();

		if (!eptr)
			return false;

		return eptr->impl->data(IN_THREAD).logical_inherited_visibility;
	}
};

#if 0
{
#endif
}

void elementObj::install_contextpopup_callback
(const functionref<install_contextpopup_callback_t> &callback,
 const shortcut &sc)
{

	impl->THREAD->run_as
		([me=ref(this), callback, sc]
		 (ONLY IN_THREAD)
		 {
			 independent_shortcut_activationptr sc_impl;
			 ptr<contextpopup_shortcut_activatorObj> sc_active;

			 // If there's a shortcut we set it up. This gets
			 // captured by the installed callback, so it remains
			 // in scope until remove_contextpopup_callback().

			 if (sc)
			 {
				 auto i=independent_shortcut_activation
					 ::create(ref(&me->impl->
						      get_window_handler()));
				 auto a=ref<contextpopup_shortcut_activatorObj>
					 ::create(callback, me);

				 i->install_shortcut(sc, a, false);

				 sc_impl=i;
				 sc_active=a;
			 }

			 me->impl->data(IN_THREAD).contextpopup_callback=
				 [callback,
				  sc_impl,
				  sc_active,
				  me=make_weak_capture(me)]
				 (ONLY IN_THREAD,
				  const auto &trigger,
				  const auto &mcguffin)
				 {
					 auto got=me.get();

					 if (!got)
						 return;

					 auto &[me]=*got;

					 auto window=me->get_main_window();

					 if (!window)
						 return;

					 try {
						 callback(IN_THREAD,
							  me, trigger,
							  mcguffin);
					 } REPORT_EXCEPTIONS(window);
				 };
		 });
}

void elementObj::remove_contextpopup_callback() const
{
	impl->THREAD->run_as
		([impl=this->impl]
		 (ONLY IN_THREAD)
		 {
			 impl->data(IN_THREAD).contextpopup_callback=nullptr;
		 });
}

ref<obj> elementObj::get_shade_busy_mcguffin() const
{
	return impl->get_window_handler().get_shade_busy_mcguffin();
}

ref<obj> elementObj::get_wait_busy_mcguffin() const
{
	return impl->get_window_handler().get_wait_busy_mcguffin();
}

ref<elementObj::implObj> elementObj::get_minimum_override_element_impl()
{
	return impl;
}

void elementObj::in_thread(const functionref<void (THREAD_CALLBACK)> &cb)
{
	get_screen()->get_connection()->in_thread(cb);
}

void elementObj::in_thread_idle(const functionref<void (THREAD_CALLBACK)> &cb)
{
	get_screen()->get_connection()->in_thread_idle(cb);
}

std::ostream &operator<<(std::ostream &o, const element_state &s)
{
	return o << "state update: " << (int)s.state_update
		 << ", shown=" << s.shown
		 << ", position: " << s.current_position << std::endl;
}

///////////////////////////////////////////////////////////////////

std::pair<coord_t, coord_t> draw_info::background_xy_to(coord_t x,
							coord_t y,
							coord_t offset_x,
							coord_t offset_y) const
{
	coord_squared_t::value_type x2=coord_t::value_type(x)
		- coord_t::value_type(background_x);

	coord_squared_t::value_type y2=coord_t::value_type(y)
		- coord_t::value_type(background_y);

	x2 += coord_t::value_type(offset_x);
	y2 += coord_t::value_type(offset_y);

	return { coord_t::truncate(x2), coord_t::truncate(y2) };
}
LIBCXXW_NAMESPACE_END
