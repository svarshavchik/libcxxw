/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
#include "x/w/canvas.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/focusable_label.H"
#include "x/w/input_field_lock.H"
#include "x/w/shortcut.H"
#include <string>
#include <iostream>

#include "testcombobox.inc.H"

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef ref<close_flagObj> close_flag_ref;

static const char *moretext[]={
	"incididunt ut",
	"labore et dolore",
	"magna"
	"aliqua",
	"Ut enim ad",
	"minim veniam",
	"quis nostrud",
	"exercitation"
	"ullamco",
	"laboris nisi",
	"ut aliquip",
	"ex ea",
	"commodo consequat",
	"Duis aute",
	"irure dolor",
	"in reprehenderit",
	"in voluptate",
	"velit"
	"esse",
	"cillum dolore",
	"eu fugiat",
	"nulla pariatur",
	"Excepteur sint",
	"occaecat cupidatat",
	"non proident",
	"sunt in",
	"culpa qui",
	"officia deserunt",
	"mollit anim",
	"id est"
	"laborum"
};

focusable_container create_combobox(const factory &f,
				    const new_custom_comboboxlayoutmanager &ncc)
{
	return f->create_focusable_container
		([]
		 (const auto &new_container) {

			standard_comboboxlayoutmanager lm=new_container
				->get_layoutmanager();

			lm->replace_all_items({
					"Lorem ipsum",
						"dolor sit",
						"ament",
						"consectetur",
						"adipisicing",
						"elid set",
						"do",
						"eiusmod tempor",
						});
		}, ncc);
}

focusable_container create_editable_combobox(const factory &f)
{
	new_editable_comboboxlayoutmanager ec{
		[] (const auto &info)
		{
			if (info.selected_flag)
				std::cout << "Selected item #"
					  << info.item_index
					  << std::endl;
		}
	};

	return create_combobox(f, ec);
}

focusable_container create_standard_combobox(const factory &f)
{
	new_standard_comboboxlayoutmanager sc{
		[] (const auto &info)
		{
			if (info.selected_flag)
				std::cout << "Selected item #"
					  << info.item_index
					  << std::endl;
		}
	};

	return create_combobox(f, sc);
}


void testcombobox(const testcombobox_options &options)
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::focusable_containerptr combobox;

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 auto factory=layout->append_row();

			 if (options.editable->value)
				 combobox=create_editable_combobox(factory);
			 else
				 combobox=create_standard_combobox(factory);

			 factory=layout->append_row();

			 factory->halign(halign::fill)
			 .create_normal_button_with_label({"Append"},{"Alt", 'A'})
			 ->on_activate([combobox, i=0](const auto &) mutable {
					 standard_comboboxlayoutmanager lm=
						 combobox->get_layoutmanager();

					 lm->append_item(moretext[i]);

					 i=(i+1) % (sizeof(moretext)/
						    sizeof(moretext[0]));
					 std::cout << "Appended" << std::endl;
				 });

			 factory=layout->append_row();

			 factory->halign(halign::fill)
			 .create_normal_button_with_label("Delete")
			 ->on_activate([combobox](const auto &) {
					 standard_comboboxlayoutmanager lm=
						 combobox->get_layoutmanager();

					 if (lm->size() == 0)
						 return;

					 lm->remove_item(0);
				 });

			 factory=layout->append_row();

			 factory->halign(halign::center)
			 .create_normal_button_with_label({"Append Separator"})
			 ->on_activate([combobox](const auto &) {
					 standard_comboboxlayoutmanager lm=
						 combobox->get_layoutmanager();

					 lm->append_item("");
				 });

			 factory=layout->append_row();

			 factory->halign(halign::center)
			 .create_normal_button_with_label({"Insert Separator"})
			 ->on_activate([combobox](const auto &) {
					 standard_comboboxlayoutmanager lm=
						 combobox->get_layoutmanager();

					 lm->insert_item(0, "");
				 });

			 factory=layout->append_row();

			 factory->halign(halign::center)
			 .create_normal_button_with_label({"Disable/Enable 1st item"})
			 ->on_activate([combobox](const auto &) {
					 standard_comboboxlayoutmanager lm=
						 combobox->get_layoutmanager();

					 lm->enabled(0, !lm->enabled(0));
				 });

		 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

	standard_comboboxlayoutmanager lm=combobox->get_layoutmanager();

	auto n=lm->selected();

	if (n)
	{
		size_t i=n.value();

		std::cout << "Final selection: " << i << std::endl;
	}

	if (options.editable->value)
	{
		editable_comboboxlayoutmanager lm=combobox->get_layoutmanager();

		input_lock lock{lm};

		std::cout << "Final selection: " << lock.get() << std::endl;
	}
}

int main(int argc, char **argv)
{
	try {
		testcombobox_options options;

		options.parse(argc, argv);

		testcombobox(options);
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
