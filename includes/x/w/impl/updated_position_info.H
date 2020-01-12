/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_updated_position_info_h
#define x_w_impl_updated_position_info_h

#include <x/w/impl/updated_position_infofwd.H>
#include <x/w/rectangle.H>

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

		size_t nw{0};
		size_t ne{0};
		size_t sw{0};
		size_t se{0};

		static bool sort_for_nw(const updated_position_move_info &a,
					const updated_position_move_info &b)
		{
			return (a.move_to_x < b.move_to_x ||
				(a.move_to_x == b.move_to_x &&
				 a.move_to_y < b.move_to_y));
		}

		static bool sort_for_ne(const updated_position_move_info &a,
					const updated_position_move_info &b)
		{
			return (a.move_to_x > b.move_to_x ||
				(a.move_to_x == b.move_to_x &&
				 a.move_to_y < b.move_to_y));
		}

		static bool sort_for_sw(const updated_position_move_info &a,
					const updated_position_move_info &b)
		{
			return (a.move_to_x < b.move_to_x ||
				(a.move_to_x == b.move_to_x &&
				 a.move_to_y > b.move_to_y));
		}

		static bool sort_for_se(const updated_position_move_info &a,
					const updated_position_move_info &b)
		{
			return (a.move_to_x > b.move_to_x ||
				(a.move_to_x == b.move_to_x &&
				 a.move_to_y > b.move_to_y));
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
				  ({&summary::nw, summary::sort_for_nw},
				   {&summary::ne, summary::sort_for_ne}),
				  {&summary::sw, summary::sort_for_sw}),
				 {&summary::se, summary::sort_for_se});
		}
	};

	template<typename F> void where(F &&f) const
	{
		if (scroll_from.x > move_to_x)
		{
			if (scroll_from.y >= move_to_y)
				f( &summary::nw);
			if (scroll_from.y <= move_to_y)
				f( &summary::sw);
		}
		else if (scroll_from.x < move_to_x)
		{
			if (scroll_from.y >= move_to_y)
				f( &summary::ne);
			if (scroll_from.y <= move_to_y)
				f( &summary::se);
		}
		else
		{
			if (scroll_from.y > move_to_y)
			{
				f( &summary::nw);
				f( &summary::ne);
			}
			else if (scroll_from.y < move_to_y)
			{
				f(&summary::sw);
				f(&summary::se);
			}
		}
	}
};

LIBCXXW_NAMESPACE_END

#endif
