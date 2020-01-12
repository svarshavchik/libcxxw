/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/updated_position_info.H"

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/button.H"
#include "x/w/label.H"

#include <x/obj.H>
#include <x/mpobj.H>
#include <x/destroy_callback.H>
#include <x/exception.H>
#include <iostream>

LIBCXX_NAMESPACE::w::elementObj::implObj *DEBUG;

static LIBCXX_NAMESPACE::mpcobj<size_t> button_draw_counter{0};
static LIBCXX_NAMESPACE::mpcobj<size_t> flush_counter{0};

#define DEBUG_EXPLICIT_REDRAW() do {					\
		if (this == DEBUG)				\
		{							\
			std::cout << "DREW BUTTON" << std::endl;	\
			LIBCXX_NAMESPACE::mpcobj<size_t>::lock		\
				l{button_draw_counter};			\
			++*l;						\
			l.notify_all();					\
		}							\
	} while (0);

#include "element_impl.C"

#define DEBUG_FLUSH_REDRAWN_AREAS() do {			  \
		if (!DEBUG)					  \
			return;					  \
		std::cout << "FLUSH: " << r << std::endl;	  \
		LIBCXX_NAMESPACE::mpcobj<size_t>::lock			\
			l{flush_counter};				\
		++*l;							\
		l.notify_all();						\
	} while(0);
#include "generic_window_handler.C"

struct strict_weak_order {
	LIBCXX_NAMESPACE::w::rectangle a, b;
};

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

typedef LIBCXX_NAMESPACE::w::updated_position_move_info
updated_position_move_info;

static void sanity_check()
{
	static const strict_weak_order nw_tests[]=
		{
		 { {0, 0}, {1, 1} },
		 { {0, 0}, {1, 0} },
		 { {0, 0}, {0, 1} },
		};

	static const strict_weak_order ne_tests[]=
		{
		 { {1, 0}, {0, 1} },
		 { {1, 0}, {0, 0} },
		 { {1, 0}, {1, 1} },
		};

	static const strict_weak_order sw_tests[]=
		{
		 { {0, 1}, {1, 1} },
		 { {0, 1}, {1, 0} },
		 { {0, 1}, {0, 0} },
		};

	static const strict_weak_order se_tests[]=
		{
		 { {1, 1}, {0, 1} },
		 { {1, 1}, {1, 0} },
		 { {1, 1}, {0, 0} },
		};

	static const struct {
		LIBCXX_NAMESPACE::w::coord_t move_to_x;
		LIBCXX_NAMESPACE::w::coord_t move_to_y;

		updated_position_move_info::summary expected_result;

		size_t updated_position_move_info::summary::*expected_chosen;

		const strict_weak_order *strict_weak_order_tests;

	} tests[] = {
		     {0, 0, {1, 0, 0, 0},
		      &updated_position_move_info::summary::nw,
		      nw_tests},
		     {2, 0, {0, 1, 0, 0},
		      &updated_position_move_info::summary::ne,
		      ne_tests},
		     {0, 2, {0, 0, 1, 0},
		      &updated_position_move_info::summary::sw,
		      sw_tests},
		     {2, 2, {0, 0, 0, 1},
		      &updated_position_move_info::summary::se,
		      se_tests},

		     {1, 0, {1, 1, 0, 0},
		      &updated_position_move_info::summary::ne,
		     nullptr},
		     {1, 2, {0, 0, 1, 1},
		      &updated_position_move_info::summary::se,
		     nullptr},

		     {0, 1, {1, 0, 1, 0},
		      &updated_position_move_info::summary::sw,
		     nullptr},
		     {2, 1, {0, 1, 0, 1},
		      &updated_position_move_info::summary::se,
		     nullptr}
	};

	for (const auto &test:tests)
	{
		updated_position_move_info move_info
			{{1,1}, test.move_to_x, test.move_to_y};

		updated_position_move_info::summary s;

		move_info.where([&]
				(auto to)
				{
					++(s.*to);
				});

		if (s.ne != test.expected_result.ne ||
		    s.nw != test.expected_result.nw ||
		    s.se != test.expected_result.se ||
		    s.sw != test.expected_result.sw)
		{
			std::cout << "where() failed" << std::endl;
			exit(1);
		}

		s.set_chosen();

		if (test.strict_weak_order_tests)
		{
			auto [biggest_direction, comparator]=s.chosen;

			for (size_t i=0; i<3; ++i)
			{
				const auto &swo_test=
					test.strict_weak_order_tests[i];

				LIBCXX_NAMESPACE::w::updated_position_move_info
					mva, mvb;

				mva.move_to_x=swo_test.a.x;
				mva.move_to_y=swo_test.a.y;
				mvb.move_to_x=swo_test.b.x;
				mvb.move_to_y=swo_test.b.y;

				if (!comparator(mva, mvb) ||
				    comparator(mvb, mva) ||
				    comparator(mva, mva))
				{
					std::cout << "comparator failed"
						  << std::endl;
					exit(1);
				}
			}
		}
	}
}

void testupdatedposition()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 LIBCXX_NAMESPACE::w::gridlayoutmanager
				 layout=main_window->get_layoutmanager();
			 LIBCXX_NAMESPACE::w::gridfactory factory=
				 layout->append_row();

			 auto b=factory->create_button("Button");

			 DEBUG=&*b->elementObj::impl;
		 });

	main_window->set_window_title("Move");

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
	{
		LIBCXX_NAMESPACE::mpcobj<size_t>::lock l{button_draw_counter};

		l.wait_for(std::chrono::seconds(5), [&]{ return *l == 1;});

		if (*l != 1)
			throw EXCEPTION("Did not execute the initial draw");
	}

	{
		LIBCXX_NAMESPACE::w::gridlayoutmanager
			layout=main_window->get_layoutmanager();

		auto f=layout->insert_row(0);

		f->create_label("Ok")->show_all();
	}
	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	if (button_draw_counter.get() != 1)
		throw EXCEPTION("DREW BUTTON expected to happen only once.");
	if (flush_counter.get() != 2)
		throw EXCEPTION("Expected to flush only twice");
}

int main()
{
	sanity_check();
	try {
		testupdatedposition();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
