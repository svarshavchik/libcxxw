/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include "close_flag.H"

#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/input_field.H>
#include <x/w/input_field_lock.H>
#include <x/w/input_field_filter.H>
#include <x/w/canvas.H>
#include <x/w/container.H>
#include <x/w/button.H>

#include <x/weakcapture.H>

#include <string>
#include <iostream>
#include <sstream>

std::string x::appid() noexcept
{
	return "filteredinput.examples.w.libcxx.com";
}

void create_mainwindow(const x::w::main_window &main_window,
		       const close_flag_ref &close_flag)
{
	auto layout=main_window->gridlayout();

	layout->row_alignment(0, x::w::valign::middle);

	x::w::gridfactory factory=layout->append_row();

	factory->create_label("Your ID:");

	// 11 characters in the input field. As always, the input field's
	// width needs to be one more than its maximum size, in order to leave
	// room for the cursor when it's after the last character in the
	// input field, without scrolling.

	x::w::input_field_config config{12};

	config.maximum_size=11;

	// Automatically selecting and deselecting everything in the input
	// field is often desirable, for filtered input fields.
	config.autoselect=true;
	config.autodeselect=true;

	// Explicit left-to-right text direction
	config.direction=x::w::bidi::left_to_right;

	// We create the input field with its contents initially "empty", but
	// in reality it's full of spaces, with dashes where the input
	// is separated.
	auto field=factory->create_input_field(U"   -   -   ", config);

	field->on_filter
		// The filter callback needs to have its own input field, and it
		// must be weakly-captured to avoid a circular reference.
		([me=x::make_weak_capture(field)]
		 (ONLY IN_THREAD,
		  const x::w::input_field_filter_info &s)
		 {
			 // Recover a strong reference to my input field.

			 auto got=me.get();

			 if (!got)
				 return;

			 auto &[me]=*got;

			 // starting_pos demarcate the starting position
			 // of the modified text, and n_deleted is how
			 // many characters are affected.
			 //
			 // n_deleted is 0 if new text is being added,
			 // only.
			 auto starting_pos=s.starting_pos->pos();
			 auto n_deleted=s.n_deleted;

			 // If this is cursor movement only: if the cursor
			 // is moved into a position where the dash is,
			 // just keep moving.

			 if (s.type == x::w::input_filter_type::move_only)
			 {
				 // Use original_pos() to find out where the
				 // cursor used to be. This indicates in which
				 // direction the cursor was moved, so we go
				 // one notch in the same direction.
				 switch (starting_pos) {
				 case 3:
					 s.move(s.original_pos() > 3
						? 2:4);
					 return;
				 case 7:
					 s.move(s.original_pos() > 7
						? 6:8);
					 return;
				 }
				 return;
			 }

			 auto current_contents=
				 x::w::input_lock{me}.get_unicode();

			 // If we, allegedly are starting somewhere other
			 // than the logical end of data, nudge things in the
			 // right direction. This happens if you skip ahead of
			 // the spaces, and begin typing in the middle of the
			 // field.

			 for (auto i=starting_pos; i>0; --i)
				 if (current_contents.at(i-1) == ' ')
				 {
					 starting_pos=i-1;
					 n_deleted=0;
				 }

			 // Allow stuff to be deleted only at the end of the
			 // input field. s.starting_pos is the starting
			 // location to be deleted. Everything on or after
			 // starting_pos+n_deleted should either be a space
			 // or a dash.

			 if (n_deleted > 0)
			 {
				 // If the region to delete ends on a dash,
				 // make sure to include it in the region.

				 size_t end_pos=starting_pos+n_deleted;

				 if (end_pos == 3 || end_pos == 7)
				 {
					 ++end_pos;
					 ++n_deleted;
				 }

				 // Ditto for the starting position, so
				 // backspacing with the cursor just after
				 // the dash deletes the dash, and the preceding
				 // digit.

				 switch (starting_pos) {
				 case 3:
				 case 7:
					 --starting_pos;
					 ++n_deleted;
				 }

				 while (end_pos < current_contents.size())
				 {
					 switch(current_contents[end_pos]) {
					 case ' ':
					 case '-':
						 break;
					 default:
						 // Do nothing. Just return.
						 return;
					 }
					 ++end_pos;
				 }
			 }

			 std::u32string new_contents;

			 // s.new_contents is what's new. Ignore everything
			 // except digits. So if a dash gets typed or pasted,
			 // it gets ignored. Instead, we pick off only the
			 // digits, and add the dashes ourselves, in the right
			 // place.

			 new_contents.reserve(s.new_contents.size());

			 for (auto c:s.new_contents)
			 {
				 if (c < '0' || c > '9')
					 continue;

				 switch (starting_pos+new_contents.size()) {
				 case 3:
				 case 7:
					 new_contents.push_back('-');
					 break;
				 }
				 new_contents.push_back(c);
			 }

			 // This is the ending cursor position.

			 auto end_pos=starting_pos+new_contents.size();

			 // However, if n_deleted was more, more stuff is
			 // to be deleted, so pad out new_contents, to
			 // effectively delete it, by overwriting it.

			 while (new_contents.size() < n_deleted)
			 {
				 switch (starting_pos+new_contents.size()) {
				 case 3:
				 case 7:
					 new_contents.push_back('-');
					 break;
				 default:
					 new_contents.push_back(' ');
					 break;
				 }
			 }

			 // At this point, we're effectively replacing the
			 // existing contents of the input field, so n_deleted
			 // should always be the same as new_contents.size();

			 n_deleted=new_contents.size();

			 // Apply the filtered changes to the input field.

			 // update() first two parameters are the starting
			 // and the ending position of the modified text,
			 // which are provided in the input_field_filter_info.
			 //
			 // However as the result of the above we've arrived
			 // at, possibly, a different range of the text to
			 // be modified, so we'll create new iterators.

			 auto new_starting_pos=
				 s.starting_pos->pos(starting_pos);

			 auto new_ending_pos= n_deleted
				 ? s.starting_pos->pos(starting_pos+n_deleted)
				 // This optimization is not really needed...
				 : new_starting_pos;

			 s.update(new_starting_pos, new_ending_pos,
				  new_contents);

			 // If we wind up on top of a dash, advance past it.
			 if (end_pos == 3 || end_pos == 7)
				 ++end_pos;

			 // And place the cursor where it should be.
			 s.move(end_pos);
		 });

	factory=layout->append_row();

	factory->create_canvas();

	factory->create_button("Ok", x::w::default_button() )
		->on_activate([close_flag]
			      (ONLY IN_THREAD,
			       const auto &trigger,
			       const auto &mcguffin)
			      {
				      close_flag->close();
			      });

}

void filteredinputfield()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 create_mainwindow(main_window, close_flag);
		 });

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	guard(main_window->connection_mcguffin());

	main_window->set_window_title("Enter your ID");
	main_window->set_window_class("main",
				      "filteredinput.examples.w.libcxx.com");
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
		filteredinputfield();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
