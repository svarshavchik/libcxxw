/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "rectangle.H"
#include "messages.H"

#include <algorithm>
LIBCXXW_NAMESPACE_START

std::ostream &operator<<(std::ostream &o, const rectangle &r)
{
	return o << x::gettextmsg(_("x=%1%, y=%2%, width=%3%, height=%4%"),
				  r.x,
				  r.y,
				  r.width,
				  r.height);
}

///////////////////////////////////////////////////////////////////////////
//
// When the same logic is applied to rectangles' horizontal and vertical
// components, rect_op_orientation points to the relevant fields in the
// rectangle object, for the given orientation.

struct rect_op_orientation;

struct LIBCXX_INTERNAL rect_op_orientation {

	const rect_op_orientation *other;

	coord_t rectangle::*coord;
	dim_t rectangle::*size;
};

extern const struct rect_op_orientation horizontal_dim, vertical_dim;

const struct rect_op_orientation horizontal_dim LIBCXX_INTERNAL = {
	&vertical_dim,
	&rectangle::x,
	&rectangle::width,
};

const struct rect_op_orientation vertical_dim LIBCXX_INTERNAL = {
	&horizontal_dim,
	&rectangle::y,
	&rectangle::height,
};

/////////////////////////////////////////////////////////////////////////////
//
// Merge rectangles.


class rectangle_to_merge;

// A rectangle's edge is identified by x/y coordinates, and its size.

class LIBCXX_INTERNAL rectangle_edge {

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

	// strict weak ordering for a set of these rectangle_edges.

	bool operator<(const rectangle_edge &other) const
	{
		if (x < other.x)
			return true;
		if (other.x < x)
			return false;
		if (y < other.y)
			return true;
		if (other.y < y)
			return false;
		return size < other.size;
	}
};

// We create this for every rectangle in the original std::set, in order
// to find the adjoining rectangles.

class LIBCXX_INTERNAL rectangle_to_merge {

 public:

	// The original rectangle in the original std::set
	rectarea::iterator rect_iter;

	// Points to the edge object for the left or the top of this rectangle.
	std::set<rectangle_edge>::iterator prev;

	// Points to the edge object for the bottom or the right edge.

	std::set<rectangle_edge>::iterator next;

	rectangle_to_merge(rectarea::iterator rect_iter,
			   std::set<rectangle_edge>::iterator prev,
			   std::set<rectangle_edge>::iterator next)
		: rect_iter(rect_iter),
		prev(prev),
		next(next)
		{
		}
};

class LIBCXX_INTERNAL rect_merge_pass {

 public:

	const rect_op_orientation &orientation;
	rectarea &rectangles;

	std::vector<rectangle_to_merge> merge_list;
	std::set<rectangle_edge> edges;

	bool merged=false;

	rect_merge_pass(const rect_op_orientation &orientation,
			rectarea &rectangles)
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
	std::set<rectangle_edge>::iterator
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

	std::set<rectangle_edge>::iterator
		find_trailing_edge(const rectangle &rect)
	{
		auto cpy=rect;

		auto d=(dim_t::value_type)(cpy.*(orientation.other->size));

		cpy.*(orientation.other->coord) += d;

		return find_edge(cpy.x, cpy.y,
				 (rect.*(orientation.size)));
	}

	std::set<rectangle_edge>::iterator find_edge(coord_t x, coord_t y,
						     dim_t size)
	{
		return edges.insert(rectangle_edge(x, y, size)).first;
	}
};

void merge(rectarea &rectangles) LIBCXX_HIDDEN;

void merge(rectarea &rectangles)
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

/////////////////////////////////////////////////////////////////////////////
//
// The rectangles that are involved in slicing are sorted into a vector,
// ordered by the position, first, then size.

class LIBCXX_INTERNAL rect_sort_by_dim {

 public:
	const rect_op_orientation &orientation;

	rect_sort_by_dim(const rect_op_orientation &orientation)
		: orientation(orientation)
	{
	}

	bool operator()(const rectangle &a,
			const rectangle &b)
	{
		if (a.*(orientation.coord) < b.*(orientation.coord))
			return true;
		if (b.*(orientation.coord) < a.*(orientation.coord))
			return false;
		return a.*(orientation.size) < b.*(orientation.size);
	}
};

class LIBCXX_INTERNAL rect_slice_pass {

 public:

	const rect_op_orientation &orientation;
	std::vector<rectangle> &slicee;
	std::vector<rectangle> &slicer;

	rect_slice_pass(const rect_op_orientation &orientation,
			std::vector<rectangle> &slicee,
			std::vector<rectangle> &slicer)
		: orientation(orientation), slicee(slicee), slicer(slicer)
	{
		rect_sort_by_dim comparator{orientation};

		std::sort(slicee.begin(), slicee.end(), comparator);
		std::sort(slicer.begin(), slicer.end(), comparator);

		// Ballpark estimate:
		//
		// Each slicer rectangle can potentially slice each slicee
		// into three rectangles. One of those would replace the
		// original rectangle, that leaves two more.

		slicee.reserve(slicee.size() + slicer.size()*2);

		size_t n_original_slicees=slicee.size();

		auto slicer_b=slicer.begin();
		auto slicer_e=slicer.end();

		size_t n=0;
		while (n < n_original_slicees &&
		       slicer_b != slicer_e)
		{
			auto &current_slicee=slicee[n];

			auto slicee_coord=current_slicee.*(orientation.coord);
			auto slicee_size=current_slicee.*(orientation.size);

			auto slicer_coord=(*slicer_b).*(orientation.coord);
			auto slicer_size=(*slicer_b).*(orientation.size);

			// Keep in mind that both slicees and slicers
			// are sorted by coord, then size.

			if (slicer_coord < slicee_coord &&
			    (slicee_coord - slicer_coord) >=
			    (dim_t::value_type)slicer_size)
			{
				// Slicer can't possibly slice anything,
				// it's completely before the start of the
				// slicee rectangle.
				++slicer_b;
				continue;
			}

			std::set<coord_t> slice_me_at;

			for (auto p=slicer_b; p != slicer_e; ++p)
			{
				auto slicer_coord=(*p).*(orientation.coord);
				auto slicer_size=(*p).*(orientation.size);

				if (slicer_coord >= slicee_coord &&
				    (slicer_coord-slicee_coord)
				    >= (dim_t::value_type)slicee_size)
					// No remaining slicers can touch
					// this slicee.
					break;

				if (!current_slicee.overlaps(*p))
					continue;

				if (slicer_coord > slicee_coord &&
				    (slicer_coord-slicee_coord) <
				    (dim_t::value_type)slicee_size)
					slice_me_at.insert(slicer_coord);

				auto slicer2_coord = slicer_coord + slicer_size;

				if (slicer2_coord >
				    (coord_t::value_type)slicee_coord &&
				    (slicer2_coord-(coord_t::value_type)
				     slicee_coord) <
				    (dim_t::value_type)slicee_size)
					slice_me_at.insert((coord_squared_t
							    ::value_type
							    )slicer2_coord);

			}

			auto slicee_copy=current_slicee;

			auto &copy_coord=slicee_copy.*(orientation.coord);
			auto &copy_size=slicee_copy.*(orientation.size);

			std::for_each(slice_me_at.begin(),
				      slice_me_at.end(),
				      [&]
				      (coord_t pos)
				      {
					      dim_t n=(dim_squared_t
						       ::value_type)
						      (pos-copy_coord);
					      auto new_rect=slicee_copy;

					      new_rect.*(orientation.size)=n;
					      slicee.push_back(new_rect);

					      copy_coord=
						      (coord_squared_t
						       ::value_type)
						      (copy_coord+n);

					      copy_size -= n;
				      });
			slicee[n]=slicee_copy; // Remainder replaces orig.

			++n;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
//
// Have two rectangle sets slice each other. We nominally call one the slicee
// and the other one a slicer, but they are equivalent to each other.
//
// Each rectangle set is unrolled into a vector, then each set slices each
// other.

rectangle_slicer::rectangle_slicer(const rectarea &slicee,
				   const rectarea &slicer)
	: slicee_v(slicee.begin(), slicee.end()),
	  slicer_v(slicer.begin(), slicer.end())
{
}

rectangle_slicer::~rectangle_slicer()=default;

void rectangle_slicer::slice_slicee()
{
	rect_slice_pass(horizontal_dim, slicee_v, slicer_v);
	rect_slice_pass(vertical_dim, slicee_v, slicer_v);
}

void rectangle_slicer::slice_slicer()
{
	rect_slice_pass(horizontal_dim, slicer_v, slicee_v);
	rect_slice_pass(vertical_dim, slicer_v, slicee_v);
}

rectarea add(const rectarea &a,
	     const rectarea &b,
	     coord_t x_offset,
	     coord_t y_offset)
{
	rectangle_slicer slicer{a, b};

	slicer.slice_slicee();
	slicer.slice_slicer();

	for (auto &r:slicer.slicee_v)
	{
		r.x = (coord_squared_t::value_type)(r.x + x_offset);
		r.y = (coord_squared_t::value_type)(r.y + y_offset);
	}

	for (auto &r:slicer.slicer_v)
	{
		r.x = (coord_squared_t::value_type)(r.x + x_offset);
		r.y = (coord_squared_t::value_type)(r.y + y_offset);
	}

	rectarea result{slicer.slicee_v.begin(),
			slicer.slicee_v.end()
			};

	result.insert(slicer.slicer_v.begin(),
		      slicer.slicer_v.end());

	merge(result);

	return result;
}

rectarea intersect(const rectarea &a,
		   const rectarea &b,
		   coord_t x_offset,
		   coord_t y_offset)
{
	rectangle_slicer slicer{a, b};

	slicer.slice_slicee();
	slicer.slice_slicer();

	rectarea result{slicer.slicee_v.begin(),
			slicer.slicee_v.end()
			};

	rectarea other_result{
		slicer.slicer_v.begin(),
			slicer.slicer_v.end()};

	rectarea intersection;

	for (auto p=result.begin(); p != result.end(); )
	{
		auto q=p;

		++p;

		if (other_result.find(*q) != other_result.end())
		{
			rectangle c=*q;

			c.x = (coord_squared_t::value_type)(c.x + x_offset);
			c.y = (coord_squared_t::value_type)(c.y + y_offset);

			intersection.insert(c);
		}
	}
	merge(intersection);

	return intersection;
}

rectarea subtract(const rectarea &a,
		  const rectarea &b,
		  coord_t x_offset,
		  coord_t y_offset)
{
	rectangle_slicer slicer{a, b};

	slicer.slice_slicee();
	slicer.slice_slicer();

	rectarea result{slicer.slicee_v.begin(),
			slicer.slicee_v.end()
			};

	std::for_each(slicer.slicer_v.begin(),
		      slicer.slicer_v.end(),
		      [&]
		      (const rectangle &r)
		      {
			      result.erase(r);
		      });

	merge(result);

	if (x_offset == 0 && y_offset == 0)
		return result;

	rectarea subtraction;

	for (auto c:result)
	{
		c.x = (coord_squared_t::value_type)(c.x + x_offset);
		c.y = (coord_squared_t::value_type)(c.y + y_offset);

		subtraction.insert(c);
	}
	return subtraction;

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
