/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "rectangle.H"
#include "messages.H"
#include <unordered_set>
#include <algorithm>
#include <utility>

LIBCXXW_NAMESPACE_START

std::ostream &operator<<(std::ostream &o, const rectangle &r)
{
	return o << x::gettextmsg(_("x=%1%, y=%2%, width=%3%, height=%4%"),
				  r.x,
				  r.y,
				  r.width,
				  r.height);
}

namespace {
#if 0
}
#endif

///////////////////////////////////////////////////////////////////////////
//
// When the same logic is applied to rectangles' horizontal and vertical
// components, rect_op_orientation points to the relevant fields in the
// rectangle object, for the given orientation.

struct rect_op_orientation;

struct rect_op_orientation {

	const rect_op_orientation *other;

	coord_t rectangle::*coord;
	dim_t rectangle::*size;
};

extern const struct rect_op_orientation horizontal_dim, vertical_dim;

const struct rect_op_orientation horizontal_dim = {
	&vertical_dim,
	&rectangle::x,
	&rectangle::width,
};

const struct rect_op_orientation vertical_dim = {
	&horizontal_dim,
	&rectangle::y,
	&rectangle::height,
};

// Take one set of rectangle, the "slicee".
//
// For either the X or the Y axis:
//
// Take all rectangles that are spanned by the same axis from any of the
// rectangles in the other set, and split that rectangle at that position.
//
// So, for example, when splitting on the X axis:
//
// Take all rectangles from the slicee: if any rectangle in the slicer has
// it's starting or the ending coordinate inside the slicee's rectangle,
// that slicee rectangle gets split at that position. Example:
//
//
//     +-----------+
//     |           |
//     |  slicer   |
//     |           |
//     +-----------+
//
// +-----------+
// |           |
// |  slicee   |
// |           |
// +-----------+
//
//      |
//     \|/
//      V
// +---+-------+
// |   |       |
// |   |       |
// |   |       |
// +---+-------+
//
// These two rectangles replace the original one.
//
// A single slicee rectangle may be split multiple times, by multiple slicer
// rectangles, and by either the left or the right edge of the same slicer
// rectangle.
//
// The Y axis slice is analogous/
//

static auto slice_by(const rectarea &slicee,
		     const rectarea &slicer,
		     const rect_op_orientation &axis)
{
	// Take each slicer's starting and ending coordinate, and put it into
	// e.

	std::vector<coord_t> e;

	e.reserve((slicer.size()+slicee.size())*2);

	for (const auto &a:slicee)
	{
		auto c=a.*(axis.coord);
		e.push_back(c);
		e.push_back(coord_t::truncate(c+a.*(axis.size)));
	}

	for (const auto &a:slicer)
	{
		auto c=a.*(axis.coord);
		e.push_back(c);
		e.push_back(coord_t::truncate(c+a.*(axis.size)));
	}

	// Sort it. There can be duplicates, here.

	std::sort(e.begin(), e.end());

	return e;
}

static auto slice(const rectarea &slicee,
		  const rectarea &slicer,
		  const std::vector<coord_t> &e,
		  const rect_op_orientation &axis)
{
	// Now take the slicee, and sort it by its starting coordinate.

	std::vector<rectangle> slicee_sorted{slicee};

	std::sort(slicee_sorted.begin(),
		  slicee_sorted.end(),
		  [&]
		  (const auto &a, const auto &b)
		  {
			  return a.*(axis.coord) < b.*(axis.coord);
		  });

	// Start slicing the slicee, and put the result into ret.
	std::vector<rectangle> ret;

	ret.reserve(e.size()+slicee_sorted.size()); // Estimate.

	auto sb=slicee_sorted.begin(), se=slicee_sorted.end();

	// e is also in sorted order.
	auto eb=e.begin(), ee=e.end();

	while (sb != se)
	{
		// Advance until the slicing coordinate is at least on or
		// after the slicee's starting coordinate.
		while (eb != ee && *eb < (*sb).*(axis.coord))
			++eb;

		// Make a copy of the sliced rectangle, and compute its
		// ending coordinate.
		auto cpy=*sb++;

		coord_t c_end{coord_t::truncate(cpy.*(axis.coord)+
						cpy.*(axis.size))};

		// We now start at the current slicer coordinate, and keep
		// advancing until the slicer reaches the ending coordinate,
		// c+end.

		auto p=eb;

		while (p != ee && *p < c_end)
		{

			// The slicer can have duplicate coordinates, see above.
			if (*p == cpy.*(axis.coord))
			{
				++p;
				continue;
			}

			// This rectangle can be sliced. Compute the size of the
			// first slice.

			dim_t s{dim_t::truncate(*p - cpy.*(axis.coord))};
			cpy.*(axis.size)=s;

			ret.push_back(cpy);

			// Update the cpy to the new starting coordinate, and
			// recompute the remaining slice's size.
			cpy.*(axis.coord)=*p;
			cpy.*(axis.size)=dim_t::truncate(c_end-*p);
			++p;
		}

		ret.push_back(cpy);
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
// Merge rectangles.

class rectangle_to_merge;

// A rectangle's edge is identified by x/y coordinates, and its size.

class rectangle_edge {

 public:

	coord_t x;
	coord_t y;
	dim_t size;

	rectangle_edge(coord_t x,
		       coord_t y,
		       dim_t size)
		: x(x), y(y), size(size)
	{
	}

	// This points to the rectangles that have this edge.
	mutable rectangle_to_merge *prev=nullptr;
	mutable rectangle_to_merge *next=nullptr;

	inline bool operator==(const rectangle_edge &o) const
        {
		return x == o.x && y == o.y && size == o.size;
	}
};

class rectangle_edge_hash {
public:
	inline size_t operator()(const rectangle_edge &e) const
        {
		return coord_t::value_type(e.x)+coord_t::value_type(e.y)+
			dim_t::value_type(e.size);
	}
};

typedef std::unordered_set<rectangle_edge,
			   rectangle_edge_hash> rectangle_edge_set;


// We create this for every rectangle in the original std::set, in order
// to find the adjoining rectangles.

class rectangle_to_merge {

 public:

	// The original rectangle in the original std::set
	rectangle_uset::iterator rect_iter;

	// Points to the edge object for the left or the top of this rectangle.
	rectangle_edge_set::iterator prev;

	// Points to the edge object for the bottom or the right edge.

	rectangle_edge_set::iterator next;

	rectangle_to_merge(rectangle_uset::iterator rect_iter,
			   rectangle_edge_set::iterator prev,
			   rectangle_edge_set::iterator next)
		: rect_iter(rect_iter),
		prev(prev),
		next(next)
		{
		}
};

class rect_merge_pass {

 public:

	const rect_op_orientation &orientation;
	rectangle_uset &rectangles;

	std::vector<rectangle_to_merge> merge_list;
	rectangle_edge_set edges;

	bool merged=false;

	rect_merge_pass(const rect_op_orientation &orientation,
			rectangle_uset &rectangles)
		: orientation(orientation), rectangles(rectangles)
	{
		merge_list.reserve(rectangles.size());

		// Make a pass over the rectangles.

		// Initialize merge_list, by copying its contents from the
		// rectangles set. At the same time populate the
		// edges set.

		for (auto p=rectangles.begin(), q=p; p != rectangles.end(); p=q)
		{
			q=p;
			++q;

			// Get rid of rectangles that have no width or depth.
			if (p->width == 0 || p->height == 0)
			{
				rectangles.erase(p);
				continue;
			}

			auto prev=find_leading_edge(*p);
			auto next=find_trailing_edge(*p);

			if (prev->next || next->prev)
				// Already seen this edge?
				throw EXCEPTION("Internal error: overlapping rectangles");

			merge_list.emplace_back(p, prev, next);

			auto r=merge_list.end();
			--r;

			prev->next=&*r;
			next->prev=&*r;
		}

		// Now, look for the edges

		for (auto &edge:edges)
		{
			if (!edge.prev || !edge.next)
				continue;

			// This is the rectangle before the shared edge
			auto &prev_rectangle_to_merge=*edge.prev;
			// This is the rectangle after the shared edge.
			auto &next_rectangle_to_merge=*edge.next;

			// Make a copy of the prev_rectangle_to_merge
			auto new_rectangle=*prev_rectangle_to_merge.rect_iter;
			auto &other_rectangle=
				*next_rectangle_to_merge.rect_iter;

			// Combine the other orientation's dimension into
			// the previous rectangle.
			new_rectangle.*(orientation.other->size) +=
				(dim_t::value_type)
				(other_rectangle.*(orientation.other->size));

			// Remove the two rectangles from the set, and
			// insert the new rectangle.
			rectangles.erase(prev_rectangle_to_merge.rect_iter);
			rectangles.erase(next_rectangle_to_merge.rect_iter);

			auto new_iter=rectangles.insert(new_rectangle);

			if (!new_iter.second)
				throw EXCEPTION("Internal error: overlapping rectangles");

			// Recycle prev_rectangle_to_merge.
			prev_rectangle_to_merge.rect_iter=new_iter.first;

			// Point prev_rectangle_to_merge's trailing edge.
			// to next_rectangle_to_merge's trailing edge.
			prev_rectangle_to_merge.next=next_rectangle_to_merge.next;
			// And repoint the trailing edge back to the new
			// rectangle that now points to it.
			prev_rectangle_to_merge.next->prev=edge.prev;
			merged=true;
		}
	}

	operator bool() const { return merged; }

	// Locate the top or the left edge for this rectangle.
	//
	// same x & y coordinates as the rectangle, plus the orientation's
	// dimension.
	rectangle_edge_set::iterator
		find_leading_edge(const rectangle &rect)
	{
		return find_edge(rect.x, rect.y,
				 (rect.*(orientation.size)));
	}

	// Locate the rightmost or the bottom edge for this rectangle.
	//
	// We add to either the x or the y coordinate the dimension of the
	// rectangle's other orientation, to compute its size, then use
	// this orientation's dimension to look up the edge.

	rectangle_edge_set::iterator
		find_trailing_edge(const rectangle &rect)
	{
		auto cpy=rect;

		auto d=(dim_t::value_type)(cpy.*(orientation.other->size));

		cpy.*(orientation.other->coord) += d;

		return find_edge(cpy.x, cpy.y,
				 (rect.*(orientation.size)));
	}

	rectangle_edge_set::iterator find_edge(coord_t x, coord_t y,
						     dim_t size)
	{
		return edges.insert(rectangle_edge(x, y, size)).first;
	}
};

#if 0
{
#endif
}

void merge(rectangle_uset &rectangles)
{
	bool merged_horizontally;
	bool merged_vertically;

	do
	{
		merged_horizontally=rect_merge_pass(horizontal_dim,
						    rectangles);
		merged_vertically=rect_merge_pass(vertical_dim,
						  rectangles);
	} while (merged_horizontally || merged_vertically);
}


// Take two rectangle sets that will be operated on, and have each one slice
// each other, by both X and Y coordinates.

rectangle_slicer::rectangle_slicer(const rectarea &a,
				   const rectarea &b)
	: rectangle_slicer{a, b,
			   slice_by(a, b, horizontal_dim),
			   slice_by(a, b, vertical_dim)}
{
}

rectangle_slicer::rectangle_slicer(const rectarea &a,
				   const rectarea &b,
				   const std::vector<coord_t> &h_slices,
				   const std::vector<coord_t> &v_slices)

	// Get things going with a horizontal slice.
	: first{slice(a, b, h_slices, horizontal_dim)},
	  second{slice(b, a, h_slices, horizontal_dim)}
{
	// Finish with a vertical slice.

	auto aa=slice(first, second, v_slices, vertical_dim);
	auto bb=slice(second, first, v_slices, vertical_dim);
	std::swap(first, aa);
	std::swap(second, bb);
}

////////////////////////////////////////////////////////////////////////////////

rectarea add(const rectarea &a,
	     const rectarea &b,
	     coord_t x_offset,
	     coord_t y_offset)
{
	rectangle_slicer slicer{a, b};

	rectangle_uset result{slicer.first.begin(),
			slicer.first.end()
			};

	result.insert(slicer.second.begin(),
		      slicer.second.end());

	merge(result);

	rectarea addition{result.begin(), result.end()};

	for (auto &c:addition)
	{
		c.x = coord_t::truncate(c.x + x_offset);
		c.y = coord_t::truncate(c.y + y_offset);
	}

	return addition;
}

rectarea intersect(const rectarea &a,
		   const rectangle &b,
		   coord_t x_offset,
		   coord_t y_offset)
{
	rectarea res;

	res.reserve(a.size());

	coord_t first_x2{coord_t::truncate(b.x+b.width)};
	coord_t first_y2{coord_t::truncate(b.y+b.height)};

	for (const auto &r:a)
	{
		coord_t x1{b.x > r.x ? b.x:r.x};
		coord_t y1{b.y > r.y ? b.y:r.y};

		coord_t second_x2{coord_t::truncate(r.x+r.width)};
		coord_t second_y2{coord_t::truncate(r.y+r.height)};

		coord_t x2{first_x2 < second_x2 ? first_x2:second_x2};
		coord_t y2{first_y2 < second_y2 ? first_y2:second_y2};

		if (x1 < x2 && y1 < y2)
		{
			res.push_back({coord_t::truncate(x1+x_offset),
				       coord_t::truncate(y1+y_offset),
				       dim_t::truncate(x2-x1),
				       dim_t::truncate(y2-y1)});
		}
	}
	return res;
}

rectarea intersect(const rectarea &a,
		   const rectarea &b,
		   coord_t x_offset,
		   coord_t y_offset)
{
	// Fall back to the fast path for the trivial case, if it wasn't
	// called directly.

	if (b.size() == 1)
		return intersect(a, *b.begin(), x_offset, y_offset);

	if (a.size() == 1)
		return intersect(b, *a.begin(), x_offset, y_offset);

	rectangle_slicer slicer{a, b};

	rectangle_uset result{slicer.first.begin(),
			      slicer.first.end()};

	rectangle_uset other_result;

	for (const auto &second:slicer.second)
	{
		auto iter=result.find(second);

		if (iter != result.end())
			other_result.insert(second);
	}

	merge(other_result);
	rectarea intersection{other_result.begin(), other_result.end()};

	for (auto &c:intersection)
	{
		c.x = coord_t::truncate(c.x + x_offset);
		c.y = coord_t::truncate(c.y + y_offset);
	}

	return intersection;
}

rectarea subtract(const rectarea &a,
		  const rectarea &b,
		  coord_t x_offset,
		  coord_t y_offset)
{
	rectangle_slicer slicer{a, b};

	rectangle_uset result{slicer.first.begin(),
			      slicer.first.end()};

	for (const auto &c:slicer.second)
		result.erase(c);

	merge(result);

	rectarea difference{result.begin(), result.end()};

	for (auto &c:difference)
	{
		c.x = coord_t::truncate(c.x + x_offset);
		c.y = coord_t::truncate(c.y + y_offset);
	}
	return difference;

}

rectangle bounds(const rectarea &s)
{
	if (s.empty())
		return {};

	auto b=s.begin(), e=s.end();

	coord_t x1=b->x, y1=b->y;
	auto x2=b->x+b->width, y2=b->y+b->height;

	while (++b != e)
	{
		if (b->x < x1)
			x1=b->x;

		if (b->y < y1)
			y1=b->y;

		auto x3=b->x+b->width, y3=b->y+b->height;

		if (x3 > x2)
			x2=x3;

		if (y3 > y2)
			y2=y3;
	}

	return {x1, y1, dim_t::truncate(x2-x1), dim_t::truncate(y2-y1)};
}

LIBCXXW_NAMESPACE_END
