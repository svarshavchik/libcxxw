/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/pagelayoutmanager.H>
#include <x/w/pagefactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/button.H>
#include <x/w/input_field.H>
#include <x/w/input_field_config.H>
#include <x/w/canvas.H>

#include <iostream>
#include "close_flag.H"

std::string x::appid() noexcept
{
	return "pagelayoutmanager.examples.w.libcxx.com";
}

/*
** Create:
**
**    First name: _____________________________
**     Last name: _____________________________
**
** Returns the "first name" x::w::input_field
*/

static x::w::input_field name_tab(const x::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->halign(x::w::halign::right).create_label("First name:");

	x::w::input_field_config config;

	config.autoselect=true;

	auto firstname=f->create_input_field("", config);

	f=glm->append_row();

	f->halign(x::w::halign::right).create_label("Last name:");
	f->create_input_field("", config);

	return firstname;
}

/*
** Create:
**
**    Address: _____________________________
**             _____________________________
**       City: ______ State: ___ Zip: ________

** Returns the first "Address" x::w::input_field
*/

static x::w::input_field address_tab(const x::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->halign(x::w::halign::right).create_label("Address:");

	x::w::input_field_config config;

	config.autoselect=true;

	// There are six elements on the last row of the grid, so we
	// need to make the address input fields span 5 columns (the
	// label takes the first column)

	auto address1=f->colspan(5).create_input_field("", config);

	f=glm->append_row();

	// No label for the 2nd address field, put an empty canvas in there.
	f->create_canvas();

	f->colspan(5).create_input_field("", config); // address2

	f=glm->append_row();

	f->halign(x::w::halign::right).create_label("City:");

	config.columns=20;

	f->create_input_field("", config);

	// No alignment is typically needed for State: and Zip: labels,
	// but just in case a theme makes the two big input fields wider
	// than the four elements, and the grid stretches the elements in
	// last row to align everything up, this will make things look
	// better.
	f->halign(x::w::halign::right).create_label("State:");

	config.columns=3;

	f->create_input_field("", config);

	f->halign(x::w::halign::right).create_label("Zip:");

	config.columns=11;

	f->create_input_field("", config);

	return address1;
}

/*
**    Phone: _______________________
**
** Returns the phone x::w::input_field
*/

static x::w::input_field phone_tab(const x::w::gridlayoutmanager &glm)
{
	auto f=glm->append_row();

	f->create_label("Phone:");

	x::w::input_field_config config;

	config.autoselect=true;

	return f->create_input_field("", config);
}

/*
** Creator lambda for the page layout manager, factored out of
** create_mainwindow() for readability.
**
** Returns a tuple of three x::w::input_field-s, the first x::w::input_field
** on each of the three containers that get added to the page layout
** manager.
*/
static auto create_page(const x::w::pagelayoutmanager &pl)
{
	/*
	** append() returns a factory that appends new elements to the
	** page container.
	*/
	x::w::pagefactory new_page=pl->append();

	x::w::input_fieldptr address1;

	/*
	** The page layout manager sizes its container to be big enough
	** to accomodate the widest and the tallest element in the
	** paged container, and by default all other elements get
	** centered in the paged container.
	**
	** halign() and valign() optional methods specify a different
	** alignment for the next element added by the factory.
	**
	** We add three elements to the paged container. Each one of the
	** elements is itself a container of display elements. Since each
	** container is viewed as one individual display element, by the
	** page layout manager, this ends up creating three groups of
	** elements, as three pages.
	**
	** Like all display elements, they must be show()n to be visible,
	** so we digilently use show_all() to show the new containers, and
	** all of the individual display elements in each created container.
	**
	** The fact that the page layout manager makes each element
	** individually visible, or not, is orthogonal. Each display element
	** must still be explicitly show()n. We can leave out these
	** show_all()s here, and simply show_all() the entire main window,
	** after everything gets constructed. That's also acceptable.
	*/

	new_page->halign(x::w::halign::left).valign(x::w::valign::top)
		.create_container
		([&]
		 (const auto &container)
		 {
			 address1=address_tab(container->gridlayout());
		 },
		 x::w::new_gridlayoutmanager{})
		->show_all();

	/*
	** Use the same append() factory to add another one to the page
	** layout manager.
	*/

	x::w::input_fieldptr phone;

	new_page->create_container
		([&]
		 (const auto &container)
		 {
			 phone=phone_tab(container->gridlayout());
		 },
		 x::w::new_gridlayoutmanager{})
		->show_all();

	x::w::input_fieldptr firstname;

	/*
	** insert() returns a factory that inserts new elements before an
	** existing element in the page layout manager. So the following
	** actually ends up inserting element #0 in the page layout manager,
	** and after it gets inserted, the above two containers become
	** elements #1 and elements #2.
	*/
	new_page=pl->insert(0);

	new_page->halign(x::w::halign::left).valign(x::w::valign::top)
		.create_container
		([&]
		 (const auto &container)
		 {
			 firstname=name_tab(container->get_layoutmanager());
		 },
		 x::w::new_gridlayoutmanager{})
		->show_all();

	/*
	** The initial state of the paged container: make element #0
	** visible.
	*/
	pl->open(0);

	return std::tuple{firstname, address1, phone};
}

/*
** The main_window creator lambda, factored out for readability.
*/
static void create_mainwindow(const x::w::main_window &mw)
{
	x::w::gridlayoutmanager glm=mw->get_layoutmanager();

	auto gf=glm->append_row();

	x::w::input_fieldptr
		firstname, address1, phone;

	/*
	** On the first row we create our container with a
	** new_pagelayoutmanager.
	**
	** The second row contains four buttons, so this container spans
	** four columns. In the event this container is smaller than the
	** four buttons (unlikely), it gets centered.
	*/

	auto pg=gf->colspan(4).halign(x::w::halign::center).create_container
		([&]
		 (const auto &s)
		 {
			 x::w::pagelayoutmanager pl=s->get_layoutmanager();

			 std::tie(firstname, address1, phone)=create_page(pl);
		 },
		 x::w::new_pagelayoutmanager{});

	/*
	** Create four buttons on the last row.
	*/

	gf=glm->append_row();

	auto name_button=gf->create_button
		({"underline"_decoration,
				"N",
				"no"_decoration,
				"ame"},
			x::w::shortcut{"Alt", 'N'});
	auto address_button=gf->create_button
		({"underline"_decoration,
				"A",
				"no"_decoration,
				"ddress"},
			x::w::shortcut{"Alt", 'A'});
	auto phone_button=gf->create_button
		({"underline"_decoration,
				"P",
				"no"_decoration,
				"hone"},
			x::w::shortcut{"Alt", 'P'});
	auto clear_button=gf->create_button
		({"underline"_decoration,
				"C",
				"no"_decoration,
				"lear"},
			x::w::shortcut{"Alt", 'C'});

	/*
	** The first three buttons open the corresponding page,
	** then request_focus() to the first input field in the page.
	**
	** Note that the callbacks capture by value the paged container
	** and the input field.
	**
	** The callbacks cannot capture the pagelayoutmanager itself, they
	** capture the container by value, and use
	** get_layoutmanager()/pagelayout() when needed.
	**
	** Most layout managers acquire a lock on their container,
	** that may block the internal library execution thread from
	** updating the display, and all instantiated layout managers
	** enable buffering of internal processing. For efficiency, the
	** library's internal execution thread skips updating the individual
	** display elemnts' position, visibility, and other attributes, while
	** any layout manager is instantiated. All accumulated changes get
	** processed in one batch after all layout manager objects go out of
	** scope and get destroyed.
	**
	** Capturing the layout manager by value effectively keeps the
	** layout manager in existence while the callback closure is
	** install. This blocks processing and make the display appear to
	** freeze. For that reason, always capture the container (when a
	** circular reference won't be created, and use a weak capture
	** otherwise) then get_layoutmanager()/pagelayout() only when needed.
	**
	** Neither the paged container -- nor any of the display elements
	** in the paged container, such as the captured input_fields --
	** are a parent or child element of these buttons, so capturing them
	** by value does not create a circular reference. Only display
	** elements that are immediate parent OR child elements cannot be
	** captured by value without creating an internal circular reference.
	*/

	name_button->on_activate([=]
				 (ONLY IN_THREAD,
				  const auto &trigger, const auto &busy)
				 {
					 x::w::pagelayoutmanager pl=
						 pg->pagelayout();

					 pl->open(0);
					 firstname->request_focus();
				 });

	address_button->on_activate([=]
				    (ONLY IN_THREAD,
				     const auto &trigger, const auto &busy)
				    {
					    auto pl=pg->pagelayout();

					    pl->open(1);
					    address1->request_focus();
				    });

	phone_button->on_activate([=]
				  (ONLY IN_THREAD,
				   const auto &trigger, const auto &busy)
				  {
					  auto pl=pg->pagelayout();

					  pl->open(2);
					  phone->request_focus();
				  });

	/*
	** For the fourth button, we just close() everything.
	*/

	clear_button->on_activate([=]
				  (ONLY IN_THREAD,
				   const auto &trigger, const auto &busy)
				  {
					  auto pl=pg->pagelayout();

					  pl->close();
				  });
	name_button->show();
	address_button->show();
	phone_button->show();
	clear_button->show();
	pg->show();
}

void testpage()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto mw=x::w::main_window::create([]
					  (const auto &mw)
					  {
						  create_mainwindow(mw);
					  });

	mw->set_window_title("Page!");
	mw->set_window_class("main",
			     "pagelayoutmanager.examples.w.libcxx.com");

	guard(mw->connection_mcguffin());

	mw->on_disconnect([]
			  {
				  exit(1);
			  });

	mw->on_delete([close_flag]
		      (ONLY IN_THREAD,
		       const auto &ignore)
		      {
			      close_flag->close();
		      });

	mw->show();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testpage();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
