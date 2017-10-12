/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/obj.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/focusable_container.H>
#include <x/w/gridfactory.H>
#include <x/w/button.H>
#include <x/w/label.H>
#include <x/w/screen.H>
#include <x/w/connection.H>
#include <string>
#include <iostream>

#include "close_flag.H"
#include "listlayoutmanager.H"

static const char * const lorem_ipsum[]={
	"Lorem ipsum",
	"dolor sit amet",
	"consectetur",
	"adipisicing",
	"elit",
	"sed do eiusmod",
	"tempor incididunt",
	"ut labore",
	"et dolore magna"
};

static size_t lorem_ipsum_idx=(size_t)-1;

// next_lorem_ipsum() always gets called from the library's internal
// execution thread (except for the initial list contents, but this doesn't
// count), so we don't need to worry about thread safety for the index
// counter. But even if there was a thread concurrency issue here this
// isn't critical for this example.

static const char *next_lorem_ipsum()
{
	size_t i=(lorem_ipsum_idx+1) %
		(sizeof(lorem_ipsum)/sizeof(lorem_ipsum[0]));

	lorem_ipsum_idx=i;

	return lorem_ipsum[i];
}


static inline void create_main_window(const x::w::main_window &main_window,
				      const options &opts)
{
	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();

	x::w::gridfactory factory=layout->append_row();

	//
	// The first column in the window contains the list container.
	//
	// The second column in the window contains five buttons. The list
	// container is going to span the six rows in the first column.

	factory->rowspan(6);

	x::w::new_listlayoutmanager
		new_list{opts.bullets->value
			? x::w::bulleted_list : x::w::highlighted_list};

	// x::w::single_selection_type is the default:
	//
	// new_list.selection_type=x::w::single_selection_type;

	if (opts.multiple->value)
		new_list.selection_type=x::w::multiple_selection_type;

	if (opts.rows->isSet())
		new_list.rows=opts.rows->value;

	// The selection_changed() callback gets executed whenever a
	// list item changes state.
	//
	// NOTE: the usual rules apply: the callback cannot strongly capture
	// a reference to its display element (this includes the list layout
	// manager, which gets conveniently passed to the callback as a
	// parameter, if it wants it), or any of its parent display elements.

	new_list.selection_changed=
		[]
		(const x::w::listlayoutmanager &list,
		 size_t item_number,
		 bool is_selected,
		 const x::w::busy &busy_mcguffin)
		{
			std::cout << "selection_changed: item #" << item_number
			<< (is_selected ? " is selected."
			    : " is not selected.") << std::endl;
		};

	x::w::focusable_container list_container=
		factory->create_focusable_container
		([&]
		 (const auto &list_container)
		 {
			 // Initialize the contents of the list container.

			 x::w::listlayoutmanager l=list_container->get_layoutmanager();

			 // Initialize with two items in the list.

			 l->append_item(next_lorem_ipsum());

			 // append_item() is overloaded, and can take a vector
			 // to insert multiple items at once:

			 std::vector<x::w::text_param> items={next_lorem_ipsum()};
			 l->append_item(items);
		 },
		 new_list);
	list_container->show();

	// Create the four buttons. The first button is on the same row as
	// the list container. The next three buttons are on a row of their
	// own, and because the list container spans four rows columns
	// all the four buttons end up in the second column.
	//
	// halign::fill all four buttons, so that they have the same,
	// uniform width, even though they have different labels and each
	// button would, by default be just wide enough for its label.

	auto insert_row=
		factory->halign(x::w::halign::fill)
		.create_special_button_with_label("Insert New Row");

	insert_row->show();

	factory=layout->append_row();

	auto append_row=factory->halign(x::w::halign::fill)
		.create_special_button_with_label("Append New Row");

	append_row->show();

	factory=layout->append_row();
	auto remove_row=factory->halign(x::w::halign::fill)
		.create_special_button_with_label("Remove Row");

	remove_row->show();

	factory=layout->append_row();
	auto replace_row=
		factory->halign(x::w::halign::fill)
		.create_special_button_with_label("Replace Row");

	replace_row->show();

	factory=layout->append_row();

	auto reset=
		factory->halign(x::w::halign::fill)
		.create_special_button_with_label("Reset");

	reset->show();

	factory=layout->append_row();

	auto show_me=
		factory->halign(x::w::halign::fill)
		.create_special_button_with_label("Show Selected Items");

	show_me->show();

	// Attach callbacks to the buttons. As is the case with all callbacks
	// they cannot capture references to their parent display elements.
	// However the list container is a sibling to the buttons, so this is
	// not a problem.
	//
	// The layout manager object acquires a lock on the container. Can't
	// capture the layout manager, since it would then exist for the
	// duration of the installed callback. The correct approach is to
	// capture the container and then construct the layout manager when
	// needed.

	insert_row->on_activate
		([list_container, counter=0]
		 (const x::w::busy &busy_mcguffin)
		 mutable
		 {
			 x::w::listlayoutmanager l=
				 list_container->get_layoutmanager();

			 // An example of callbacks attached to list items.
			 //
			 // Install a callback factory into the list layout
			 // manager.
			 //
			 // The callback factory gets invoked whenever a new
			 // list item gets added, and returns a second callback.
			 // The second callback gets invoked any time the new
			 // list item gets selected or unselected.
			 //
			 // This on_activate() lambda captures a counter
			 // variable, that gets initialized to 0.
			 //
			 // Install a callback_factory into the instantiated
			 // listlayoutmanager, that captures the counter
			 // by reference, and using it to number each item
			 // created by this listlayoutmanager.
			 //
			 // Since callback_factory()s are attached to
			 // each individual listlayoutmanager, this takes
			 // effect only for list items created by this
			 // on_activate() callback.


			 l->callback_factory
				 ([&]
				  {
					  // Return a callback for a new list
					  // item being created.

					  return [n=counter++]
						  (x::w::list_lock &lock,
						   size_t i,
						   bool selected)
					  {
						  std::cout << "Item factory: "
							  "item #"
							    << n
							    << (selected ?
								" is":
								" is not")
							    << " selected at"
							    << " position "
							    << i
							    << std::endl;
					  };
				  });

			 l->insert_item(0, next_lorem_ipsum());

			 // insert_item(), like append_item() is overloaded
			 // to take a vector of items to insert at once.
		 });

	append_row->on_activate
		([list_container]
		 (const x::w::busy &busy_mcguffin)
		 {
			 x::w::listlayoutmanager l=
				 list_container->get_layoutmanager();

			 // insert_item() and append_item() can insert an
			 // arbitrary display element.
			 //
			 // append_item() and insert_item(item_numbers) returns
			 // a generic factory that can be used to insert
			 // any non-focusable display element as a list item.
			 //
			 // Inserting a text string (or a text_param) is
			 // really a wrapper for create_label().
			 //
			 // insert_item(0, next_lorem_ipsum());
			 //
			 // is exactly equivalent to:
			 //
			 // l->insert_item(0)->create_label(next_lorem_ipsum());
			 //
			 // Here's an example of using the factory overload:

			 x::w::factory f=l->append_item();

			 // Also insert_item(item_number). Then:

			 f->create_label(next_lorem_ipsum());

			 // Another f->create_label(next_lorem_ipsum()) will
			 // insert another item.
			 //
			 // The insert_item() overload works the same way.
		 });

	remove_row->on_activate
		([list_container]
		 (const x::w::busy &busy_mcguffin)
		 {
			 x::w::listlayoutmanager l=
				 list_container->get_layoutmanager();

			 if (l->size() == 0)
				 return;
			 l->remove_item(0);
		 });

	replace_row->on_activate
		([list_container]
		 (const x::w::busy &busy_mcguffin)
		 {
			 x::w::listlayoutmanager l=
				 list_container->get_layoutmanager();

			 if (l->size() == 0)
				 return;

			 // replace_item() works like insert_item(), except
			 // that the existing item gets removed. It has the
			 // same overloads as append_item() and insert_item().

			 l->replace_item(0, next_lorem_ipsum());
		 });

	reset->on_activate
		([list_container]
		 (const x::w::busy &busy_mcguffin)
		 {
			 x::w::listlayoutmanager l=
				 list_container->get_layoutmanager();

			 lorem_ipsum_idx=(size_t)-1;

			 // Two overloads of replace_all(). One that
			 // takes a vector of text_params, specifying the
			 // new items for the list:
			 //
			 l->replace_all_items({ next_lorem_ipsum(), next_lorem_ipsum()});
			 // Without any parameters replace_all() removes
			 // all existing items from the list and returns
			 // a factory for creating new list items.
		 });

	show_me->on_activate
		([list_container]
		 (const x::w::busy &busy_mcguffin)
		 {
			 x::w::listlayoutmanager l=
				 list_container->get_layoutmanager();

			 // This callback gets executed by the library's
			 // internal connection thread, so this lock is
			 // not really necessary, since only the connection
			 // thread modifies the list.
			 //
			 // Acquiring a list_lock blocks other execution
			 // threads from accessing the list. The lock freezes
			 // the list state, so that the list's contents can
			 // be examined and modified, with the list's contents
			 // remaining consistent.

			 x::w::list_lock lock{l};

			 auto s=l->size();
			 size_t n=0;

			 for (size_t i=0; i<s; ++i)
				 if (l->selected(i))
				 {
					 ++n;
					 std::cout << "Item #"
						   << i << " is selected."
						   << std::endl;

				 }

			 std::cout << "Total # of selected items: " << n
				   << std::endl;
		 });
}

void testlistlayoutmanager(const options &opts)
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window, opts);
			 });

	main_window->set_window_title("List layout manager");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		options opts;

		opts.parse(argc, argv);

		testlistlayoutmanager(opts);
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
