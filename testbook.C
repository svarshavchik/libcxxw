/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/canvas.H"
#include "x/w/text_param_literals.H"
#include "x/w/font.H"

#include <iostream>
#include <sstream>

class close_flagObj : public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef LIBCXX_NAMESPACE::ref<close_flagObj> close_flag_ref;

void name_tab(const LIBCXX_NAMESPACE::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("First name:");

	LIBCXX_NAMESPACE::w::input_field_config config;

	config.autoselect=true;

	f->create_input_field("", config);

	f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("Last name:");
	f->create_input_field("", config);
}

void address_tab(const LIBCXX_NAMESPACE::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("Address:");

	LIBCXX_NAMESPACE::w::input_field_config config;

	config.autoselect=true;

	f->colspan(5).create_input_field("", config);

	f=glm->append_row();

	f->create_canvas();

	f->colspan(5).create_input_field("", config); // address2

	f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("City:");

	config.columns=20;

	f->create_input_field("", config);

	f->create_label("State:");

	config.columns=3;

	f->create_input_field("", config);

	f->create_label("Zip:");

	config.columns=11;

	f->create_input_field("", config);
}

void phone_tab(const LIBCXX_NAMESPACE::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->create_label("Phone:");

	LIBCXX_NAMESPACE::w::input_field_config config;

	config.autoselect=true;

	f->create_input_field("", config);
}

static void create_book(const LIBCXX_NAMESPACE::w::booklayoutmanager &sl)
{
	LIBCXX_NAMESPACE::w::bookpagefactory sf=sl->append();

	sf->halign(LIBCXX_NAMESPACE::w::halign::left)
		.valign(LIBCXX_NAMESPACE::w::valign::top);

	sf->add([]
		(const auto &tab_factory,
		 const auto &page_factory)
		{
			tab_factory->create_label("Address")->show();

			page_factory->create_container
				([&]
				 (const auto &container)
				 {
					 address_tab
						 (container->get_layoutmanager()
						  );
				 },
				 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});
		});

	LIBCXX_NAMESPACE::w::input_fieldptr phone;

	sf->add([]
		(const auto &tab_factory,
		 const auto &page_factory)
		{
			tab_factory->create_label("Phone")->show();
			page_factory->create_container
				([&]
				 (const auto &container)
				 {
					 phone_tab(container
						   ->get_layoutmanager());
				 },
				 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});
		});
	LIBCXX_NAMESPACE::w::input_fieldptr firstname;

	sf=sl->insert(0);

	sf->halign(LIBCXX_NAMESPACE::w::halign::left)
		.valign(LIBCXX_NAMESPACE::w::valign::top);

	sf->add([]
		(const auto &tab_factory,
		 const auto &page_factory)
		{
			tab_factory->create_label("Name")->show();
			page_factory->create_container
				([&]
				 (const auto &container)
				 {
					 name_tab(container
						  ->get_layoutmanager());
				 },
				 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});
		});

	sf=sl->append();

	for (int i=0; i<6; i++)
	{
		sf->add([i]
			(const auto &tab_factory,
			 const auto &page_factory)
			{
				std::ostringstream o;

				o << "Page " << i+1;

				std::string s=o.str();

				tab_factory->create_label(s);

				page_factory->create_label
					({LIBCXX_NAMESPACE::w::font{
							"sans serif",
								36},
							s});
			});
	}
	sl->open(0);

	sl->on_opened
		([]
		 (THREAD_CALLBACK, const auto &info)
		 {
			 auto n=info.opened;

			 auto e=info.lock.layout_manager->get_page(n);

			 if (!e->template isa<LIBCXX_NAMESPACE::w::container>())
				 return;

			 LIBCXX_NAMESPACE::w::container page=e;

			 LIBCXX_NAMESPACE::w::gridlayoutmanager
				 glm=page->get_layoutmanager();

			 LIBCXX_NAMESPACE::w::input_field
				 f=glm->get(0, 1);

			 f->request_focus();
		 });

	LIBCXX_NAMESPACE::w::book_lock lock{sl};

	size_t n=sl->pages();
	std::cout << n << " elements:" << std::endl;

	for (size_t i=0; i<n; ++i)
		std::cout << sl->get_page(i)->objname() << std::endl;
}

static void create_mainwindow(const LIBCXX_NAMESPACE::w::main_window &mw)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager glm=mw->get_layoutmanager();

	auto gf=glm->append_row();
#if 0
	gf->padding(0);
	gf->left_border("book_tab_border");
	gf->right_border("book_tab_border");
	gf->top_border("book_tab_border");

	gf->create_label("Label!");

	gf=glm->append_row();

	gf->create_canvas([](const auto &){},
			  {50, 50, 50}, {10, 10, 10})
		->set_background_color(x::w::rgb{});

	if (0)
		create_book(gf);
#else
	gf->create_focusable_container
		([&]
		 (const auto &s)
		 {
			 LIBCXX_NAMESPACE::w::booklayoutmanager sl=s->get_layoutmanager();

			 create_book(sl);
		 },
		 LIBCXX_NAMESPACE::w::new_booklayoutmanager{});
#endif
}

void testbook()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto configfile=
		LIBCXX_NAMESPACE::configdir("testbook@libcxx.com") + "/windows";

	auto pos=LIBCXX_NAMESPACE::w::load_screen_positions(configfile);
	auto mw=LIBCXX_NAMESPACE::w::main_window::create(pos, "main",
							 []
							 (const auto &mw)
							 {
								 create_mainwindow(mw);
							 });

	mw->set_window_title("Book!");
	mw->show_all();

	guard(mw->connection_mcguffin());

	mw->on_disconnect([]
			  {
				  exit(1);
			  });

	mw->on_delete([close_flag]
		      (THREAD_CALLBACK,
		       const auto &ignore)
		      {
			      close_flag->close();
		      });

	mw->show();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
	pos.clear();
	pos.emplace("main", mw->get_screen_position());
	LIBCXX_NAMESPACE::w::save_screen_positions(configfile, pos);

}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testbook();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
