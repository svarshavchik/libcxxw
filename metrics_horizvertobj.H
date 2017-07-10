/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_horizvertobj_H
#define metrics_horizvertobj_H

#include "x/w/types.H"
#include "x/w/metrics/axis.H"
#include "x/w/alignment.H"
#include "connection_threadfwd.H"
#include "metrics_horizvertfwd.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! Horizontal and vertical axis:
	struct LIBCXX_HIDDEN horizvert_axi {

		//! The horizontal dimensions.
		axis horiz;

		//! The vertical dimensions.
		axis vert;

		//! Constructor

		horizvert_axi();

		//! Constructor
		horizvert_axi(const axis &horiz,
			      const axis &vert);
	};

	//! A display element's metrics.

	//! Specifies a display element's horizontal and vertical sizes.
	//!
	//! \note
	//! THIS ENTIRE OBJECT IS IN_THREAD_ONLY

	class LIBCXX_HIDDEN horizvertObj : public horizvert_axi,
					   virtual public obj {

		//! Minimum horizontal size override
		dim_t minimum_horiz_override=0;

		//! Minimum vertical size override
		dim_t minimum_vert_override=0;

		//! Original, unoverridden axis.

		horizvert_axi original_axis;
	public:
		/////////////////////////////////////////////////////////////
		//
		// NOTE: this entire object is IN_THREAD_ONLY

		virtual void horizvert_updated(IN_THREAD_ONLY)=0;

		//! Default constructor
		horizvertObj();

		//! Also can use the copy constructor
		horizvertObj(const horizvert_axi &c);

		//! Destructor

		~horizvertObj();

		//! Update the metrics.

		void set_element_metrics(IN_THREAD_ONLY,
					 const axis &new_horiz,
					 const axis &new_vert)
		{
			auto prev_horiz=horiz;
			auto prev_vert=vert;

			horiz=new_horiz;
			vert=new_vert;

			original_axis.horiz=new_horiz;
			original_axis.vert=new_vert;

			horiz.set_minimum(minimum_horiz_override);
			vert.set_minimum(minimum_vert_override);

			if (horiz != prev_horiz || vert != prev_vert)
			{
				horizvert_updated(IN_THREAD);
			}
		}

		//! Install new minimum overrides

		void set_minimum_override(IN_THREAD_ONLY,
					  dim_t horiz_override,
					  dim_t vert_override)
		{
			minimum_horiz_override=horiz_override;
			minimum_vert_override=vert_override;

			// Check if the metrics should be updated due to the
			// new override.

			set_element_metrics(IN_THREAD,
					    original_axis.horiz,
					    original_axis.vert);
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
