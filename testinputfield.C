/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/obj.H>
#include <x/strtok.H>

#include "x/w/main_window.H"
#include "x/w/input_field.H"
#include "x/w/input_field_lock.H"
#include "x/w/label.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
#include "x/w/itemlayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include <x/weakcapture.H>
#include <string>
#include <iostream>
#include <algorithm>
#include <courier-unicode.h>

class close_flagObj : public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef LIBCXX_NAMESPACE::ref<close_flagObj> close_flag_ref;

#include "testinputfield.inc.H"

class appdataObj : public inputfields, virtual public LIBCXX_NAMESPACE::obj {

public:
	using inputfields::inputfields;
};

static void search_function(const LIBCXX_NAMESPACE::w::input_field_search_info
			    &info)
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

	if (info.search_string.size() > 2)
		sleep(1);

	for (const auto &search:lorem_ipsum)
	{
		auto iter=std::search(search.begin(), search.end(),
				      info.search_string.begin(),
				      info.search_string.end(),
				      []
				      (const auto &a,
				       const auto &b)
				      {
					      return unicode_uc(a) ==
						      unicode_uc(b);
				      });

		if (iter==search.end())
			continue;

		info.search_results.push_back(search);

		auto iter_end=iter+info.search_string.size();

		LIBCXX_NAMESPACE::w::text_param t;

		if (iter != search.begin())
		{
			t("sans_serif"_theme_font);
			t(std::u32string{search.begin(), iter});
		}
		t("sans_serif;weight=bold"_theme_font);
		t(LIBCXX_NAMESPACE::w::text_decoration::underline);
		t(std::u32string{iter, iter_end});

		if (iter_end != search.end())
		{
			t("sans_serif"_theme_font);
			t(LIBCXX_NAMESPACE::w::text_decoration::none);
			t(std::u32string{iter_end, search.end()});
		}
		info.search_items.push_back(t);
	}
}

void testbutton()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	typedef LIBCXX_NAMESPACE::ref<appdataObj> appdata_t;

	auto main_window=
		LIBCXX_NAMESPACE::w::screen::create()
		->create_mainwindow
		([&]
		 (const auto &main_window)
		 {
			 inputfieldsptr fields;

			 LIBCXX_NAMESPACE::w::gridlayoutmanager
			 layout=main_window->get_layoutmanager();
			 LIBCXX_NAMESPACE::w::gridfactory factory=
			 layout->append_row();

			 LIBCXX_NAMESPACE::w::border_infomm my_border;

			 my_border.color1=LIBCXX_NAMESPACE::w::black;

			 my_border.width=.1;
			 my_border.height=.1;
			 my_border.rounded=true;
			 my_border.radius=2;

			 LIBCXX_NAMESPACE::w::new_borderlayoutmanager neb
				 {[&]
				  (const auto &f)
				  {
					  auto l=f->create_label
						  ("Border Layout Manager");

					  LIBCXX_NAMESPACE::w::radial_gradient
						  g;

					  g.gradient={
						      {0, LIBCXX_NAMESPACE::w
						       ::silver},
						      {1, LIBCXX_NAMESPACE::w
						       ::white}};
					  l->set_background_color(g);
					  l->show();
				  }};

			 neb.border=my_border;
			 neb.hpad=3;
			 neb.vpad=3;

			 factory->create_container
				 ([&]
				  (const auto &c)
				  {
				  },
				  neb);

			 factory=layout->append_row();

			 neb=LIBCXX_NAMESPACE::w::new_borderlayoutmanager
				 {[&]
				  (const auto &f)
				  {
					  auto l=f->create_label
						  ("Border Layout Manager");
				  }};

			 neb.title("This is a title");

			 auto be=factory->create_container
				 ([&]
				  (const auto &)
				  {
				  },
				  neb);

			 LIBCXX_NAMESPACE::w::borderlayoutmanager{
				 be->get_layoutmanager()
					 }->get()->show();

			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::new_itemlayoutmanager nilm;

			 auto itemc=factory->create_focusable_container
				 ([]
				  (const auto &ignore)
				  {
				  },
				  nilm);

			 itemc->show();

			 LIBCXX_NAMESPACE::w::itemlayoutmanager ilm
				 {itemc->get_layoutmanager()};

			 ilm->on_remove([]
					(ONLY IN_THREAD,
					 size_t i,
					 const auto &lock,
					 const auto &trigger,
					 const auto &busy)
					{
						lock.layout_manager
							->remove_item(i);
						std::cout << "Removed "
							  << i << std::endl;
					});

			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::input_field_config conf1{30, 1,
					 true};

			 conf1.hint("Enter something here");

			 conf1.maximum_size=30;

			 fields.first=factory->create_input_field({""},
								  conf1);

			 fields.first->on_change
			 ([]
			  (THREAD_CALLBACK,
			   const auto &what) {
				 switch (what.type) {
				 case LIBCXX_NAMESPACE::w::input_change_type::deleted:
					 std::cout << "deleted ";
					 break;
				 case LIBCXX_NAMESPACE::w::input_change_type::inserted:
					 std::cout << "inserted ";
					 break;
				 case LIBCXX_NAMESPACE::w::input_change_type::set:
					 std::cout << "set ";
					 break;
				 }
				 std::cout << " ["
					   << what.deleted
					   << "/"
					   << what.inserted
					   << "]" << std::endl;
			 });

			 fields.first->on_validate
				 ([f=make_weak_capture(fields.first), itemc]
				  (ONLY IN_THREAD,
				   const auto &trigger)
				  {
					  auto got=f.get();

					  if (!got)
						  return true;

					  auto &[f]=*got;

					  LIBCXX_NAMESPACE::w::input_lock
						  lock{f};

					  std::vector<std::u32string> words;

					  LIBCXX_NAMESPACE::strtok_str
						  (lock.get_unicode(),
						   U",", words);

					  f->set(U"");

					  for (auto &w:words)
					  {
						  auto b=w.begin();
						  auto e=w.end();

						  LIBCXX_NAMESPACE::trim(b, e);

						  if (b == e)
							  continue;

						  LIBCXX_NAMESPACE::w
							  ::itemlayoutmanager
							  lm=itemc
							  ->get_layoutmanager();

						  lm->append_item
							  ([&]
							   (const auto &f)
							   {
								   f->create_label
									   (std::u32string{b, e})
									   ->show();
							   });

						  LIBCXX_NAMESPACE::w::label
							  l=lm->get_item(0);
					  }
					  return true;
				  });

			 auto context_popup=fields.first->create_popup_menu
				 ([&]
				  (const auto &llm)
				  {
					  llm->append_items
						  ({
						    [w=LIBCXX_NAMESPACE::make_weak_capture(fields.first)]
						    (ONLY IN_THREAD,
						     const auto &status_info)
						    {
							    auto got=w.get();

							    if (!got)
								    return;

							    auto &[w]=*got;
							    w->focusable_cut_or_copy_selection(LIBCXX_NAMESPACE::w::cut_or_copy_op::copy);
						    },

						    {"Copy"},
						    [w=LIBCXX_NAMESPACE::make_weak_capture(fields.first)]
						    (ONLY IN_THREAD,
						     const auto &status_info)
						    {
							    auto got=w.get();

							    if (!got)
								    return;

							    auto &[w]=*got;

							    w->focusable_cut_or_copy_selection(LIBCXX_NAMESPACE::w::cut_or_copy_op::cut);
						    },
						    {"Cut"},
						    [w=LIBCXX_NAMESPACE::make_weak_capture(fields.first)]
						    (ONLY IN_THREAD,
						     const auto &status_info)
						    {
							    auto got=w.get();

							    if (!got)
								    return;

							    auto &[w]=*got;

							    w->focusable_receive_selection();
						    },
						    {"Paste"}});
				  });

			 fields.first->install_contextpopup_callback
				 ([context_popup,
				   field=make_weak_capture(fields.first)]
				  (ONLY IN_THREAD,
				   const auto &e,
				   const auto &trigger,
				   const auto &busy)
				  {
					  auto got=field.get();

					  if (!got)
						  return;

					  auto & [field]=*got;

					  LIBCXX_NAMESPACE::w::listlayoutmanager
						  l=context_popup
						  ->get_layoutmanager();

					  bool cut_or_copy=field
						  ->focusable_cut_or_copy_selection
						  (IN_THREAD,
						   LIBCXX_NAMESPACE::w
						   ::cut_or_copy_op::available);

					  l->enabled(IN_THREAD, 0, cut_or_copy);
					  l->enabled(IN_THREAD, 1, cut_or_copy);
					  l->enabled(IN_THREAD, 2,
						     e->selection_has_owner() &&
						     e->selection_can_be_received());
					  context_popup->show_all();
				  });
			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::input_field_config conf2{30, 4};

			 conf2.vertical_scrollbar=
			 LIBCXX_NAMESPACE::w::scrollbar_visibility
			 ::automatic_reserved;

			 fields.second=factory->halign(LIBCXXW_NAMESPACE::halign::right)
			 .create_input_field
			 ({"sans_serif"_font,
					 LIBCXX_NAMESPACE::w::rgb{
					 0, 0,
						 LIBCXX_NAMESPACE::w::rgb::maximum},
					 "Hello world!"}, conf2);

			 LIBCXX_NAMESPACE::w::label_config config;

			 config.widthmm=30;

			 fields.second->create_tooltip("A brief message, a few lines long.", config);

			 factory=layout->append_row();

			 fields.second=factory->create_input_field
			 ("The quick brown fox jumped over "
			  "the lazy dog's tail", conf2);

			 factory=layout->append_row();

			 auto n=factory->create_input_field("", {5});

			 n->set_string_validator([]
						 (THREAD_CALLBACK,
						  const std::string &v,
						  int *nptr,
						  const LIBCXX_NAMESPACE::w
						  ::input_field &f,
						  const auto &ignore)
						 -> std::optional<int> {
							 if (nptr && *nptr >= 0
							     && *nptr <= 99)
								 return *nptr;

							 if (!v.empty())
								 f->stop_message("0-99, please...");
							 return std::nullopt;
						 },
						 []
						 (const std::optional<int> &n)
						 -> std::string {
							 if (!n)
								 return "";

							 return std::to_string(*n);
						 });

			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::input_field_config conf3{20};

			 conf3.set_password();

			 fields.password=factory->create_input_field({"rosebud"},
								     conf3);

			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::input_field_config conf4{5};

			 conf4.set_default_spin_control_factories();

#if 0
			 conf4.set_spin_control_factories
			 ([](const auto &factory) {
				 factory->create_label({
						 "liberation mono"_font,
							 "-"

					 });
			 }, [](const auto &factory) {
				 factory->create_label({
						 "liberation mono"_font,
							 "+"

					 });
			 });
#endif
			 fields.spinner=factory->create_input_field("", conf4);

			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::input_field_config conf5{20};

			 conf5.hint = "Search...";
			 conf5.input_field_search_callback=search_function;

			 auto search=factory->create_input_field({},
								 conf5);

			 search->on_validate
				 ([f=make_weak_capture(search)]
				  (ONLY IN_THREAD,
				   const auto &trigger)
				  {
					  auto got=f.get();

					  if (!got)
						  return true;

					  auto &[f]=*got;

					  LIBCXX_NAMESPACE::w::input_lock
						  lock{f};

					  std::cout << "Search found: "
						    << lock.get()
						    << std::endl;
					  return true;
				  });

			 factory=layout->append_row();

			 LIBCXX_NAMESPACE::w::input_field_config conf6{12};

			 conf6.maximum_size=11;
			 conf6.autoselect=true;
			 conf6.autodeselect=true;

			 factory->create_input_field("           ", conf6)
				 ->on_default_filter
				 ([](char32_t c)
				  {
					  return c >= '0' && c <= '9';
				  },
					 {3, 7});

			 factory=layout->append_row();

			 auto b=factory->create_special_button_with_label({"Ok"});
			 b->on_activate([close_flag](THREAD_CALLBACK,
						     const auto &,
						     const auto &) {
						close_flag->close();
					});
			 factory=layout->append_row();
			 b=factory->create_special_button_with_label("Enable/Disable");
			 b->on_activate
			 ([flag=true,
			   mw=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
			  (THREAD_CALLBACK,
			   const auto &,
			   const auto &) mutable {

				 flag=!flag;

				 auto got=mw.get();

				 if (!got)
					 return;

				 auto &[mw]=*got;

				 appdata_t appdata=mw->appdata;

				 appdata->first->set_enabled(flag);
				 appdata->second->set_enabled(flag);
				 appdata->password->set_enabled(flag);
				 appdata->spinner->set_enabled(flag);
			 });


			 factory=layout->append_row();

			 factory->halign(x::w::halign::fill).create_container
			 (
			  []
			  (const auto &c)
		{
			x::w::gridlayoutmanager layout=c->get_layoutmanager();
			layout->requested_col_width(1, 100);
			auto factory=layout->append_row();
			factory->create_label({"Foo"});
			factory->create_label({"Bar"});
		},
			  x::w::new_gridlayoutmanager());

			 main_window->appdata=appdata_t::create(fields);
		 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

	appdata_t appdata=main_window->appdata;

	LIBCXX_NAMESPACE::w::input_lock lock_first{appdata->first},
		lock_second{appdata->second},
			lock_password{appdata->password};

	std::cout << lock_first.get() << std::endl;
	std::cout << lock_second.get() << std::endl;
	std::cout << lock_password.get() << std::endl;

	std::cout << lock_first.size() << std::endl;

	auto [pos1, pos2]=lock_first.pos();

	std::cout << "POS: [" << pos1 << ", " << pos2 << "]" << std::endl;

	std::cout << "Done" << std::endl;
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testbutton();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
