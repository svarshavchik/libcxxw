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
#include <x/w/listlayoutmanager.H>
#include <x/w/focusable_container.H>
#include <x/w/gridfactory.H>
#include <x/w/button.H>
#include <x/w/label.H>
#include <string>
#include <iostream>

#include "close_flag.H"
#include "listlayoutmanager.H"

std::string x::appid() noexcept
{
	return "listlayoutmanager.examples.w.libcxx.com";
}

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
	auto layout=main_window->gridlayout();

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
	// The default value is:
	//
	// new_list.selection_type=x::w::single_selection_type;
	//
	// Other possible values also include:
	//
	// x::w::single_optional_selection_type
	//
	// x::w::no_selection_type


	if (opts.multiple->value)
		new_list.selection_type=x::w::multiple_selection_type;

	// The list's height can be specified as a number of rows, both
	// the minimum and maximum (which could be the same). A scrollbar
	// gets added if the actual list exceeds the number of rows. The
	// number of rows gets multiplied by the default font's height,
	// so a list containing items that use different fonts may not
	// end up showing an exact number of lines specified, because of
	// that.
	if (opts.rows->is_set())
	{
		size_t min=opts.rows->value, max=min;

		if (opts.maxrows->is_set())
		{
			max=opts.maxrows->value;
		}

		new_list.height(min, max);
	}
	else if (opts.maxrows->is_set())
	{
		auto v=opts.maxrows->value;
		new_list.height(v); //
	}

	// Alternatively, the list's height gets set in millimeters, via
	// x::w::dim_axis_arg.
	else if (opts.height->is_set())
	{
		auto v=opts.height->value;
		new_list.height(x::w::dim_axis_arg{v});
	}

	// An optional callback that gets invoked whenever a list item gets
	// selected or unselected.
	//
	// NOTE: the usual rules apply: the callback cannot strongly capture
	// a reference to its display element (this includes the list layout
	// manager, which gets conveniently passed to the callback as a
	// parameter, if it wants it), or any of its parent display elements.


	new_list.selection_changed=
		[]
		(ONLY IN_THREAD,
		 const x::w::list_item_status_info_t &info)
		{
			std::cout << "Item #" << info.item_number << " was ";

			std::cout << (info.selected ? "selected":"unselected");

			std::cout << std::endl;
		};

	// An optional callback that gets invoked whenever a list item is
	// highlighted, not necessary selected. This can be used to provide
	// some feedback elsewhere.
	new_list.current_list_item_changed=
		[]
		(ONLY IN_THREAD,
		 const x::w::list_item_status_info_t &info)
		{
			std::cout << "Item #" << info.item_number << " was ";

			std::cout << (info.selected ? "highlighted"
				      : "unhighlighted");

			std::cout << std::endl;
		};

	x::w::focusable_container list_container=
		factory->create_focusable_container
		([&]
		 (const auto &list_container)
		 {
			 // Initialize the contents of the list container.

			 x::w::listlayoutmanager l=
				 list_container->listlayout();

			 // append_items()'s parameter is a
			 // std::vector<x::w::list_item_param>.
			 //
			 // list_item_param is (derived from) a std::variant,
			 // and can be constructed from a literal character
			 // string.

			 l->append_items({next_lorem_ipsum()});

			 // list_item_param can be constructed with an
			 // explicit text_param too, in order to use
			 // custom colors or fonts, for the given item.
			 // The resulting list may not show the number of
			 // rows requested in the new_listlayoutmanager.
			 // The resulting list height is sized based on the
			 // default font's height.

			 l->append_items({
					 x::w::text_param{next_lorem_ipsum()}
				 });
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
		.create_button("Insert New Row");

	insert_row->show();

	factory=layout->append_row();

	auto append_row=factory->halign(x::w::halign::fill)
		.create_button("Append New Row");

	append_row->show();

	factory=layout->append_row();
	auto remove_row=factory->halign(x::w::halign::fill)
		.create_button("Remove Row");

	remove_row->show();

	factory=layout->append_row();
	auto replace_row=
		factory->halign(x::w::halign::fill)
		.create_button("Replace Row");

	replace_row->show();

	factory=layout->append_row();

	auto reset=
		factory->halign(x::w::halign::fill)
		.create_button("Reset");

	reset->show();

	factory=layout->append_row();

	auto show_me=
		factory->halign(x::w::halign::fill)
		.create_button("Show Selected Items");

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
	//
	// Each list method that modifies the list, such as insert_items() and
	// append_items(), is overloaded. When invoked from a callback that
	// receives the IN_THREAD handle, the overloaded methods that take
	// an IN_THREAD parameter update the list item.
	//
	// Each method also has an overloaded version without an IN_THREAD
	// parameter, allowing the application's main execution thread to modify
	// the list too. They end up sending a message to the library's
	// connection thread to effect the change. The library's internal
	// connection thread is responsible for updating the visual appearance
	// of the display element.
	//
	// This means that, for example, invoking the non-IN_THREAD overload
	// of append_items() and then calling size() immediately will probably
	// return the same size of the list. size() won't reflect the new
	// list items until the connection thread processes the message.
	//
	// In general, the initial contents of the list get inserted or appended
	// when the new list container gets created, and all further changes
	// or updates to the list get carried out IN_THREAD. The main execution
	// thread can see some general information about the list, such as
	// it's size(), and acquire a list_lock (see below), to block
	// other execution threads from accessing the list.

	insert_row->on_activate
		([list_container, counter=0]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &busy_mcguffin)
		 mutable
		 {
			 auto l=list_container->listlayout();

			 // insert_items() inserts new items before an
			 // existing list item.
			 //
			 // An example of attaching a selection_changed
			 // callback to an individual list item by specifying
			 // it in the x::w::list_item_param.
			 //
			 // This insert_row() on_activated callback captured
			 // counter by value. Use this to count the number of
			 // new list items that get created here.
			 //
			 l->insert_items
				 (IN_THREAD,
				  0, {
					 [counter]
					 (ONLY IN_THREAD,
					  const x::w::list_item_status_info_t
					  &info)
					 {
						 std::cout << "Item factory: "
							 "item #"
							   << counter
							   << (info.selected ?
							       " is":
							       " is not")
							   << " selected at"
							   << " position "
							   << info.item_number
							   << std::endl;
					 },

					 next_lorem_ipsum()
				 });
		 });

	append_row->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 // insert_items() and append_items() take
			 // a std::vector of list_item_param-s as parameters.
			 //
			 // Each list_item_param is constructible with either
			 // an explicit x::w::text_param, or with a
			 // std::string (UTF-8) or std::u32string (unicode).
			 //
			 // A plain const char pointer will work as well.

			 l->append_items(IN_THREAD, {next_lorem_ipsum()});
		 });

	remove_row->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 // If the list is non-empty, remove the first list
			 // item.

			 if (l->size() == 0)
				 return;
			 l->remove_item(IN_THREAD, 0);
		 });

	replace_row->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 if (l->size() == 0)
				 return;

			 // replace_items() works like insert_items(), except
			 // that the existing item gets removed.

			 l->replace_items(IN_THREAD, 0, {next_lorem_ipsum()});
		 });

	reset->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &busy_mcguffin)
		 {
			 auto l=list_container->listlayout();

			 lorem_ipsum_idx=(size_t)-1;

			 // replace_all_items() is equivalent to removing
			 // all items from the list, then append_items().
			 //
			 // This effectively sets the new list of items.
			 //
			 l->replace_all_items(IN_THREAD,
					      { next_lorem_ipsum(),
						next_lorem_ipsum()});
		 });

	show_me->on_activate
		([list_container]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &busy_mcguffin)
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
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	close_flag->wait();
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
