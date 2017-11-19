/*
** Copyright 2017 Double Precision, Inc.
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
#include <x/w/switchlayoutmanager.H>
#include <x/w/switchfactory.H>
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
** Creator lambda for the switch layout manager, factored out of
** create_mainwindow() for readability.
**
** Returns a tuple of three x::w::input_field-s, the first x::w::input_field
** on each of the three containers that get added to the switch layout
** manager.
*/
static auto create_switch(const x::w::switchlayoutmanager &sl)
{
	/*
	** append() returns a factory that appends new elements to the
	** switch container.
	*/
	x::w::switchfactory sf=sl->append();

	x::w::input_fieldptr address1;

	/*
	** The switch layout manager sizes its container to be big enough
	** to accomodate the widest and the tallest element in the
	** switched container, and by default all other elements get
	** centered in the switched container.
	**
	** halign() and valign() optional methods specify a different
	** alignment for the next element added by the factory.
	**
	** We add three elements to the switched container. Each one of the
	** elements is itself a container of display elements. Since each
	** container is viewed as one individual display element, by the
	** switch layout manager, this ends up creating three groups of
	** elements and the switch layout manager switches each group out
	** as a whole.
	**
	** Like all display elements, they must be show()n to be visible,
	** so we digilently use show_all() to show the new containers, and
	** all of the individual display elements in each created container.
	**
	** The fact that the switch layout manager makes each element
	** individually visible, or not, is orthogonal. Each display element
	** must still be explicitly show()n. We can leave out these
	** show_all()s here, and simply show_all() the entire main window,
	** after everything gets constructed. That's also acceptable.
	*/

	sf->halign(x::w::halign::left).valign(x::w::valign::top)
		.create_container
		([&]
		 (const auto &container)
		 {
			 address1=address_tab(container->get_layoutmanager());
		 },
		 x::w::new_gridlayoutmanager{})
		->show_all();

	/*
	** Use the same append() factory to add another one to the switch
	** layout manager.
	*/

	x::w::input_fieldptr phone;

	sf->create_container
		([&]
		 (const auto &container)
		 {
			 phone=phone_tab(container->get_layoutmanager());
		 },
		 x::w::new_gridlayoutmanager{})
		->show_all();

	x::w::input_fieldptr firstname;

	/*
	** insert() returns a factory that inserts new elements before an
	** existing element in the switch layout manager. So the following
	** actually ends up inserting element #0 in the switch layout manager,
	** and afterit gets inserted, the above two containers become
	** elements #1 and elements #2.
	*/
	sf=sl->insert(0);

	sf->halign(x::w::halign::left).valign(x::w::valign::top)
		.create_container
		([&]
		 (const auto &container)
		 {
			 firstname=name_tab(container->get_layoutmanager());
		 },
		 x::w::new_gridlayoutmanager{})
		->show_all();

	/*
	** The initial state of the switched container: make element #0
	** visible.
	*/
	sl->switch_to(0);

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
	** new_switchlayoutmanager.
	**
	** The second row contains four buttons, so this container spans
	** four columns. In the event this container is smaller than the
	** four buttons (unlikely), it gets centered.
	*/

	auto sw=gf->colspan(4).halign(x::w::halign::center).create_container
		([&]
		 (const auto &s)
		 {
			 x::w::switchlayoutmanager sl=s->get_layoutmanager();

			 std::tie(firstname, address1, phone)=create_switch(sl);
		 },
		 x::w::new_switchlayoutmanager{});

	/*
	** Create four buttons on the last row.
	*/

	gf=glm->append_row();

	auto name_button=gf->create_normal_button_with_label
		({"underline"_decoration,
				"N",
				"no"_decoration,
				"ame"},
		 {"Alt", 'N'});
	auto address_button=gf->create_normal_button_with_label
		({"underline"_decoration,
				"A",
				"no"_decoration,
				"ddress"},
		 {"Alt", 'A'});
	auto phone_button=gf->create_normal_button_with_label
		({"underline"_decoration,
				"P",
				"no"_decoration,
				"hone"},
		 {"Alt", 'P'});
	auto clear_button=gf->create_normal_button_with_label
		({"underline"_decoration,
				"C",
				"no"_decoration,
				"lear"},
		 {"Alt", 'C'});

	/*
	** The first three buttons switch_to the corresponding container,
	** then request_focus() to the first input field in the switched-to
	** container.
	**
	** Note that the callbacks capture by value the switch container,
	** and the input field.
	**
	** The callbacks cannot capture the switchlayoutmanager itself, they
	** must capture the container, by value, and use get_layoutmanager()
	** when needed. Although layout manager typically do not acquire
	** locks on internal objects, for the duration of their existence,
	** instantiated layout managers enable buffering of internal
	** processing. For efficiency, the library's internal execution thread
	** delays processing of changes to individual display elements'
	** position, visibility, and other attributes, while any layout
	** manager is instantiated.
	**
	** Capturing the layout manager by value will effectively block
	** this processing and make the display appear to freeze. For that
	** reason, always capture the container, and acquire its layout
	** manager when needed.
	**
	** Neither the switched container, nor any of the display elements
	** in the switched container, such as the captured input_fields,
	** are parent or child elements of these buttons, so capturing them
	** by value does not create a circular reference. Only display
	** elements that are immediate parent OR child elements cannot be
	** captured by value without creating a circular reference.
	*/

	name_button->on_activate([=]
				 (const auto &trigger, const auto &busy)
				 {
					 x::w::switchlayoutmanager sl=
						 sw->get_layoutmanager();

					 sl->switch_to(0);
					 firstname->request_focus();
				 });

	address_button->on_activate([=]
				    (const auto &trigger, const auto &busy)
				    {
					    x::w::switchlayoutmanager sl=
						    sw->get_layoutmanager();

					    sl->switch_to(1);
					    address1->request_focus();
				    });

	phone_button->on_activate([=]
				  (const auto &trigger, const auto &busy)
				  {
					  x::w::switchlayoutmanager sl=
						  sw->get_layoutmanager();

					  sl->switch_to(2);
					  phone->request_focus();
				  });

	/*
	** For the last four buttons, we just switch_off() everything.
	*/

	clear_button->on_activate([=]
				  (const auto &trigger, const auto &busy)
				  {
					  x::w::switchlayoutmanager sl=
						  sw->get_layoutmanager();

					  sl->switch_off();
				  });
	name_button->show();
	address_button->show();
	phone_button->show();
	clear_button->show();
	sw->show();
}

void testswitch()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto mw=x::w::main_window::create([]
					  (const auto &mw)
					  {
						  create_mainwindow(mw);
					  });

	mw->set_window_title("Switch!");

	guard(mw->connection_mcguffin());

	mw->on_disconnect([]
			  {
				  exit(1);
			  });

	mw->on_delete([close_flag]
		      (const auto &ignore)
		      {
			      close_flag->close();
		      });

	mw->show();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testswitch();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
