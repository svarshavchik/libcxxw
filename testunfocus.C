/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/property_properties.H>
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/input_field.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/image_button.H"
#include "x/w/canvas.H"
#include "x/w/dialog.H"
#include "x/w/button.H"
#include <x/mpobj.H>

#include <string>
#include <iostream>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

std::atomic<int> counter=0;

mpcobj<int> installed_mcguffin_counter{0};
mpcobj<int> uninstalled_mcguffin_counter{0};

#define TEST_UNFOCUS() ++counter;

#define TEST_INSTALLED_DELAYED_MCGUFFIN() do {				\
		x::mpcobj<int>::lock lock{installed_mcguffin_counter};	\
									\
		++*lock;						\
		lock.notify_all();					\
	} while (0)

#define TEST_UNINSTALL_DELAYED_MCGUFFIN() do {				\
		x::mpcobj<int>::lock lock{uninstalled_mcguffin_counter}; \
									\
		++*lock;						\
		lock.notify_all();					\
	} while (0)

mpobj<bool> resizing_timeout_detected;

#define DEBUG_RESIZING_TIMEOUT() do {		\
		resizing_timeout_detected=true;		\
	} while (0)

#include "focus/focusable_impl.C"
#include "generic_window_handler.C"
#include "connection_threadrunelement.C"

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef ref<close_flagObj> close_flag_ref;

void initfields(const gridlayoutmanager &layout)
{
	for (int i=0; i<10; ++i)
		layout->append_row()->create_input_field("");
}

void testunfocus()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window
		::create([]
			 (const auto &main_window)
			 {
				 initfields(main_window->get_layoutmanager());
			 });

	main_window->set_window_title("Input fields...");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	main_window->hide_all();

	lock.wait_for(std::chrono::seconds(2), [&] { return *lock; });

	auto v=counter.load();

	if (v != 1)
		throw EXCEPTION("unfocus() was called " << v << " times");
}

void testdelayed()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window
		::create([]
			 (const auto &main_window)
			 {
				 gridlayoutmanager glm=
					 main_window->get_layoutmanager();

				 glm->append_row()->create_input_field("");
			 });

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });
	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	{
		gridlayoutmanager glm=
			main_window->get_layoutmanager();

		glm->append_row()->create_input_field("")->request_focus();
	}

	std::cout << "Waiting for delayed focus request." << std::endl;
	{
		mpcobj<int>::lock
			lock{installed_mcguffin_counter};
		lock.wait_for(std::chrono::seconds(3),
			      [&]{ return *lock >= 1; });
	}
	std::cout << "Waiting for completed focus request." << std::endl;
	main_window->show_all();
	{
		mpcobj<int>::lock
			lock{uninstalled_mcguffin_counter};
		lock.wait_for(std::chrono::seconds(3),
			      [&]{ return *lock >= 1; });
	}
	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	if (installed_mcguffin_counter.get() != 1)
		throw EXCEPTION("installed counter is wrong: "
				<< installed_mcguffin_counter.get()
				<< " instead of 1");

	if (uninstalled_mcguffin_counter.get() != 1)
		throw EXCEPTION("uninstalled counter is wrong: "
				<< uninstalled_mcguffin_counter.get()
				<< " instead of 1");
}

focusable_container create_input(const container &c,
				 const close_flag_ref &cf)
{
	auto glm=c->gridlayout();

	glm->remove();

	auto ifld=glm->append_row()->create_focusable_container
		([]
		 (const auto &)
		 {
		 },
		 new_editable_comboboxlayoutmanager{});

	ifld->on_keyboard_focus([cf]
				(ONLY IN_THREAD,
				 focus_change fc,
				 const auto &trigger)
				{
					if (fc == focus_change::gained)
						cf->close();
				});
	return ifld;
}

void testautorestore()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	containerptr radio_container;
	image_buttonptr radio_button;
	buttonptr big_button;

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 auto glm=main_window->gridlayout();

			 canvas_config conf;

			 conf.width={30};
			 conf.height={30};

			 glm->append_row()->create_canvas(conf);
		 });

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });
	main_window->show_all();

	{
		mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait_for(std::chrono::seconds(2), [&] { return *lock; });
	}

	create_dialog_args d_args
		{"testunfocus_dialog@w.libcxx.com", true};

	auto d=main_window->create_dialog
		(d_args,
		 [&]
		 (const auto &d)
		 {
			 auto glm=d->dialog_window->gridlayout();

			 x::w::focusable_containerptr ifld;

			 radio_button=glm->append_row()->create_radio
				 ("radio_button",
				  [&]
				  (const factory &f)
				  {
					  radio_container=f->create_container
						  ([&]
						   (const auto &c)
						   {
							   ifld=create_input
								   (c,
								    close_flag);
						   },
						   new_gridlayoutmanager{});

				  });

			 ifld->get_focus_after(radio_button);
			 ifld->request_focus();
			 big_button=glm->append_row()->create_button("Ok");
		 });

	d->dialog_window->on_state_update
		([close_flag]
		 (ONLY IN_THREAD,
		  const element_state &st,
		  const auto &busy)
		 {
			 if (st.state_update == st.after_hiding)
				 close_flag->close();
		 });

	d->dialog_window->show_all();
	{
		mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait_for(std::chrono::seconds(10), [&] { return *lock; });

		if (!*lock)
			throw EXCEPTION("Keyboard focus not received");

		*lock=false;

		d->dialog_window->in_thread
			([d]
			 (ONLY IN_THREAD)
			 {
				 d->dialog_window->hide(IN_THREAD);
			 });
	}

	{
		mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait_for(std::chrono::seconds(10), [&] { return *lock; });

		if (!*lock)
			throw EXCEPTION("Window not hidden");
		*lock=false;

		radio_container->in_thread
			([=]
			 (ONLY IN_THREAD)
			 {
				 auto ifld=create_input(radio_container,
							close_flag);

				 ifld->get_focus_after(radio_button);
				 ifld->request_focus();
				 d->dialog_window->show_all(IN_THREAD);
			 });
	}

	{
		mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait_for(std::chrono::seconds(10), [&] { return *lock; });

		if (!*lock)
			throw EXCEPTION("Keyboard focus not received 2nd time");
	}
}

void settle_down(const main_window &mw)
{
	mpcobj<bool> flag{false};

	mw->in_thread_idle([&]
			   (ONLY IN_THREAD)
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	});

	mpcobj<bool>::lock lock{flag};
	lock.wait([&]
	{
		return *lock;
	});
}

void testdialog()
{
	destroy_callback::base::guard guard;

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 auto glm=main_window->gridlayout();

			 canvas_config conf;

			 conf.width={30};
			 conf.height={30};

			 glm->append_row()->create_canvas(conf);
		 });

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->show_all();
	settle_down(main_window);

	create_dialog_args d_args
		{"testunfocus_dialog@w.libcxx.com", true};

	auto d=main_window->create_dialog
		(d_args,
		 [&]
		 (const auto &d)
		 {
			 auto glm=d->dialog_window->gridlayout();

			 auto f=glm->append_row();

			 auto combobox=f->create_focusable_container
				 ([]
				  (const auto &c)
				 {
					 auto lm=c->editable_comboboxlayout();

					 lm->replace_all_items
						 ({
							 "Rolem",
							 "Ipsum",
							 "Dolor",
							 "Sit",
							 "Amet",
						 });
				 },
				  new_editable_comboboxlayoutmanager{});
		 });

	d->dialog_window->show_all();
	settle_down(main_window);
	d->dialog_window->hide_all();
	settle_down(main_window);

	static mpcobj<bool> dialog_shown{false};

	d->dialog_window->on_state_update
		([]
		 (ONLY IN_THREAD,
		  const element_state &st,
		  const auto &busy)
		{
			if (st.state_update == st.after_showing)
			{
				mpcobj<bool>::lock lock{dialog_shown};

				*lock=true;
				lock.notify_all();
			}
		});

	main_window->in_thread
		([&]
		 (ONLY IN_THREAD)
		{
			 auto glm=d->dialog_window->gridlayout();

			 glm->remove();
			 auto f=glm->append_row();

			 auto combobox=f->create_focusable_container
				 ([]
				  (const auto &c)
				 {
					 auto lm=c->editable_comboboxlayout();

					 lm->replace_all_items
						 ({
							 "Rolem Ipsum Dolor",
							 "Sit Amet",
						 });
				 },
				  new_editable_comboboxlayoutmanager{});

			d->dialog_window->show_all(IN_THREAD);
		});

	{
		mpcobj<bool>::lock lock{dialog_shown};

		lock.wait_for(std::chrono::seconds(5),
			      [&] { return *lock; });

		if (!*lock)
			throw EXCEPTION("Timed out");
	}
}

int main(int argc, char **argv)
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	try {
		if (argc > 1)
		{
			if (std::string{argv[1]} == "delayed")
			{
				alarm(60);
				testdelayed();
				return 0;
			}

			if (std::string{argv[1]} == "autorestore")
			{
				alarm(60);
				testautorestore();
				return 0;
			}

			if (std::string{argv[1]} == "dialog")
			{
				alarm(60);
				testdialog();

				if (resizing_timeout_detected.get())
					throw EXCEPTION("Resizing timeout"
							" detected");
				return 0;
			}
		}
		testunfocus();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
