/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/itemlayoutmanager.H>
#include <x/w/label.H>
#include <x/w/input_field.H>
#include <x/w/input_field_lock.H>
#include <x/w/container.H>
#include <x/w/focusable_container.H>
#include <x/w/canvas.H>

#include <x/singletonptr.H>
#include <x/strtok.H>
#include <x/join.H>
#include <x/mutex.H>
#include <courier-unicode.h>
#include <unordered_set>
#include <vector>

// The application object.

class appObj : virtual public x::obj {

public:

	// The main window
	const x::w::main_window main_window;

	// The input text field where toppings get typed in, as text
	const x::w::input_field toppings_text_field;

	// The container with the list layout manager.
	const x::w::focusable_container toppings_list;

	// The list of items in the container
	//
	// This object gets accessed by both the library execution thread and
	// the main execution thread, hence it's wrapped inside a mutex.
	//
	// This vector parallels the items in the toppings_list container.
	// Those item are display elements, and this vector is the corresponding
	// topping names.
	typedef x::mpobj<std::vector<std::string>> toppings_t;

	toppings_t toppings;

	// Constructor
	appObj(const x::w::main_window &main_window,
	       const x::w::input_field &toppings_text_field,
	       const x::w::focusable_container &toppings_list)
		: main_window{main_window},
		  toppings_text_field{toppings_text_field},
		  toppings_list{toppings_list}
	{
	}

	// Item layout manager callback that gets invoked in response to
	// the click on the closing "X". We remove the item label from the
	// container, and also update our toppings list.
	void remove_toppings(const x::w::itemlayout_lock &lock,
			     size_t i)
	{
		toppings_t::lock t_lock{toppings};

		if (i >= t_lock->size())
			throw EXCEPTION("Shouldn't happen");

		// Remove the item label, in the container, and update our
		// vector.
		lock.layout_manager->remove_item(i);
		t_lock->erase(t_lock->begin()+i);
	}

	void add_toppings()
	{
		// Something was entered into the toppings text input field.
		//
		// Retrieve the string, split it by commas, then clear the
		// contents.
		std::vector<std::string> words;

		{
			x::w::input_lock lock{toppings_text_field};

			x::strtok_str(lock.get(), ",", words);

			toppings_text_field->set("");
		}

		add_toppings(words);
	}

	void add_toppings(const std::vector<std::string> &words)
	{
		std::unordered_set<std::string> dupes;

		// We must obtain the item layout manager lock first, then
		// the lock on the toppings vector. When the internal
		// execution thread invokes the remove_toppings() callback,
		// the internal execution thread acquires the itemlayout_lock
		// first, then passes it to the callback, which them obtains
		// the lock on the toppings vector.
		//
		// We must use the same locking order here, to avoid the
		// deadlock.
		x::w::itemlayout_lock i_lock{toppings_list->itemlayout()};

		toppings_t::lock t_lock{toppings};

		// Go through each word that was split out of the comma-
		// separated string.
		for (auto &w:words)
		{
			auto b=w.begin();
			auto e=w.end();

			// Trim off the leading and trailing whitespace.
			x::trim(b, e);

			// Convert each trimmed word to lowercase, and
			// capitalize the first letter.
			auto word=unicode::iconvert::convert_tocase
				({b, e},
				 unicode_default_chset(),
				 unicode_tc,
				 unicode_lc);

			if (word.empty())
				continue;

			// The toppings vector is sorted, find where this
			// topping name belongs, in the vector.
			auto iter=std::lower_bound(t_lock->begin(),
						   t_lock->end(), word);

			// Is this topping already entered?
			if (iter != t_lock->end() && *iter == word)
			{
				dupes.insert(word);
				continue;
			}

			// Insert this topic at this position both in the
			// toppings vector and at the same corresponding
			// position in the container.

			size_t pos=iter-t_lock->begin();

			t_lock->insert(iter, word);

			// The item layout manager's insert_item() adds a new
			// item to the container positioned before an existing
			// item. There's also append_item() that adds a new
			// item to the end of the container after all existing
			// items.
			//
			// insert_item() and append_item() take a lambda, or
			// a callable object as a parameter. The lambda
			// gets called with an x::w::factory parameter. The
			// lambda must use this factory to create exactly
			// one display element, that represents the item's
			// label. Typically the lambda calls create_label(),
			// but any display element is permissible. In all
			// cases the lambda is responsible for show()ing the
			// newly-created label.
			i_lock.layout_manager->insert_item
				(pos,
				 [&]
				 (const x::w::factory &f)
				 {
					 f->create_label(word)->show();
				 });
		}

		// See if there were any dupes, and show an error message

		if (dupes.empty())
			return;

		std::vector<std::string>
			sorted_dupes{dupes.begin(), dupes.end()};

		std::sort(sorted_dupes.begin(),
			  sorted_dupes.end());

		auto list=x::join(sorted_dupes.begin(),
				  sorted_dupes.end(),
				  ", ");
		main_window->alert_message(list + " already ordered!");
	}
};

typedef x::singletonptr<appObj> app;

auto create_mainwindow(const x::w::main_window &main_window)
{
	// Create the main application window and its important display
	// elements.
	auto layout=main_window->gridlayout();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	// A label and an input field on the first row in the main window.
	factory->create_label("Pizza toppings:");

	x::w::input_field_config config{30};

	auto field=factory->create_input_field("", config);

	// Install a manual on_validate() callback that gets invoked by
	// "Enter" in the input field, or when tabbing out of it.

	field->on_validate([]
			   (ONLY IN_THREAD,
			    x::w::input_lock &lock,
			    const x::w::callback_trigger_t &triggering_event)
			   {
				   app my_app;

				   if (my_app)
					   my_app->add_toppings();
				   return true;
			   });

	factory=layout->append_row();

	// The grid has two columns. Put an empty canvas place holder in
	// the first column on the 2nd row, below the "Pizza toppings:" label.
	factory->create_canvas();

	// And create a focusable container that uses the item layout manager.
	//
	// The callback that gets invoked by each item's "X", that's
	// intuitively indicates to click there to remove the item.
	x::w::new_itemlayoutmanager nilm
		{
		 []
		 (ONLY IN_THREAD,
		  size_t i,
		  const x::w::itemlayout_lock &lock,
		  const x::w::callback_trigger_t &trigger,
		  const x::w::busy &mcguffin)
		 {
			 app my_app;

			 if (my_app)
				 my_app->remove_toppings(lock, i);
		 }
		};

	auto container=factory->create_focusable_container
		([]
		 (const auto &c)
		 {
			 // Initially nothing here
		 },
		 nilm);

	return std::tuple{field, container};
}

void itemlayoutmanager()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::input_fieldptr toppings_field;
	x::w::focusable_containerptr toppings_list;

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 std::tie(toppings_field, toppings_list)=
				 create_mainwindow(main_window);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	// Create the application object, with the fields.

	app my_app{x::ref<appObj>::create(main_window,
					  toppings_field,
					  toppings_list)};

	main_window->set_window_title("Sam's pizzeria");
	main_window->set_window_class("main",
				      "itemlayoutmanager@examples.w.libcxx.com");
	// Put an initial item into the list.
	my_app->add_toppings({"cheese"});
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

	// Show what's been ordered:

	auto toppings=my_app->toppings.get();

	std::cout << "Ordered: " << x::join(toppings.begin(),
					    toppings.end(),
					    ", ") << std::endl;
}

int main(int argc, char **argv)
{
	try {
		itemlayoutmanager();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
