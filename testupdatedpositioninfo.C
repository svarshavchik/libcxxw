/*
** Copyright 2019-2021 Double Precision, Inc.
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

typedef LIBCXX_NAMESPACE::mpcobj<LIBCXX_NAMESPACE::w::rectarea> flush_counter_t;

static flush_counter_t flush_counter;

struct state_change_t {
	size_t count=0;
	LIBCXX_NAMESPACE::w::rectangle current_position;
	bool clear_all=false;
};

static LIBCXX_NAMESPACE::mpcobj<state_change_t> state_change;

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

#define DEBUG_FLUSH_REDRAWN_AREAS() do {				\
		if (!DEBUG)						\
			return;						\
		std::cout << "FLUSH: " << r << std::endl;		\
		flush_counter_t::lock l{flush_counter};			\
		l->push_back(r);					\
		l.notify_all();						\
	} while(0);

#define DEBUG_KEY_EVENT() return true
#define DEBUG_BUTTON_EVENT() return
#define DEBUG_POINTER_MOTION_EVENT() return

#define CLEAR_TO_COLOR_LOG() do {					\
		LIBCXX_NAMESPACE::mpcobj<state_change_t>::lock		\
			lock{state_change};				\
									\
		for (const auto &r:areas)				\
		{							\
			if (r.width==lock->current_position.width &&	\
			    r.height==lock->current_position.height)	\
			{						\
				std::cout << "*** CLEAR ALL: "		\
					  << r << std::endl;		\
				lock->clear_all=true;			\
			}						\
		}							\
	} while(0)

#include "element_impl.C"

#include "generic_window_handler.C"

static LIBCXX_NAMESPACE::mpobj<int> moved_count=0;

#define DEBUG_MOVE_LOG() do {					\
		if (e->objname() == "x::w::buttonObj::implObj")	\
			moved_count=moved_count.get()+1;	\
		std::cout << "MOVE " << e->objname()		\
			  << ": " << move_info.scroll_from	\
			  << " -> " << move_info.move_to_x	\
			  << ", " << move_info.move_to_y	\
			  << std::endl;				\
	} while (0)
#include "connection_threadrunelement.C"

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

				mva.scroll_from={0,0,1,1};
				mvb.scroll_from={0,0,1,1};
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

void wait_for_idle(const LIBCXX_NAMESPACE::w::main_window &mw)
{
	auto flag=LIBCXX_NAMESPACE::ref<close_flagObj>::create();

	mw->in_thread_idle([flag]
			   (ONLY IN_THREAD)
			   {
				   flag->close();
			   });

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag->flag};

	lock.wait([&]
		  {
			  return *lock;
		  });
	std::cout << "Done" << std::endl;
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

	// Install a state update, wait for the initial one.
	main_window->on_state_update
		([]
		 (ONLY IN_THREAD,
		  const auto &state,
		  const auto &)
		 {
			 LIBCXX_NAMESPACE::mpcobj<state_change_t>::lock
				 lock{state_change};

			 std::cout << "State: " << state << std::endl;
			 ++lock->count;
			 lock->current_position=state.current_position;
			 lock.notify_all();
		 });

	{
		LIBCXX_NAMESPACE::mpcobj<state_change_t>::lock
			lock{state_change};

		lock.wait_for(std::chrono::seconds(5),
			      [&]
			      {
				      return lock->count == 1;
			      });

		if (lock->count != 1)
			throw EXCEPTION("Did not get the initial state");
	}

	// Wait for things to settle down.

	wait_for_idle(main_window);

	// And we expect to be exactly one flush.

	{
		flush_counter_t::lock lock{flush_counter};

		bool flag=false;

		lock.wait_for(std::chrono::seconds(3),
			      [&]
			      {
				      auto rect=add(*lock, *lock);

				      if (rect.size() != 1 ||
					  rect[0].x != 0 ||
					  rect[0].y != 0)
				      {
					      throw EXCEPTION
						      ("Unexpected flush");
				      }
				      lock->clear();
				      return flag=true;
			      });

		if (!flag)
			throw EXCEPTION("Did not receive exactly one initial "
					"flush");

		lock->clear();

		// A brief delay.

		lock.wait_for(std::chrono::seconds(2),
			      []
			      {
				      return false;
			      });
	}

	// Insert "Ok", shifting the button down.

	{
		LIBCXX_NAMESPACE::w::gridlayoutmanager
			layout=main_window->get_layoutmanager();

		auto f=layout->insert_row(0);

		f->border("thin_0%").create_container
			([]
			 (const auto &c)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager glm=
					 c->get_layoutmanager();

				 auto f=glm->append_row();

				 f->create_label("Foo");
				 f=glm->append_row();
				 f->create_label("Bar");
			 },
			 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{})
			->show_all();
	}

	// Wait for the window to resize.

	{
		LIBCXX_NAMESPACE::mpcobj<state_change_t>::lock
			lock{state_change};

		lock.wait_for(std::chrono::seconds(5),
			      [&]
			      {
				      return lock->count == 2;
			      });

		if (lock->count != 2)
			throw EXCEPTION("Did not get the 1st resized state");
	}

	wait_for_idle(main_window);

	std::cout << "MOVE COUNT: " << moved_count.get()
		  << std::endl;

	if (moved_count.get() != 1)
		throw EXCEPTION("Expected button contents to be moved");
	{
		flush_counter_t::lock lock{flush_counter};

		auto rect=add(*lock, *lock);

		if (rect.size() != 1 ||
		    rect[0].x != 0 ||
		    rect[0].y != 0)
		{
			for (const auto &r:rect)
				std::cout << "NO (1): " << r << std::endl;
			throw EXCEPTION("Did not flush everything?");
		}

		lock->clear();

		// A brief delay.

		lock.wait_for(std::chrono::seconds(2),
			      []
			      {
				      return false;
			      });
	}

	{
		LIBCXX_NAMESPACE::w::gridlayoutmanager
			layout=main_window->get_layoutmanager();

		LIBCXX_NAMESPACE::w::container c=layout->get(0, 0);

		layout=c->get_layoutmanager();

		layout->remove_row(0);
	}

	{
		LIBCXX_NAMESPACE::mpcobj<state_change_t>::lock
			lock{state_change};

		lock.wait_for(std::chrono::seconds(5),
			      [&]
			      {
				      return lock->count == 3;
			      });

		if (lock->count != 3)
			throw EXCEPTION("Did not get the 2nd resized state");
	}

	wait_for_idle(main_window);

	std::cout << "MOVE COUNT: " << moved_count.get()
		  << std::endl;
	if (moved_count.get() != 2)
		throw EXCEPTION("Expected button contents to be moved");

	{
		flush_counter_t::lock lock{flush_counter};

		auto rect=add(*lock, *lock);

		if (rect.size() != 1 ||
		    rect[0].x != 0 /* ||
				      rect[0].y != 0

				      smart enough not to redraw the top
				      border
				   */
		    )
		{
			for (const auto &r:rect)
				std::cout << "NO(2): " << r << std::endl;
			throw EXCEPTION("Did not flush everything?");
		}

		lock->clear();

		// A brief delay.

		lock.wait_for(std::chrono::seconds(2),
			      []
			      {
				      return false;
			      });
	}

	if (button_draw_counter.get() != 1)
		throw EXCEPTION("DREW BUTTON expected to happen only once.");
	if (state_change.get().clear_all)
		throw EXCEPTION("Unexpected clear");
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
