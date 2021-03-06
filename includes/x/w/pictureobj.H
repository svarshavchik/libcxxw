/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pictureobj_h
#define x_w_pictureobj_h

#include <x/w/namespace.H>
#include <x/w/picturefwd.H>
#include <x/w/pictformatfwd.H>
#include <x/w/rectanglefwd.H>
#include <x/w/pixmapfwd.H>
#include <x/w/rgbfwd.H>
#include <x/w/types.H>
#include <x/obj.H>
#include <x/number.H>

#include <vector>
#include <set>
#include <cmath>
#include <limits>
#include <type_traits>

LIBCXXW_NAMESPACE_START

//! A picture object.

//! \see picture

class pictureObj : virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor

	pictureObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor

	~pictureObj();

	//! Composite RENDER operation

	void composite(const const_picture &src,
		       coord_t src_x,
		       coord_t src_y,
		       coord_t dst_x,
		       coord_t dst_y,
		       dim_t width,
		       dim_t height,
		       render_pict_op op=render_pict_op::op_src);

	//! Composire RENDER operation

	void composite(const const_picture &src,
		       coord_t src_x,
		       coord_t src_y,
		       const rectangle &rect,
		       render_pict_op op=render_pict_op::op_src);

	//! Composite operation

	void composite(const const_picture &src,
		       const const_picture &mask,
		       coord_t src_x,
		       coord_t src_y,
		       coord_t mask_x,
		       coord_t mask_y,
		       coord_t dst_x,
		       coord_t dst_y,
		       dim_t width,
		       dim_t height,
		       render_pict_op op=render_pict_op::op_src);

	//! Set repeat property
	void repeat(render_repeat value);

	//! Clip the picture with a pixmap.

	//! The constructor installs the clip mask, the destructor removes it.
	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::picture::base::clip_mask

	class clip_mask {
		//! My picture
		const picture clipped_picture;

		//! My clip mask
		const pixmap picture_clip_mask;
	public:
		clip_mask(const picture &clipped_picture,
			  const pixmap &picture_clip_mask,
			  coord_t clip_x_origin,
			  coord_t clip_y_origin);
		~clip_mask();

		clip_mask(const clip_mask &)=delete;
		clip_mask &operator=(const clip_mask &)=delete;
	};

	//! Rectangular clip mask

	class rectangular_clip_mask {

		//! My picture
		const picture clipped_picture;

	public:
		rectangular_clip_mask(const picture &clipped_picture,
				      const rectarea &rectangles,
				      coord_t x=0,
				      coord_t y=0);

		~rectangular_clip_mask();

		rectangular_clip_mask(const rectangular_clip_mask &)=delete;

		rectangular_clip_mask &operator=(const rectangular_clip_mask &)
		=delete;
	};
	//! SetPictureClipRectangles RENDER operation.
	void set_clip_rectangles(const rectarea &clipregion,
				 coord_t x=0,
				 coord_t y=0);

	//! Clear the clip area completely.
	void clear_clip();

	//! A fixed precision value.

	//! Wrapped for the FIXEDPREC type.

	struct fixedprec {

		//! The value.
		typedef int32_t value_t;

		value_t value;

		//! Default constructor
		inline constexpr fixedprec() noexcept;

		//! Constructor
		inline constexpr fixedprec(int16_t integer,
					   uint16_t fraction) noexcept;

		//! Constructor
		inline constexpr fixedprec(const coord_t &c) noexcept;

		struct double_constructor_helper;
		struct intprec_constructor_helper;

		//! Use SFINAE to select a constructor helper

		template<typename T,
			 bool T_is_int16=(std::numeric_limits<T>::is_integer &&
					  std::numeric_limits<T>::max() <=
					  std::numeric_limits<int16_t>::max()),
			bool T_is_double=(!std::numeric_limits<T>::is_exact &&
					  std::numeric_limits<T>::digits10 <=
					  std::numeric_limits<double>::digits10)
			> struct constructor_helper;

		//! Single argument constructor

		//! Accepts either an int16_t or a double value

		template<typename T,
			 typename=std::enable_if_t<std::numeric_limits<T>
						   ::is_specialized>>
		inline constexpr fixedprec(T int16_or_double_value) noexcept;

		//! Constructor

		//! Accept only an int16_t parameter, or smaller.

		inline constexpr fixedprec(const intprec_constructor_helper &v)
		noexcept;

		//! Constructor
		inline constexpr fixedprec(const double_constructor_helper &v)
		noexcept;

		//! Comparison operator
		inline constexpr int compare(const fixedprec &o) const;

		//! Comparison operator
		inline constexpr bool operator<(const fixedprec &o) const;

		//! Comparison operator

		inline constexpr bool operator<=(const fixedprec &o) const;

		//! Comparison operator

		inline constexpr bool operator>(const fixedprec &o) const;

		//! Comparison operator

		inline constexpr bool operator>=(const fixedprec &o) const;

		//! Comparison operator
		inline constexpr bool operator==(const fixedprec &o) const;

		//! Comparison operator
		inline constexpr bool operator!=(const fixedprec &o) const;

		//! Distance between two fixedprec.

		//! Basically, abs(a-b).

		inline constexpr uint32_t distance(const fixedprec &) const;

		//! Borrow number.H to safely truncate a value
		template<typename T>
		static auto inline constexpr truncate(T &&v);
	};

	//! Fixed point

	struct point {
		//! X coordinate
		fixedprec x;

		//! Y coordinate
		fixedprec y;

		//! Comparison operator
		constexpr int compare(const point &o) const noexcept;

		//! Comparison operator

		constexpr bool operator<(const point &o) const noexcept;

		//! Comparison operator

		constexpr bool operator==(const point &o) const noexcept;
	};

	//! Triangle
	struct triangle {

		//! First point
		struct point p1;

		//! Second point
		struct point p2;

		//! Third point
		struct point p3;

		//! Comparison operator

		constexpr int compare(const triangle &o) const;

		//! Comparison operator

		constexpr bool operator<(const triangle &o) const;
	};

	//! Fill rectangles
	void fill_rectangle(const rectangle &r,
			     const rgb &color,
			     render_pict_op op=render_pict_op::op_src);

	//! Fill rectangles
	void fill_rectangles(const rectangle *rectangles,
			     size_t n_rectangles,
			     const rgb &color,
			     render_pict_op op=render_pict_op::op_src);
	//! Fill triangles
	void fill_triangles(const std::set<triangle> &triangles,
			    const const_picture &src,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill triangles
	void fill_triangles(const std::set<triangle> &triangles,
			    const const_picture &src,
			    const const_pictformat &mask,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill a triangle strip.
	void fill_tri_strip(const std::vector<point> &tri_strip,
			    const const_picture &src,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0)
	{
		if (tri_strip.empty())
			return;

		fill_tri_strip(&tri_strip.at(0),
			       tri_strip.size(),
			       src,
			       op,
			       src_x,
			       src_y);
	}

	//! Fill a triangle strip.
	void fill_tri_strip(const std::vector<point> &tri_strip,
			    const const_picture &src,
			    const const_pictformat &mask,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0)
	{
		if (tri_strip.empty())
			return;

		fill_tri_strip(&tri_strip.at(0),
			       tri_strip.size(),
			       src,
			       mask,
			       op,
			       src_x,
			       src_y);
	}

	//! Fill a triangle strip.
	void fill_tri_strip(const point *points,
			    size_t n_points,
			    const const_picture &src,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill a triangle strip.
	void fill_tri_strip(const point *points,
			    size_t n_points,
			    const const_picture &src,
			    const const_pictformat &mask,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0);

	//! Fill a triangle fan.
	void fill_tri_fan(const std::vector<point> &tri_strip,
			    const const_picture &src,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0)
	{
		if (tri_strip.empty())
			return;

		fill_tri_fan(&tri_strip.at(0),
			     tri_strip.size(),
			     src,
			     op,
			     src_x,
			     src_y);
	}

	//! Fill a triangle fan.
	void fill_tri_fan(const std::vector<point> &tri_strip,
			    const const_picture &src,
			    const const_pictformat &mask,
			    render_pict_op op=render_pict_op::op_src,
			    coord_t src_x=0,
			    coord_t src_y=0)
	{
		if (tri_strip.empty())
			return;

		fill_tri_fan(&tri_strip.at(0),
			     tri_strip.size(),
			     src,
			     mask,
			     op,
			     src_x,
			     src_y);
	}

	//! Fill a triangle fan.
	void fill_tri_fan(const point *points,
			  size_t n_points,
			  const const_picture &src,
			  render_pict_op op=render_pict_op::op_src,
			  coord_t src_x=0,
			  coord_t src_y=0);

	//! Fill a triangle fan.
	void fill_tri_fan(const point *points,
			  size_t n_points,
			  const const_picture &src,
			  const const_pictformat &mask,
			  render_pict_op op=render_pict_op::op_src,
			  coord_t src_x=0,
			  coord_t src_y=0);
};

LIBCXXW_NAMESPACE_END

#endif
