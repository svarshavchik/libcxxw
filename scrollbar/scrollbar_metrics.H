/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_metrics_h
#define scrollbar_metrics_h

#include "x/w/namespace.H"
#include "x/w/scrollbarfwd.H"
#include "x/w/rectanglefwd.H"

LIBCXXW_NAMESPACE_START

struct scroll_squared_v_tag;

//! A larger data type used when calculating scrollbar values.
typedef number<uint64_t, scroll_squared_v_tag> scroll_squared_v_t;

//! Scrollbar metrics

//! The scrollbar consists of scroll-low and scroll-high buttons at each end,
//! and a sliding porition.
//!
//! The scrollbar's pixel_size is its actual size.
//!
//! The size of the scroll buttons gets subtracted from the pixel size to
//! compute the number of pixels for the sliding portion.
//!
//! The sliding portion has  a virtual size, a page size, and a computed
//! pixel size obtained by subtracting the button's size from the overall
//! scrolbar pixel size.
//!
//! The virtual size of the scrollbar defines the virtual range of values
//! that the scrollbar ranges across. The page size is the scrollbar's
//! virtual size. The virtual and the page size is 0 based. If the virtual
//! size is 100, and page size is 20, the scrollbar handle itself represents
//! 20 values, so the scrollbar's actual position ranges from 0 to 80 because
//! at position 80 the scrollbar represents positions 80-99.
//!
//! The scrollbar's actual visual size is its computed pixel size. The
//! scrollbar's virtual range and page size gets scaled to its pixel size.
//! This class handles the scaling and the calculations.
//!
//! If the scrollbar's pixel size is not big enough for its buttons, the
//! too_small flag is set.
//!
//! If the scrollbar's pixel size is big enough for its buttons, but not
//! big enough for the slider's handle, the no_slider flag is set.

class LIBCXX_HIDDEN scrollbar_metrics {

	scroll_v_t scroll_low_size=0;
	scroll_v_t scroll_high_size=0;
	scroll_v_t virtual_size=0;
	scroll_v_t page_size=0;
	scroll_v_t pixel_size=0;

 public:

	//! Constructor
	scrollbar_metrics();

	//! Destructor
	~scrollbar_metrics();

	//! Copy constructor

	scrollbar_metrics(const scrollbar_metrics &);

	//! Assignment operator

	scrollbar_metrics &operator=(const scrollbar_metrics &);

	//! Compute scrollbar metrics.

	//! Calculates the public class members.

	void calculate(scroll_v_t scroll_low_size,
		       scroll_v_t scroll_high_size,
		       scroll_v_t virtual_size,
		       scroll_v_t page_size,
		       scroll_v_t pixel_size,
		       scroll_v_t minimum_handlebar_pixel_size);

	// Computed by calculate():
	bool too_small=true;
	bool no_slider=true;
	scroll_v_t handlebar_pixel_size=0;

	//! Given a value, compute the starting pixel position for the handlebar

	//! When the given value is 0, 0 gets returned.
	//!
	//! The maximum returned value is pixel_size-scroll_low_size-
	//! scroll_high_size-handlebar_pixel_size.
	//!
	//! Subtracting the scroll low/high sizes from pixel_size gives the
	//! size of the sliding portion of the scrollbar, that's
	//! handle_pixel_size in pixels, so the maximum starting position
	//! for the handlebar is that minus handlebar_pixel_size.

	scroll_v_t value_to_pixel(scroll_v_t) const;

	struct pixel_info {

		bool lo=false, hi=false;

		scroll_v_t value=0;
	};

	//! Compute the value for a pixel position.

	//! The inverse of value_to_pixel().

        pixel_info pixel_to_value(scroll_v_t) const;

	//! Compute the region occupied by each scrollbar element.

	//! scroll_low, slider, and scroll_high should be initialized with
	//! the scroll-bar's width and height, and (x,y) set to 0.
	//!
	//! This returns with each rectangle set to the appropriate area of
	//! the scrollbar element.
	//!
	//! coordinate and size points to y and height for vertical scrollbars
	//! and x and width for horizontal scrollbars.

	void regions(rectangle &scroll_low,
		     rectangle &slider,
		     rectangle &scroll_high,
		     coord_t rectangle::*coordinate,
		     dim_t rectangle::*size) const;
};

LIBCXXW_NAMESPACE_END

#endif
