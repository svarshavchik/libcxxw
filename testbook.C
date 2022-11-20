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
#include <x/weakptr.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/screen_positions.H"
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
#include "x/w/book_appearance.H"
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

static void prep(const LIBCXX_NAMESPACE::w::bookpagefactory &sf)
{
#if 0
	sf->noncurrent_color=LIBCXX_NAMESPACE::w::lime;
	sf->current_color=LIBCXX_NAMESPACE::w::aqua;
	sf->warm_color=LIBCXX_NAMESPACE::w::yellow;
	sf->active_color=LIBCXX_NAMESPACE::w::white;

	sf->horiz_padding=4;
	sf->label_font="mono"_theme_font;
#endif
}

static void create_book(const LIBCXX_NAMESPACE::w::booklayoutmanager &sl)
{
	LIBCXX_NAMESPACE::w::bookpagefactory sf=sl->append();

	prep(sf);
	sf->halign(LIBCXX_NAMESPACE::w::halign::left)
		.valign(LIBCXX_NAMESPACE::w::valign::top);

	sf->add([]
		(const auto &tab_factory)
		{
			tab_factory->create_label("Address");
		},
		[]
		(const auto &page_factory)
		{
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
		(const auto &tab_factory)
		{
			tab_factory->create_label("Phone");
		},
		[]
		(const auto &page_factory)
		{
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
	prep(sf);
	sf->halign(LIBCXX_NAMESPACE::w::halign::left)
		.valign(LIBCXX_NAMESPACE::w::valign::top);

	sf->add([]
		(const auto &tab_factory)
		{
			tab_factory->create_label("First/Last\nName");
		},
		[]
		(const auto &page_factory)
		{
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
	prep(sf);

	for (int i=0; i<6; i++)
	{
		std::ostringstream o;

		o << "Page " << i+1;

		std::string s=o.str();

		sf->add([&]
			(const auto &tab_factory)
			{
				tab_factory->create_label(s);

			},
			[&]
			(const auto &page_factory)
			{
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
			 if (info.trigger.index() ==
			     LIBCXX_NAMESPACE::w::callback_trigger_initial)
			 {
				 std::cout << "Initial open: "
					   << info.opened
					   << std::endl;
				 return;
			 }

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

	LIBCXX_NAMESPACE::w::new_booklayoutmanager nblm;

	auto custom=nblm.appearance->modify
		([]
		 (const auto &custom)
		 {
			 custom->tabs_background_color=
				 LIBCXX_NAMESPACE::w::white;

		 });

	nblm.appearance=custom;

	// nblm.scroll_button_height=20;
	auto c=gf->colspan(3).create_focusable_container
		([&]
		 (const auto &s)
		 {
			 LIBCXX_NAMESPACE::w::booklayoutmanager sl=s->get_layoutmanager();

			 create_book(sl);
		 },
		 nblm);

	gf=glm->append_row();

	auto b=gf->create_button("Open 1st page");

	b->on_activate([c]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &mcguffin)
		       {
			       LIBCXX_NAMESPACE::w::booklayoutmanager lm=
				       c->get_layoutmanager();

			       lm->open(0);
		       });
	b->show();

	b=gf->create_button("Delete 1st page");

	b->on_activate([c, b=LIBCXX_NAMESPACE::weakptr<LIBCXX_NAMESPACE::w::buttonptr>{b}]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &mcguffin)
		       {
			       auto bb=b.getptr();

			       if (!bb)
				       return;

			       LIBCXX_NAMESPACE::w::booklayoutmanager lm=
				       c->get_layoutmanager();

			       lm->remove(0);
			       bb->set_enabled(IN_THREAD, false);
		       });
	b->show();


	b=gf->create_button("Close", {
			LIBCXX_NAMESPACE::w::normal_button()
				});

	b->on_activate([c]
		       (ONLY IN_THREAD,
			const auto &trigger,
			const auto &mcguffin)
		       {
			       LIBCXX_NAMESPACE::w::booklayoutmanager lm=
				       c->get_layoutmanager();

			       lm->close();
		       });
	b->show();
}

void testbook()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto pos=LIBCXX_NAMESPACE::w::screen_positions::create();

	LIBCXX_NAMESPACE::w::main_window_config config{"main"};

	auto mw=LIBCXX_NAMESPACE::w::main_window::create(config,
							 []
							 (const auto &mw)
							 {
								 create_mainwindow(mw);
							 });

	mw->set_window_title("Book!");

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

	mw->show_all();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	x::property::load_property("x::w::themes", "./themes", true, false);
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
