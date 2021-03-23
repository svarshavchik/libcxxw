/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/weakcapture.H>
#include <x/threads/run.H>

#include "x/w/main_window.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
#include "x/w/canvas.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/focusable_label.H"
#include "x/w/input_field_lock.H"
#include "x/w/shortcut.H"
#include "x/w/singletonlayoutmanager.H"
#include "x/w/element_state.H"
#include <string>
#include <iostream>
#include <algorithm>
#include <utility>
#include <set>
#include "testcombobox.inc.H"

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

typedef mpobj<std::set<rectangle>> all_sizes_t;

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	all_sizes_t all_sizes;

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

static const char *moretext[]={
	"incididunt ut",
	"labore et dolore",
	"magna"
	"aliqua",
	"Ut enim ad"
	"minim veniam"
	"quis nostrud"
	"exercitation"
	"ullamco",
	"laboris nisi",
	"ut aliquip",
	"ex ea",
	"commodo consequat",
	"Duis aute",
	"irure dolor",
	"in reprehenderit",
	"in voluptate",
	"velit"
	"esse",
	"cillum dolore",
	"eu fugiat",
	"nulla pariatur",
	"Excepteur sint",
	"occaecat cupidatat",
	"non proident",
	"sunt in",
	"culpa qui",
	"officia deserunt",
	"mollit anim",
	"id est"
	"laborum"
};

focusable_container create_combobox(const factory &f,
				    const new_custom_comboboxlayoutmanager &ncc)
{
	return f->create_focusable_container
		([]
		 (const auto &new_container) {

			 new_container->in_thread_idle
				 ([new_container]
				  (ONLY IN_THREAD)
				  {
					  standard_comboboxlayoutmanager lm=new_container
						  ->get_layoutmanager();

					  lm->replace_all_items
						  ({
						    {"Lorem ipsum"},
						    {"dolor sit"},
						    {"ament"},
						    {"consectetur"},
						    {"adipisicing"},
						    {"elid set"},
						    {"do"},
						    {"eiusmod tempor"},
						  });
				  });
		}, ncc);
}

focusable_container create_editable_combobox(const factory &f)
{
	new_editable_comboboxlayoutmanager ec{
		[] (THREAD_CALLBACK, const auto &info)
		{
			if (info.list_item_status_info.selected)
				std::cout << "Selected item #"
					  << info.list_item_status_info.item_number
					  << std::endl;
		}
	};

	//	auto c=ec.appearance->clone();
	//
	//	c->list_font=x::w::theme_font{"mono"};
	//	ec.appearance=c;
	return create_combobox(f, ec);
}

focusable_container create_standard_combobox(const factory &f)
{
	new_standard_comboboxlayoutmanager sc{
		[](ONLY IN_THREAD,
		   const auto &info)
		{
			if (info.list_item_status_info.selected)
				std::cout << "Selected item #"
					  << info.list_item_status_info
					.item_number
					  << std::endl;
		}
	};

	return create_combobox(f, sc);
}

typedef LIBCXX_NAMESPACE::mpobj<std::vector<std::tuple<size_t, bool>>
				> callback_test_t;

callback_test_t callback_test;

focusable_container create_standard_combobox2(const factory &f)
{
	new_standard_comboboxlayoutmanager sc{
		[](ONLY IN_THREAD,
		   const auto &info)
		{
			callback_test_t::lock lock{callback_test};

			lock->emplace_back(info.list_item_status_info
					   .item_number,
					   info.list_item_status_info.selected);
		}
	};

	return create_combobox(f, sc);
}

static void do_resort(ONLY IN_THREAD,
		      const focusable_container &combobox,
		      const button &b)
{
	b->set_enabled(IN_THREAD, false);
	x::run_lambda
		([=]
		 {
			 for (int i=5; i>0; --i)
			 {
				 std::ostringstream o;

				 o << "Resort in " << i << "...";

				 b->get_layoutmanager()->replace()
					 ->create_label(o.str())->show();

				 sleep(1);
			 }

			 b->get_layoutmanager()->replace()
				 ->create_label("Resort")->show();

			 b->set_enabled(true);

			 LIBCXX_NAMESPACE::w::listlayoutmanager lm=
				 combobox->get_layoutmanager();

			 std::vector<size_t> n;

			 n.resize(lm->size());

			 std::generate(n.begin(), n.end(),
				       [n=0]
				       ()
				       mutable
				       {
					       return n++;
				       });

			 std::random_shuffle(n.begin(), n.end());

			 lm->resort_items(n);
		 });
}

gridlayoutmanager create_book(const x::w::focusable_container &c)
{
	gridlayoutmanagerptr glm;

	booklayoutmanager blm=c->get_layoutmanager();

	auto f=blm->append();

	f->add("Initial",
	       []
	       (const auto &f)
	       {
		       f->create_container
			       ([]
				(const auto &c)
				{
				},
				new_gridlayoutmanager{});
	       });

	blm->open(0);
	f->add("Main",
	       [&]
	       (const auto &f)
	       {
		       f->create_container
			       ([&]
				(const auto &c)
				{
					glm=c->get_layoutmanager();
				},
				new_gridlayoutmanager{});
	       });

	return glm;
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

void testcombobox(const testcombobox_options &options)
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::focusable_containerptr combobox;

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager layout
				 {
				  main_window->get_layoutmanager()
				 };


			 if (options.book->value)
			 {
				 auto f=layout->append_row();

				 f->create_focusable_container
					 ([&]
					  (const auto &c)
					  {
						  layout=create_book(c);
					  },
					  new_booklayoutmanager{});
			 }

			 auto factory=layout->append_row();

			 if (options.editable->value)
				 combobox=create_editable_combobox(factory);
			 else if (options.callback->value)
				 combobox=create_standard_combobox2(factory);
			 else
				 combobox=create_standard_combobox(factory);

			 combobox->create_tooltip("Tooltip!");
			 factory=layout->append_row();

			 factory->halign(halign::fill)
				 .create_button
				 ({"Append"},{
					 LIBCXX_NAMESPACE::w::shortcut{"Alt",
							 'A'}
				 })
				 ->on_activate
				 ([combobox, i=0](ONLY IN_THREAD,
						  const auto &,
						  const auto &)
				  mutable
				  {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  lm->append_items({moretext[i]});

					  i=(i+1) % (sizeof(moretext)/
						     sizeof(moretext[0]));
					  std::cout << "Appended" << std::endl;
				  });

			 factory=layout->append_row();

			 factory->halign(halign::fill)
				 .create_button("Delete")
				 ->on_activate
				 ([combobox](THREAD_CALLBACK,
					     const auto &,
					     const auto &)
				  {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  if (lm->size() == 0)
						  return;

					  lm->remove_item(0);
				  });

			 factory=layout->append_row();

			 factory->halign(halign::center)
				 .create_button
				 ({"Append Separator"})
				 ->on_activate
				 ([combobox](THREAD_CALLBACK,
					     const auto &, const auto &)
				  {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  lm->append_items
						  (
						   {LIBCXX_NAMESPACE::w::
								   separator{}
						   });
				  });

			 factory=layout->append_row();

			 factory->halign(halign::center)
				 .create_button
				 ({"Insert Separator"})
				 ->on_activate
				 ([combobox](ONLY IN_THREAD,
					     const auto &,
					     const auto &)
				  {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  lm->insert_items
						  (0,
						   {LIBCXX_NAMESPACE::w
								   ::separator{}
						   });
				  });

			 factory=layout->append_row();

			 factory->halign(halign::center)
				 .create_button
				 ({"Disable/Enable 1st item"})
				 ->on_activate
				 ([combobox](THREAD_CALLBACK,
					     const auto &, const auto &) {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  lm->enabled(0, !lm->enabled(0));
				  });

			 factory=layout->append_row();

			 factory->halign(halign::center)
				 .create_button
				 ({"Replace First"})
				 ->on_activate
				 ([combobox](ONLY IN_THREAD,
					     const auto &,
					     const auto &)
				  {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  if (lm->size() == 0)
						  return;

					  lm->replace_items(0,
							    {"1st item"});
				  });

			 factory=layout->append_row();

			 factory->halign(halign::center)
				 .create_button
				 ({"Replace All"})
				 ->on_activate
				 ([combobox](ONLY IN_THREAD,
					     const auto &,
					     const auto &)
				  {
					  standard_comboboxlayoutmanager lm=
						  combobox->get_layoutmanager();

					  lm->replace_all_items
						  ({
						    {"Lorem ipsum"},
						    {"dolor sit"},
						    {"ament"},
						    {"consectetur"},
						    {"adipisicing"},
						    {"elid set"},
						    {"do"},
						    {"eiusmod tempor"},
						  });
				  });

			 factory=layout->append_row();

			 auto resort=factory->halign(halign::center)
				 .create_button
				 ({"Resort"});

			 resort->on_activate
				 ([combobox,
				   resort=make_weak_capture(resort)]
				  (ONLY IN_THREAD,
				   const auto &, const auto &)
				  {
					  auto got=resort.get();

					  if (!got)
						  return;

					  auto &[resort]=*got;

					  do_resort(IN_THREAD,
						    combobox, resort);
				  });
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	if (options.callback->value)
	{
		settle_down(main_window);
		{
			standard_comboboxlayoutmanager lm=
				combobox->get_layoutmanager();
			lm->autoselect(1);
			lm->autoselect(3);
		}

		settle_down(main_window);
		callback_test_t::lock lock{callback_test};
		for (const auto &[index, status]:*lock)
		{
			std::cout << index << ":" << status << std::endl;
		}

		if (*lock != std::vector<std::tuple<size_t, bool>>{
				{1, true},
				{1, false},
				{3, true}
			})
		{
			std::cout << "Expected combo-boxes to deselect before"
				" selecting" << std::endl;
			exit(1);
		}
	}
	else
	{
		mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait([&] { return *lock; });
	}
	standard_comboboxlayoutmanager lm=combobox->get_layoutmanager();

	auto n=lm->selected();

	if (n)
	{
		size_t i=n.value();

		std::cout << "Final selection: " << i << std::endl;
	}

	if (options.editable->value)
	{
		editable_comboboxlayoutmanager lm=combobox->get_layoutmanager();

		lm->notmodified();
		std::cout << "Final selection: " << lm->get() << std::endl;
	}
}

static void reset_layout(const gridlayoutmanager &glm)
{
	auto factory=glm->insert_row(0);

	factory->create_focusable_container
		([]
		 (const auto &new_container)
		 {
			 editable_comboboxlayoutmanager lm=
				 new_container->get_layoutmanager();

			 lm->replace_all_items
				 ({
				   {"Lorem ipsum"},
				   {"dolor sit"},
				   {"consectetur adipisicing elid set do"},
				 });
		 },
		 new_editable_comboboxlayoutmanager{});
}

void testcombobox_layout()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager layout
				 {
				  main_window->get_layoutmanager()
				 };

			 reset_layout(layout);
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});


	main_window->set_window_title("Ten seconds");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
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

	lock.wait_for(std::chrono::seconds(5),
		      [&] { return *lock; });

	if (*lock)
		return;

	main_window->on_state_update
		([close_flag]
		 (ONLY IN_THREAD,
		  const auto &s,
		  const auto &busy)
		 {
			 all_sizes_t::lock lock{close_flag->all_sizes};
			 lock->insert(s.current_position);
		 });

	{
		gridlayoutmanager glm=
			main_window->get_layoutmanager();
		glm->remove_rows(0, 1);

		reset_layout(glm);
		main_window->show_all();
	}
	lock.wait_for(std::chrono::seconds(5),
		      [&] { return *lock; });

	auto n=close_flag->all_sizes.get().size();

	if (n != 1)
		throw EXCEPTION("Unexpected resize");
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testcombobox_options options;

		options.parse(argc, argv);

		if (options.layout->value)
			testcombobox_layout();
		else
			testcombobox(options);
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
