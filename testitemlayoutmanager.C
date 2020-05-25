/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"

#include <x/exception.H>
#include <x/destroy_callback.H>

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/factory.H"
#include "x/w/itemlayoutmanager.H"
#include "x/w/label.H"
#include "x/w/input_field.H"
#include "x/w/container.H"
#include "x/w/focusable_container.H"
#include "x/w/canvas.H"

#include <x/strtok.H>
#include <x/join.H>
#include <x/mutex.H>
#include <x/weakcapture.H>
#include <courier-unicode.h>

#include "testitemlayoutmanager.inc.H"

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


auto create_mainwindow(const main_window &mw)
{
	gridlayoutmanager layout=mw->get_layoutmanager();

	layout->row_alignment(0, valign::middle);

	gridfactory f=layout->append_row();

	f->create_label("Pizza toppings:");

	input_field_config config{30};

	auto field=f->create_input_field("", config);

	f=layout->append_row();

	f->create_canvas();

	new_itemlayoutmanager nilm;

	bool initialized=false;
	auto container=f->create_focusable_container
		([&]
		 (const auto &c)
		 {
			 initialized=true;
		 },
		 nilm);

	if (!initialized)
		throw EXCEPTION("itemlayoutmanager creator not called");

	field->on_validate
		([objects=make_weak_capture(container, field)]
		 (ONLY IN_THREAD,
		  const callback_trigger_t &triggering_event)
		 {
			 auto got=objects.get();

			 if (!got)
				 return true;

			 auto &[container, field]=*got;

			 std::vector<std::string> words;

			 {
				 input_lock lock{field};

				 strtok_str(lock.get(), ",", words);

				 field->set("");
			 }

			 auto nonempty_word=words.begin();

			 for (auto &w:words)
			 {
				 auto b=w.begin();
				 auto e=w.end();
				 trim(b, e);

				 auto word=unicode::iconvert
					 ::convert_tocase
					 ({b, e},
					  unicode_default_chset(),
					  unicode_tc,
					  unicode_lc);

				 if (word.empty())
					 continue;

				 *nonempty_word++=word;
			 }

			 words.erase(nonempty_word, words.end());

			 if (words.empty())
				 return true;

			 itemlayoutmanager lm=
				 container->get_layoutmanager();

			 for (const auto &word:words)
			 {
				 lm->append_item
					 ([&]
					  (const factory &f)
					  {
						  f->create_label(word)->show();
					  });
			 }

			 return true;
		 });

	return std::tuple{field, container};
}

void testitemlayoutmanager(const testitems_options &options)
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	input_fieldptr toppings_field;
	focusable_containerptr toppings_list;

	auto mw=main_window::create
		([&]
		 (const auto &mw)
		 {
			 std::tie(toppings_field, toppings_list)=
				 create_mainwindow(mw);
		 });

	mw->on_disconnect([]
			  {
				  _exit(1);
			  });

	guard(mw->connection_mcguffin());

	mw->set_window_title("Sam's pizzeria");

	mw->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const busy &ignore)
		 {
			 close_flag->close();
		 });

	mw->show_all();

	if (options.testitems->value)
	{
		mpcobj<bool>::lock lock{close_flag->flag};

		size_t n=0;

		do
		{
			if (n == 5)
				break;
			{
				itemlayoutmanager ilm=
					toppings_list->get_layoutmanager();

				switch (n) {
				case 1:
					ilm->append_item
						([]
						 (const auto &f)
						 {
							 f->create_label("Pepperoni")->show();
						 });

					{
						label l=ilm->get_item(0);
					}
					break;
				case 2:
					ilm->append_item
						([]
						 (const auto &f)
						 {
							 f->create_label("Sausage")->show();
						 });
					break;
				case 3:
					ilm->insert_item
						(0,
						 []
						 (const auto &f)
						 {
							 f->create_label("Extra cheese")->show();
						 });
					break;
				case 4:
					ilm->remove_item(2);
					break;
				}
				std::cout << ilm->size() << " items now"
					  << std::endl;
			}
			lock.wait_for(std::chrono::seconds(2),
				      [&] { return *lock; });
			++n;
		} while (!*lock);
		return;
	}

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

}

int main(int argc, char **argv)
{
	try {
		testitems_options options;

		options.parse(argc, argv);

		testitemlayoutmanager(options);
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
