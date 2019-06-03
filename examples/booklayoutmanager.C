/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/booklayoutmanager.H>
#include <x/w/bookpagefactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/button.H>
#include <x/w/input_field.H>
#include <x/w/input_field_config.H>
#include <x/w/canvas.H>

#include <iostream>
#include "close_flag.H"

/*
** Create:
**
**    First name: _____________________________
**     Last name: _____________________________
**
** Returns the "first name" x::w::input_field
*/

static void name_tab(const x::w::container &container)
{
	x::w::gridlayoutmanager glm=container->get_layoutmanager();

	auto f=glm->append_row();

	f->halign(x::w::halign::right).create_label("First name:");

	x::w::input_field_config config;

	config.autoselect=true;

	// The first focusable field on this page. Store it in the container's
	// appdata, for convenient access later.
	//
	// The new input field is a child element of the container. Only
	// callbacks that get attached to display elements are restricted
	// from capturing references to any parent or child elements, because
	// this creates a circular reference.
	//
	// Container own references to the elements in the container. Here,
	// we store a reference to the input field, in its container. This is
	// fine. When the container goes out of scope and gets destroyed, this
	// reference goes away as well.
	//
	// Every display element has an appdata x::ptr, that the library does
	// not use otherwise, and is available for applications to attach
	// x::ptrs or x::refs to reference-counted objects that are derived
	// from x::obj, and constructed with create().

	container->appdata=f->create_input_field("", config);

	f=glm->append_row();

	f->halign(x::w::halign::right).create_label("Last name:");
	f->create_input_field("", config);
}

/*
** Create:
**
**    Address: _____________________________
**             _____________________________
**       City: ______ State: ___ Zip: ________

** Returns the first "Address" x::w::input_field
*/

static void address_tab(const x::w::container &container)
{
	x::w::gridlayoutmanager glm=container->get_layoutmanager();

	auto f=glm->append_row();

	f->halign(x::w::halign::right).create_label("Address:");

	x::w::input_field_config config;

	config.autoselect=true;

	// There are six elements on the last row of the grid, so we
	// need to make the address input fields span 5 columns (the
	// label takes the first column)

	container->appdata=f->colspan(5).create_input_field("", config);

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
}

/*
**    Phone: _______________________
**
** Returns the phone x::w::input_field
*/

static void phone_tab(const x::w::container &container)
{
	x::w::gridlayoutmanager glm=container->get_layoutmanager();

	auto f=glm->append_row();

	f->create_label("Phone:");

	x::w::input_field_config config;

	config.autoselect=true;

	container->appdata=f->create_input_field("", config);
}

/*
** Creator lambda for the book layout manager, factored out of
** create_mainwindow() for readability.
**
** Returns a tuple of three x::w::input_field-s, the first x::w::input_field
** on each of the three containers that get added to the book layout
** manager.
*/
static void create_book(const x::w::booklayoutmanager &pl)
{
	/*
	** append() returns a factory that appends new pages to the book.
	*/
	x::w::bookpagefactory new_page=pl->append();

	/*
	** The book layout manager is an extended version of the page
	** layout manager, and halign() and valign() serves the same
	** purpose as they do with the page layout manager.
	**
	** The major difference between page layout manager's pagefactory
	** and book layout manager's bookpagefactory is that
	** new pages get created by add().
	*/

	new_page->halign(x::w::halign::left).valign(x::w::valign::top)
		.add(// The tab's label, an x::w::text_param
		     {"underline"_decoration,
				     "A",
				     "no"_decoration,
				     "ddress"},

		     // The second parameter constructors the page element.

		     [&]
		     (const x::w::factory &page_factory)
		     {
			     auto container=page_factory->create_container
				     ([]
				      (const auto &container)
				      {
					      address_tab(container);
				      },
				      x::w::new_gridlayoutmanager{});

			     container->show_all();
		     },

		     // The third parameter to add() is its options. We
		     // specify x::w::shortcut option for this page.

		     {
		      x::w::shortcut{"Alt", 'A'}
		     });


	/*
	** Like the pagefactory, the same bookpagefactory can be used to
	** add multiple pages.
	*/

	new_page->add({
			"underline"_decoration,
				"P",
				"no"_decoration,
				"hone"},
		[&]
		(const x::w::factory &page_factory)
		{
			page_factory->create_container
				([&]
				 (const auto &container)
				 {
					 phone_tab(container);
				 },
				 x::w::new_gridlayoutmanager{})
				->show_all();
		},
		{
		 x::w::shortcut{"Alt", 'P'}
		});

	/*
	** And, similar to the pagefactory, the insert() method adds new
	** pages before an existing page.
	*/
	new_page=pl->insert(0);

	/*
	** The page's tab doesn't have to be a plain text label, it can
	** be an arbitrary display element. This is done by passing another
	** callable object that takes a factory parameter, instead of the
	** first text_param argument to add().
	*/
	new_page->halign(x::w::halign::left).valign(x::w::valign::top)
		.add([&]
		     (const x::w::factory &tab_factory)
		     {
			     /*
			     ** Providing the text label to add() directly
			     ** is equivalent to calling create_label() on the
			     ** first factory, and show()ing the label.
			     */
			     tab_factory->create_label
				     ({
					     "underline"_decoration,
					     "N",
					     "no"_decoration,
					     "ame"})->show();

		     },
		     [&]
		     (const x::w::factory &page_factory)
		     {

			     /*
			     ** And the second parameter is the same page
			     ** factory, that creates the page element normally.
			     */
			     page_factory->create_container
				     ([&]
				      (const auto &container)
				      {
					      name_tab(container);
				      },
				      x::w::new_gridlayoutmanager{})
				     ->show_all();
		     },
		     {
		      x::w::shortcut{"Alt", 'N'}
		     });

	/*
	** The initial state of the paged container: make element #0
	** visible.
	*/
	pl->open(0);

	/*
	** The on_opened callback gets invoked whenever a new page gets
	** opened. We pointedly install it after the call to open(0), and
	** not before. At this point the entire window is not visible yet,
	** so attempting to move the input focus won't accomplish anything
	** useful.
	*/

	pl->on_opened
		([]
		 (ONLY IN_THREAD,
		  const x::w::book_status_info_t &info)
		 {
			 // Page number that was opened.

			 size_t n=info.opened;

			 auto page=info.lock.layout_manager->get_page(n);

			 // In name_tab(), address_tab(), and phone_tab(),
			 // we stashed away the first input field on each
			 // page here.

			 x::w::input_field f=page->appdata;

			 // So, after each page gets opened, we automatically
			 // move keyboard input focus here.

			 f->request_focus();
		 });
}

/*
** The main_window creator lambda, factored out for readability.
*/
static void create_mainwindow(const x::w::main_window &mw)
{
	x::w::gridlayoutmanager glm=mw->get_layoutmanager();

	auto gf=glm->append_row();

	/*
	** On the first row we create a focusable container using a
	** new_booklayoutmanager.
	**
	** Book layout manager's containers must be created with
	** create_focusable_container(), because the book layout manager
	** takes care of some focusable elements.
	*/

	auto book=gf->create_focusable_container
		([&]
		 (const auto &s)
		 {
			 x::w::booklayoutmanager pl=s->get_layoutmanager();

			 create_book(pl);
		 },
		 x::w::new_booklayoutmanager{});

	book->show();
}

void testbook()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto mw=x::w::main_window::create([]
					  (const auto &mw)
					  {
						  create_mainwindow(mw);
					  });

	mw->set_window_title("Book!");
	mw->set_window_class("main",
			     "book@examples.w.libcxx.com");

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
		testbook();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
