/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/focusable_container.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/panefactory.H"
#include "x/w/button.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/canvas.H"

#include <string>
#include <iostream>
#include <sstream>

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

static void create_pane(const LIBCXX_NAMESPACE::w::panelayoutmanager &lm)
{
}

static void insert(const LIBCXX_NAMESPACE::w::container &c,
		   LIBCXX_NAMESPACE::w::scrollbar_visibility v)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->insert_panes(0);

	f->set_initial_size(20)
		.set_scrollbar_visibility(v)
		.create_label("Lorem ipsum\n"
			      "dolor sit amet\n"
			      "consectetur\n"
			      "adipisicing elit sed\n"
			      "do eiusmod\n"
			      "tempor incididunt ut\n"
			      "labore et\n"
			      "dolore magna\n"
			      "aliqua")->show();

	LIBCXX_NAMESPACE::w::pane_lock lock{lm};
	LIBCXX_NAMESPACE::w::label l=lm->get(0);
}

static void append(const LIBCXX_NAMESPACE::w::container &c,
		   LIBCXX_NAMESPACE::w::scrollbar_visibility v)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->append_panes();

	f->set_background_color("100%")
		.padding(2.0)
		.set_scrollbar_visibility(v);

	f->create_label("Lorem ipsum "
			"dolor sit amet\n"
			"consectetur "
			"adipisicing elit sed\n"
			"do eiusmod "
			"tempor incididunt ut\n"
			"labore et "
			"dolore magna\n"
			"aliqua")->show();
}

static void remove_first(const LIBCXX_NAMESPACE::w::container &c)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	if (lm->size() > 0)
		lm->remove_pane(0);
}

static void remove_last(const LIBCXX_NAMESPACE::w::container &c)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	auto s=lm->size();

	if (s > 0)
		lm->remove_pane(s-1);
}

static void replace_first(const LIBCXX_NAMESPACE::w::container &c,
			  LIBCXX_NAMESPACE::w::scrollbar_visibility v)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->replace_panes(0);

	f->set_background_color("100%")
		.padding(2.0)
		.set_scrollbar_visibility(v)
		.valign(LIBCXX_NAMESPACE::w::valign::bottom);

	f->create_label("Lorem ipsum "
			"dolor sit amet\n"
			"consectetur "
			"adipisicing elit sed\n"
			"do eiusmod "
			"tempor incididunt ut\n"
			"labore et "
			"dolore magna\n"
			"aliqua")->show();
}

static void insert_list(const LIBCXX_NAMESPACE::w::container &c)
{
	LIBCXX_NAMESPACE::w::panelayoutmanager lm=c->get_layoutmanager();

	LIBCXX_NAMESPACE::w::panefactory f=lm->insert_panes(0);

	LIBCXX_NAMESPACE::w::new_listlayoutmanager nlm;

	nlm.variable_height();

	f->set_initial_size(20)
		.set_scrollbar_visibility(LIBCXX_NAMESPACE::w
					  ::scrollbar_visibility::never)
		.halign(LIBCXX_NAMESPACE::w::halign::fill)
		.create_focusable_container
		([]
		 (const auto &container)
		 {
			 LIBCXX_NAMESPACE::w::listlayoutmanager lm=
				 container->get_layoutmanager();

			 lm->append_items({
					 "Lorem ipsum",
					 "dolor sit amet",
					 "consectetur",
					 "adipisicing elit sed",
					 "do eiusmod",
					 "tempor incididunt ut",
					 "labore et",
					 "dolore magna",
					 "aliqua"});
		 },
		 nlm)->show();
}

static LIBCXX_NAMESPACE::w::scrollbar_visibility get_scrollbar_visibility(const auto &container)
{
	LIBCXX_NAMESPACE::w::standard_comboboxlayoutmanager lm=
		container->get_layoutmanager();

	auto selected=lm->selected();

	size_t i=selected ? *selected:0;

	static const LIBCXX_NAMESPACE::w::scrollbar_visibility values[]={
		LIBCXX_NAMESPACE::w::scrollbar_visibility::never,
		LIBCXX_NAMESPACE::w::scrollbar_visibility::always,
		LIBCXX_NAMESPACE::w::scrollbar_visibility::automatic,
		LIBCXX_NAMESPACE::w::scrollbar_visibility::automatic_reserved
	};

	return values[i];
}

static void create_main_window(const LIBCXX_NAMESPACE::w::main_window &mw,
			       int argc,
			       char **argv)
{
	LIBCXX_NAMESPACE::w::new_panelayoutmanager npl;

	if (argc > 1 && argv[1][0] == 'h')
		npl.horizontal();

	LIBCXX_NAMESPACE::w::gridlayoutmanager layout=mw->get_layoutmanager();
	auto factory=layout->append_row();

	auto pane=factory->colspan(2).create_focusable_container
		([]
		 (const auto &pane_container) {
			create_pane(pane_container->get_layoutmanager());
		}, npl);

	pane->show();

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::right)
		.valign(LIBCXX_NAMESPACE::w::valign::bottom)
		.create_label("New elements'\nscrollbar visibility:")->show();

	auto scrollbar_visibility=
		factory->valign(LIBCXX_NAMESPACE::w::valign::bottom)
		.create_focusable_container
		([&]
		 (const auto &c)
		 {
			 LIBCXX_NAMESPACE::w::standard_comboboxlayoutmanager lm=
			 c->get_layoutmanager();

			 lm->append_items({ "Hide",
						 "Always",
						 "When needed",
						 "Reserve space"});

			 lm->selected(3, true);
		 },
		 LIBCXX_NAMESPACE::w::new_standard_comboboxlayoutmanager{}
		 );

	scrollbar_visibility->show();

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	auto b=factory->create_normal_button_with_label("Insert");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       insert(pane, get_scrollbar_visibility
				      (scrollbar_visibility));
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	b=factory->create_normal_button_with_label("Append");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       append(pane, get_scrollbar_visibility
				      (scrollbar_visibility));
		       });

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	b=factory->create_normal_button_with_label("Remove 1st pane");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       remove_first(pane);
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	b=factory->create_normal_button_with_label("Remove last pane");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       remove_last(pane);
		       });

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	b=factory->create_normal_button_with_label("Replace 1st pane");

	b->show();
	b->on_activate([pane, scrollbar_visibility]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       replace_first(pane, get_scrollbar_visibility
					     (scrollbar_visibility));
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	b=factory->create_normal_button_with_label("Remove all panes");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       	LIBCXX_NAMESPACE::w::panelayoutmanager
					lm=pane->get_layoutmanager();

				lm->remove_all_panes();
		       });

	factory=layout->append_row();

	factory->halign(LIBCXX_NAMESPACE::w::halign::left);
	b=factory->create_normal_button_with_label("Insert list");

	b->show();
	b->on_activate([pane]
		       (THREAD_CALLBACK,
			const auto &trigger,
			const auto &busy) {
			       insert_list(pane);
		       });

	factory->halign(LIBCXX_NAMESPACE::w::halign::right);
	factory->create_canvas()->show();
}

void testpane(int argc, char **argv)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &mw)
			 {
				 create_main_window(mw, argc, argv);
			 });

	main_window->set_window_title("Panes!");
	main_window->set_window_class("main", "testpane@examples.w.libcxx.com");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testpane(argc, argv);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
