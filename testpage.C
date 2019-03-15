/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/pagefactory.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/input_field.H"
#include "x/w/input_field_config.H"
#include "x/w/canvas.H"

#include <iostream>
#include <unordered_map>

static std::unordered_map<std::string, size_t> filenames_loaded;

#define SXG_PARSER_CONSTRUCTOR_TEST() do {\
		++filenames_loaded[filename];				\
	} while (0)

#include "editor_impl.H"
#include "sxg/sxg_parser.C"

static LIBCXX_NAMESPACE::w::editorObj::implObj *editor_impl_constructed;
static LIBCXX_NAMESPACE::w::editorObj::implObj *editor_impl_with_focus;

static LIBCXX_NAMESPACE::w::editorObj::implObj *editor_impl_name;
static LIBCXX_NAMESPACE::w::editorObj::implObj *editor_impl_address;

#define EDITOR_CONSTRUCTOR_DEBUG() do {		\
		editor_impl_constructed=this;	\
	} while(0)

#define EDITOR_FOCUS_DEBUG() do {			\
		if (current_keyboard_focus(IN_THREAD))	\
			editor_impl_with_focus=this;	\
	} while (0)

#include "editor_impl.C"

#include "testpage.inc.H"

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

auto name_tab(const LIBCXX_NAMESPACE::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("First name:");

	LIBCXX_NAMESPACE::w::input_field_config config;

	config.autoselect=true;

	auto firstname=f->create_input_field("", config);

	editor_impl_name=editor_impl_constructed;

	f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("Last name:");
	f->create_input_field("", config);

	return firstname;
}

auto address_tab(const LIBCXX_NAMESPACE::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->halign(LIBCXX_NAMESPACE::w::halign::right)
		.create_label("Address:");

	LIBCXX_NAMESPACE::w::input_field_config config;

	config.autoselect=true;

	auto address1=f->colspan(5).create_input_field("", config);

	editor_impl_address=editor_impl_constructed;

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

	return address1;
}

auto phone_tab(const LIBCXX_NAMESPACE::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->create_label("Phone:");

	LIBCXX_NAMESPACE::w::input_field_config config;

	config.autoselect=true;

	return f->create_input_field("", config);
}

static auto create_page(const LIBCXX_NAMESPACE::w::pagelayoutmanager &sl)
{
	LIBCXX_NAMESPACE::w::pagefactory sf=sl->append();

	LIBCXX_NAMESPACE::w::input_fieldptr address1;

	auto element1=sf->halign(LIBCXX_NAMESPACE::w::halign::left)
		.valign(LIBCXX_NAMESPACE::w::valign::top)
		.create_container
		([&]
		 (const auto &container)
		 {
			 address1=address_tab(container->get_layoutmanager());
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	LIBCXX_NAMESPACE::w::input_fieldptr phone;

	auto element2=sf->create_container
		([&]
		 (const auto &container)
		 {
			 phone=phone_tab(container->get_layoutmanager());
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	LIBCXX_NAMESPACE::w::input_fieldptr firstname;

	sf=sl->insert(0);

	auto element0=sf->halign(LIBCXX_NAMESPACE::w::halign::left)
		.valign(LIBCXX_NAMESPACE::w::valign::top)
		.create_container
		([&]
		 (const auto &container)
		 {
			 firstname=name_tab(container->get_layoutmanager());
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	sl->open(0);

	LIBCXX_NAMESPACE::w::page_lock lock{sl};

	size_t n=sl->pages();
	std::cout << n << " elements:" << std::endl;

	for (size_t i=0; i<n; ++i)
		std::cout << sl->get(i)->objname() << std::endl;

	std::cout << sl->lookup(element0).value() << " "
		  << sl->lookup(element1).value() << " "
		  << sl->lookup(element2).value()
		  << (sl->lookup(address1) ? " Huh?":" Ok")
		  << std::endl;
	return std::tuple{firstname, address1, phone};
}

static void create_mainwindow(const LIBCXX_NAMESPACE::w::main_window &mw)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager glm=mw->get_layoutmanager();

	auto gf=glm->append_row();

	LIBCXX_NAMESPACE::w::input_fieldptr
		firstname, address1, phone;

	auto sw=gf->colspan(4).halign(LIBCXX_NAMESPACE::w::halign::center).create_container
		([&]
		 (const auto &s)
		 {
			 LIBCXX_NAMESPACE::w::pagelayoutmanager sl=s->get_layoutmanager();

			 std::tie(firstname, address1, phone)=create_page(sl);
		 },
		 LIBCXX_NAMESPACE::w::new_pagelayoutmanager{});

	gf=glm->append_row();

	auto name_button=gf->create_button("Name");
	auto address_button=gf->create_button("Address");
	auto phone_button=gf->create_button("Phone");
	auto clear_button=gf->create_button("Clear");

	clear_button->get_focus_before_me({name_button, address_button,
				phone_button});

	name_button->on_activate([=]
				 (THREAD_CALLBACK,
				  const auto &trigger, const auto &busy)
				 {
					 LIBCXX_NAMESPACE::w::pagelayoutmanager sl=
						 sw->get_layoutmanager();

					 sl->open(0);
					 firstname->request_focus();
				 });

	address_button->on_activate([=]
				    (THREAD_CALLBACK,
				     const auto &trigger, const auto &busy)
				    {
					    LIBCXX_NAMESPACE::w::pagelayoutmanager sl=
						    sw->get_layoutmanager();

					    sl->open(1);
					    address1->request_focus();
				    });

	phone_button->on_activate([=]
				  (THREAD_CALLBACK,
				   const auto &trigger, const auto &busy)
				  {
					  LIBCXX_NAMESPACE::w::pagelayoutmanager sl=
						  sw->get_layoutmanager();

					  sl->open(2);
					  phone->request_focus();
				  });

	clear_button->on_activate([=]
				  (THREAD_CALLBACK,
				   const auto &trigger, const auto &busy)
				  {
					  LIBCXX_NAMESPACE::w::pagelayoutmanager sl=
						  sw->get_layoutmanager();

					  sl->close();
				  });
}

void testpage(const testpage_options &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto mw=LIBCXX_NAMESPACE::w::main_window::create([]
							 (const auto &mw)
							 {
								 create_mainwindow(mw);
							 });

	mw->set_window_title("Page!");
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

	if (!editor_impl_name || !editor_impl_address)
	{
		throw EXCEPTION("EDITOR_CONSTRUCTOR_DEBUG didn't execute");
	}

	if (options.testfocus->isSet())
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait_for(std::chrono::seconds(2), [&] { return *lock; });

		if (editor_impl_with_focus != editor_impl_name)
			throw EXCEPTION("Initial focus failed");

		{
			LIBCXX_NAMESPACE::w::gridlayoutmanager
				glm=mw->get_layoutmanager();

			LIBCXX_NAMESPACE::w::container swc=glm->get(0, 0);


			LIBCXX_NAMESPACE::w::pagelayoutmanager sw=
				swc->get_layoutmanager();

			LIBCXX_NAMESPACE::w::container ac=sw->get(1);

			LIBCXX_NAMESPACE::w::gridlayoutmanager ag=
				ac->get_layoutmanager();

			LIBCXX_NAMESPACE::w::input_field address1=
				ag->get(0, 1);

			sw->open(1);
			address1->request_focus();
		}

		lock.wait_for(std::chrono::seconds(2), [&] { return *lock; });
		if (editor_impl_with_focus != editor_impl_address)
			throw EXCEPTION("Focus switch failed");
	}
	else
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait([&] { return *lock; });
	}

	for (const auto &loaded:filenames_loaded)
	{
		std::cout << loaded.first << " was loaded." << std::endl;
		if (loaded.second != 1)
			throw EXCEPTION(loaded.first << " was loaded " <<
					loaded.second << " times");
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testpage_options options;

		options.parse(argc, argv);

		testpage(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
