/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef final_move_order_h
#define final_move_order_h

#include "x/w/impl/updated_position_info.H"
#include "x/w/rectangle.H"

#include <stack>
#include <set>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

#ifndef FINAL_MOVE_ORDER_DEBUG
#define FINAL_MOVE_ORDER_DEBUG(x) do { } while(0)
#endif

/*

Final adjustments to the order of widgets that get moved by copying their
existing contents in the window_pixmap instead of redrawing them from
scratch.

We get here after move_updated_position_widgets() analyzed new widget
positions and:

1) Determined which is the dominant direction of movement: north, south,
east, or west.

2) Compiled the list of widgets that move in the given direction,
removing all others. They'll get redrawn in the usual way.

3) Sorted the remaining list of widgets by their destination position,
starting with the farthest-moved widget first. So, when moving widgets
northward the first one will be the one that winds up in the northermmost
position.

This is almost good enough, but there are edge cases where this order is
wrong. For example, the widgets are being moved northward:

Widget 1: a 10x10 widget from (30, 30) to (10, 10)

Widget 2: a 10x10 widget from (15, 15) to (40, 13)

Widget 1 gets sorted first because it ends up at the northern-most position,
but that overlaps with widget 1's current position, so it needs to be moved
first.

*/

// Implement a comparator that orders rectangles by one of their four sides

struct sort_rectangles_by {

	// Aggregate initialization: specify rectangle::left(), right(),
	// top() or bottom().

	coord_t (rectangle::* const side)() const;

	// Compare the rectangles by the respective side.
	bool operator()(const rectangle *a, const rectangle *b) const
	{
		auto ac=(a->*side)();
		auto bc=(b->*side)();

		if (ac < bc)
			return true;

		if (bc < ac)
			return false;

		// In case of a tie compare their pointers

		return std::less<const rectangle *>{}(a, b);
	}
};

// A multiset container of rectangles, ordered by one of their sides.

typedef std::multiset<const rectangle *,
		      sort_rectangles_by> sort_rectangles_by_set_t;

// Container used by final_move_order to shuffle widget order. This is
// a stack backed by a vector.

typedef std::stack<
	const updated_position_container_t::value_type *,
	std::vector<const updated_position_container_t::value_type *>
	> move_stack_t;

struct LIBCXX_HIDDEN final_move_order_private;

//! Compute the final order in which containers get moved

//! The constructor takes a reference to the updated_position_container_t,
//! the preliminary order of moving widgets.
//!
//! The constructor calculates \c moved_member which is a vector of pointers
//! to individual values in the updated_position_container_t, in their final
//! order of movement.

struct final_move_order_private {

private:

	// First, we keep four containers, ordering each rectangle
	// by one of its sides. add() and remove() adds and removes each
	// rectangle from all four of these sets.

	sort_rectangles_by_set_t left_s{
		sort_rectangles_by{&rectangle::left}
	};

	sort_rectangles_by_set_t right_s{
		sort_rectangles_by{&rectangle::right}
	};

	sort_rectangles_by_set_t top_s{
		sort_rectangles_by{&rectangle::top}
	};

	sort_rectangles_by_set_t bottom_s{
		sort_rectangles_by{&rectangle::bottom}
	};

	void add(const rectangle *ptr)
	{
		left_s.insert(ptr);
		right_s.insert(ptr);
		top_s.insert(ptr);
		bottom_s.insert(ptr);
	}

	void remove(const rectangle *ptr)
	{
		left_s.erase(ptr);
		right_s.erase(ptr);
		top_s.erase(ptr);
		bottom_s.erase(ptr);
	}

	// Stack of widgets awaiting movement
	//
	// Initialized from the container that gets passed into the constructor.
	// The initial widget at the top of the stack is the first widget
	// in the container.
	//
	// As each widget gets pushed and popped from this stack, it also
	// gets add()ed or remove()d, so the four set containers define the
	// boundaries of all widgets waiting to be moved.

	move_stack_t tomove;

	// By looking at the four set containers we can define the rectangle
	// that covers all widgets waiting tomove. Since each container
	// orders the widgets by one of the four boundaries we only need to
	// look at either the first or the last widget in that container to
	// know the farthest edge. For example: left_s orders all widgets
	// by their let side, so the first widget's left() is the leftmost
	// value, and the left() side of the rectangle.

	bool empty() const
	{
		return left_s.empty();
	}

	coord_t left() const
	{
		return (*left_s.begin())->left();
	}

	coord_t right() const
	{
		return (*--right_s.end())->right();
	}

	coord_t top() const
	{
		return (*top_s.begin())->top();
	}

	coord_t bottom() const
	{
		return (*--bottom_s.end())->bottom();
	}

	// Since the four sets track what's in tomove, we can easily determine
	// the combined boundaries of all widgets' rectangles tomove. This
	// checks if the passed-in rectangle overlaps it. It might not actually
	// overlap a widget, in the end. We'll check for that later. This is
	// a quick check. If the rectangle is outside the combined boundaries
	// it can't possible overlap any rectangle tomove.

	bool intersects_boundary(const rectangle &r)
	{
		if (empty())
			return false;

		if (r.left() >= right())
			return false;

		if (r.right() <= left())
			return false;

		if (r.top() >= bottom())
			return false;

		if (r.bottom() <= top())
			return false;

		return true;
	}

	// Push another widget into the tomove list, and add() its edges

	void push_tomove(const updated_position_container_t::value_type *p)
	{
		tomove.push(p);

		auto &[iterators,info] = *p;

		add(&info.scroll_from);
	}

	// Topmost widget tomove
	auto &top_tomove()
	{
		return tomove.top();
	}

	// Pop off the first widget tomove(), and remove() its sides
	void pop_tomove()
	{
		if (tomove.empty())
			throw EXCEPTION("Internal error: attempting to "
					"pop an empty rectangle move "
					"stack");

		auto &[iterators,info] = *tomove.top();

		remove(&info.scroll_from);

		tomove.pop();
	}

	// Micro-optimization. tomove() stack's initialize size is known,
	// and it'll never be bigger. Construct a vector that reserve()s the
	// size, and move-construct the tomove stack from it.

	struct reserve_stack_buffer :
		std::vector<const updated_position_container_t::value_type *
			    > {

		reserve_stack_buffer(const updated_position_container_t &v)
		{
			reserve(v.size());
		}
	};

	// Delegated constructor, constructs tomove.

	final_move_order_private(std::vector<
				 const updated_position_container_t::value_type
				 *> && buffer)
		: tomove{std::move(buffer)}
	{
	}

public:
	std::vector<const updated_position_container_t::value_type *> moved;

	final_move_order_private(const updated_position_container_t &v)
		: final_move_order_private{reserve_stack_buffer{v}}
	{
		// We expect to wind up with the same number of moved widgets,
		// so we can reserve it too. Then we'll populate the initial
		// tomove() stack. Since stuff gets popped off the sack in
		// reverse order of being pushed, we push the widgets tomove
		// in reverse order.

		moved.reserve(v.size());

		for (auto b=v.begin(), e=v.end(); b != e; )
		{
			auto &move=*--e;

			push_tomove(&move);
		}

		FINAL_MOVE_ORDER_DEBUG(
			({
				std::ostringstream o;

				if (empty())
					o << "Nothing to move\n";
				else
					o << "Total boundaries: l="
					  << left()
					  << ", r="
					  << right()
					  << ", t="
					  << top()
					  << ", b="
					  << bottom()
					  << "\n";

				o.str();
			}));

		// Process one widget at a time using the following overall
		// approach: look at each tomove widget. Determine if its
		// moved-to position overlaps with some other tomove widget.
		//
		// No: remove it from tomove, put it into the moved vector.
		//
		// Yes: attempt to move the overlapping widget first (using
		// the same criteria), then go back and try to move the
		// original widget.
		//
		// skipped is the stack that holds the skipped widgets. When
		// a (potential) overlap gets detected we'll start moving
		// the widgets from the tomove stack to the skipped stack.
		//
		// When the conflict gets resolved we move the widgets back
		// from the skipped stack back to the tomove stack, ending
		// up with the original widget at the beginning of the tomove
		// stack that had its destination position overlap, hopefully
		// not anymore.

		move_stack_t skipped;

		while (!empty())
		{
			// We don't know yet if we have to skip this widget,
			// and some more widgets, but we need to keep things
			// in order, so take the next widget we're about
			// tomove, then push it onto the skipped stack.

			auto next=top_tomove();
			pop_tomove();
			skipped.push(next);

			// Determine where this widget will get moved to

			auto &[iterators,info] = *next;

			auto moved_to=info.scroll_from;
			moved_to.x=info.move_to_x;
			moved_to.y=info.move_to_y;

			// Flag to indicate whether the widget should really
			// be skippped. Just because the widget
			// intersects_boundary(), the overall boundary of
			// remaining widgets tomove doesn't mean that it
			// actually conflicts, so we'll start looking/skipping
			// at widgets tomove, and set the flag if there's
			// truly a conflict.
			bool blocked=false;

			size_t skipped_counter=0;

			FINAL_MOVE_ORDER_DEBUG(
				({
					std::ostringstream o;

					o << "Attempting: "
					  << info.scroll_from << " => "
					  << moved_to
					  << "\n";

					o << "Remaining boundary: ";

					if (empty())
					{
						o << "empty\n";
					}
					else
					{
						o << "l="
						  << left()
						  << ", r="
						  << right()
						  << ", t="
						  << top()
						  << ", b="
						  << bottom()
						  << "\n";
					}
					o.str();
				})
			);

			// So, as long as the widget we were contemplating
			// to move has its move-to area intersects_boundary
			// of the combining widgets tomove, check the next
			// tomove widget, to determine if it does actually
			// overlap.

			while (intersects_boundary(moved_to))
			{
				if (empty())
					throw EXCEPTION("Internal error in "
							<< __FILE__ << " ("
							<< __LINE__ << ")");
				auto peek=top_tomove();

				auto &[iterators, info]=*peek;

				FINAL_MOVE_ORDER_DEBUG(
					"Next: "
					<< info.scroll_from << std::endl
				);

				if (info.scroll_from.overlaps(moved_to))
				{
					// Ok, we found the blocking widget.

					FINAL_MOVE_ORDER_DEBUG(
						"Overlapping rectangle\n"
					);
					blocked=true;
					break;
				}


				// Ok, now move this widget from tomove to
				// skipped. We keep doing this until the
				// original widgets tomove does not
				// intersects_boundary of the combined
				// remaining tomove widget, at which point
				// we know that it can't possibly overlap
				// with any of the remaining tomove widgets.

				pop_tomove();
				skipped.push(peek);
				++skipped_counter;

				FINAL_MOVE_ORDER_DEBUG(
					({
						std::ostringstream o;

						o << "Does not overlap.\n"
						  << "Total boundaries: ";

						if (empty())
						{
							o << "empty\n";
						}
						else
						{
							o << "l="
							  << left()
							  << ", r="
							  << right()
							  << ", t="
							  << top()
							  << ", b="
							  << bottom()
							  << "\n";
						}
						o.str();
					})
				);
			}

			// We found the blocking widget, it's the first one
			// in the topmove stack. We simply go back to
			// the beginning of the loop, and this will attempt
			// to move that widget, now.

			if (blocked)
				continue;

			// If we skipped some widgets in order to determine
			// and the original next tomove widget does not overlap
			// with anything. Note that skipped=0 means that
			// we knew right off the bat that it doesn't overlap.
			// Way at the beginning we proactively moved the
			// next widget to the skipped list, so we use a
			// do/while loop to move it back from the skipped
			// to the tomove list. This restores the stacks to
			// what they were at the beginning of the loop.

			do
			{
				push_tomove(skipped.top());
				skipped.pop();
			} while (skipped_counter--);

			FINAL_MOVE_ORDER_DEBUG("Move: "
					       << info.scroll_from << " => "
					       << moved_to
					       << "\n");

			// It was a long journey, but we now can officially
			// record the next widget to move, and then remove it
			// from the tomove list.
			moved.push_back(next);

			pop_tomove();

			// If there's something in the skipped stack that
			// means that the original tomove widget overlapped
			// with another one, and we just moved it, so we can
			// now move the skipped stack back to the tomove list,
			// and try again.

			while (!skipped.empty())
			{
				FINAL_MOVE_ORDER_DEBUG(
					({
						auto &[iterators,info] =
							*skipped.top();

						std::ostringstream o;

						o << "Reque: "
						  << info.scroll_from
						  << "\n";

						o.str();
					})
				);
				push_tomove(skipped.top());
				skipped.pop();
			}
		}

	}
};

#if 0
{
#endif
}

typedef final_move_order_private final_move_order;

LIBCXXW_NAMESPACE_END

#endif
