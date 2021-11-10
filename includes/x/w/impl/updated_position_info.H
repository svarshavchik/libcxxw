/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_updated_position_info_h
#define x_w_impl_updated_position_info_h

#include <x/w/impl/updated_position_infofwd.H>
#include <x/w/rectangle.H>
#include <x/number.H>

LIBCXXW_NAMESPACE_START

//! Extra parameter to process_updated_position()

struct updated_position_info {

	enum moved_how_t
		{
		 moved_with_contents,
		 moved_without_contents,
		};

	moved_how_t moved_how=moved_with_contents;
};

struct updated_position_move_info {
	rectangle scroll_from;
	coord_t move_to_x;
	coord_t move_to_y;

	struct summary {

		size_t n{0}, s{0}, w{0}, e{0};

		static bool sort_for_n(const updated_position_move_info &a,
				       const updated_position_move_info &b)
		{
			return a.move_to_y < b.move_to_y;
		}

		static bool sort_for_s(const updated_position_move_info &a,
				       const updated_position_move_info &b)
		{
			return a.move_to_y > b.move_to_y;
		}

		static bool sort_for_e(const updated_position_move_info &a,
				       const updated_position_move_info &b)
		{
			return a.move_to_x > b.move_to_x;
		}

		static bool sort_for_w(const updated_position_move_info &a,
				       const updated_position_move_info &b)
		{
			return a.move_to_x < b.move_to_x;
		}

		typedef std::tuple<
			size_t summary::*,
			bool (*)(const updated_position_move_info &,
				 const updated_position_move_info &)
			> chosen_t;

		chosen_t chosen;

		inline auto compare(chosen_t a,
				    chosen_t b) const
		{
			return (this->*(std::get<0>(a))) >
				(this->*(std::get<0>(b))) ? a:b;
		}


		inline void set_chosen()
		{
			chosen=compare
				(compare
				 (compare
				  ({&summary::n, summary::sort_for_n},
				   {&summary::s, summary::sort_for_s}),
				  {&summary::w, summary::sort_for_w}),
				 {&summary::e, summary::sort_for_e});
		}
	};

	template<typename F> void where(F &&f) const
	{
		if (scroll_from.x > move_to_x)
		{
			f( &summary::w );
		}

		if (scroll_from.x < move_to_x)
		{
			f( &summary::e);
		}

		if (scroll_from.y > move_to_y)
		{
			f( &summary::n );
		}

		if (scroll_from.y < move_to_y)
		{
			f( &summary::s);
		}
	}
};

LIBCXXW_NAMESPACE_END

#endif
