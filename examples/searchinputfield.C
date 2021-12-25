/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ondestroy.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <x/w/input_field.H>
#include <x/w/input_field_lock.H>
#include <x/w/container.H>
#include <x/w/button.H>

#include <courier-unicode.h>
#include <string>
#include <iostream>

std::string x::appid() noexcept
{
	return "searchinputfield.examples.w.libcxx.com";
}

static void search_function(const x::w::input_field_search_info &search_info)
{
	static const std::u32string lorem_ipsum[]=
		{
		 U"Lorem Ipsum",
		 U"dolor sit amet",
		 U"consectetur adipisicing elit",
		 U"sed do eiusmod tempor",
		 U"incididunt ut labore",
		 U"et dolore magna aliqua",
		 U"Ut enim ad minim veniam",
		 U"quis nostrud exercitation",
		 U"ullamco laboris nisi",
		 U"ut aliquip ex ea commodo",
		 U"consequat",
		 U"Duis aute irure dolor",
		 U"in reprehenderit",
		 U"in voluptate velit",
		 U"esse cillum dolore eu",
		 U"fugiat nulla pariatur",
		 U"Excepteur sint occaecat cupidatat non proident",
		 U"sunt in culpa qui officia deserunt mollit anim",
		 U"id est laborum",
		};

	// A separate execution thread invokes the search callback. Each time
	// a character gets added to the end of the input field, this search
	// callback gets invoked. The search callback does not get invoked
	// if something gets edited in the middle of the input field, only
	// when more characters are added, or the last character in the input
	// field gets deleted.
	//
	// Simulate a slow search when more than two characters are searched
	// for, by sleeping for one second before returning.

	if (search_info.search_string.size() > 6)
	{
		sleep(1);
	}
	//
	// Type-ahead input continues in the input field, while we're
	// "searching" (sleeping). After the existing search returns, the
	// execution thread calls it again, this time passing in the
	// additional typed-in text.

	else if (search_info.search_string.size() > 2)
	{
		// Let's do something more sophisticated than sleeping for
		// a second. Let's make use of the abort mcguffin. The main
		// library execution thread releases its reference on the
		// mcguffin when it aborts the current search in progress.
		// This could be because the keyboard focus left the search
		// field, or additional text was added to, or removed from,
		// the search field; the current "slow running search" is
		// obsolete. There are no means to forcibly terminate a
		// different execution thread, so the main library execution
		// thread releases its reference on the object as the means
		// of indicating the aborted search, and any results returned
		// by the search thread get ignored.
		//
		// What we'll do is attach a destructor callback to the
		// abort mcguffin.
		//
		// We happen to have a convenient thread-safe bool flag
		// available to us, in the form of a close_flag_ref.

		auto abort_flag=close_flag_ref::create();

		// The destructor callback captures the abort flag, and sets
		// it when the mcguffin gets destroyed.

		search_info.get_abort_mcguffin()->ondestroy
			([abort_flag]
			 {
				 abort_flag->close();
			 });

		// Wait for a second, or until the close flag gets set.
		// Using this approach makes it possible to detect when the
		// currently-running "slow search" can be bailed out of.
		//
		// Note that even in the case of an aborted search, any
		// resulted that eventually get returned from this search
		// callback may or may not end up in the popup, so the
		// search callback is not required to return without setting
		// any results. If the search callback detects an aborted
		// search after "finding" some partial results, it's fine to
		// save the partial results and return them.

		x::mpcobj<bool>::lock lock{abort_flag->flag};

		lock.wait_for(std::chrono::seconds(1),
			      [&]
			      {
				      return *lock;
			      });

		if (*lock)
			return;
	}

	for (const auto &search:lorem_ipsum)
	{
		// search_info.search_string is what's being searched.
		//
		// Case-insensitive unicode search, here.

		auto iter=std::search(search.begin(), search.end(),
				      search_info.search_string.begin(),
				      search_info.search_string.end(),
				      []
				      (const auto &a,
				       const auto &b)
				      {
					      return unicode_uc(a) ==
						      unicode_uc(b);
				      });

		if (iter==search.end())
			continue;

		// Found a "search result". Each individual "search result"
		// gets recorded in two different ways. Firstly, the
		// std::u32string representing the matching search result
		// goes into search_info.search_string.

		search_info.search_results.push_back(search);

		auto iter_end=iter+search_info.search_string.size();

		// Then each search result goes into search_info.search_items,
		// which is a list_item_param. At this time, the only
		// documented list_item_param is a text_param, which is
		// a unicode string with meta font and color mark-ups.

		x::w::text_param t;

		// Each search result may appear in the search popup with
		// custom font and color. We use this to show the portion of
		// each found "search result" that matches the search string
		// in bold font and underlined.
		//
		// First, any initial part of each "search result" is
		// just the default theme font: plain "sans_serif" font.

		if (iter != search.begin())
		{
			t("sans_serif"_theme_font);
			t(std::u32string{search.begin(), iter});
		}

		// And now the matching portion, bolded and underlined

		t("sans_serif;weight=bold"_theme_font);
		t(x::w::text_decoration::underline);
		t(std::u32string{iter, iter_end});

		// If there's anything after the matching portion of each
		// "search result", go back to the plain font.

		if (iter_end != search.end())
		{
			t("sans_serif"_theme_font);
			t(x::w::text_decoration::none);
			t(std::u32string{iter_end, search.end()});
		}
		search_info.search_items.push_back(t);
	}

	// Alternatively, if no special markup is required:
	//
	// std::vector<std::u32string> results;
	//
	// search_info.results(results);
	//
	// results() is a helper function that sets both
	// search_info.search_results and search_info.search_items.
	//
	// NOTE: it is up to the search callback to limit the size of the
	// search results. All "search results" get shown by the popup.
	// The "search results" should be limited to some maximum number
	// of results. Also, if the width of each individual search results
	// is bigger than the popup's width, it gets cut-off. It is the
	// search callback's responsibility to enforce some reasonable limits.
}

void create_mainwindow(const x::w::main_window &main_window,
		       const close_flag_ref &close_flag)
{
	auto layout=main_window->gridlayout();

	x::w::gridfactory factory=layout->append_row();

	x::w::input_field_config search_config{30};

	// Install the search callback.
	search_config.input_field_search={
		search_function
	};

	// Add some padding, to make the window bigger.

	// Align the input field and the button in the middle of the row,
	// vertically.
	auto field=factory->left_padding(10)
		.top_padding(10)
		.bottom_padding(10)
		.valign(x::w::valign::middle)
		.create_input_field("", search_config);

	// Use the on_validate callback to "report" search results.
	// on_validate() normally gets invoked to validate the input field
	// after it gets tabbed out of. With a search callback, on_validate()
	// also gets invoked after picking something from the search popup.
	//
	// Note that what's manually typed in may not match anything that
	// the search found, if the popup does not get used.

	field->on_validate
		([]
		 (ONLY IN_THREAD,
		  x::w::input_lock &lock,
		  const x::w::callback_trigger_t &trigger)
		 {
			 std::cout << "Search found: "
				   << lock.get()
				   << std::endl;
			 return true;
		 });

	auto ok=factory->right_padding(10)
		.top_padding(10)
		.bottom_padding(10)
		.create_button({"Ok"}, {
				x::w::default_button(),
				x::w::shortcut{'\n'},
			});

	ok->on_activate([close_flag]
			(ONLY IN_THREAD,
			 const x::w::callback_trigger_t &trigger,
			 const x::w::busy &ignore)
			{
				close_flag->close();
			});
}

void searchinputfield()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=
		x::w::main_window::create([&]
					  (const auto &main_window)
					  {
						  create_mainwindow(main_window,
								    close_flag);
					  });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("QuackQuackRun!");
	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();

}

int main(int argc, char **argv)
{
	try {
		searchinputfield();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
