/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_metrics_horizvertobj_H
#define x_w_impl_metrics_horizvertobj_H

#include <x/w/types.H>
#include <x/w/metrics/axis.H>
#include <x/w/alignment.H>
#include <x/w/impl/connection_threadfwd.H>
#include <x/w/impl/metrics_horizvertfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

namespace metrics {

	//! Horizontal and vertical axis:
	struct horizvert_axi {

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

	//! A widget's metrics.

	//! Specifies a widget's horizontal and vertical sizes.
	//!
	//! \note
	//! THIS ENTIRE OBJECT IS ONLY IN_THREAD

	class horizvertObj : public horizvert_axi, virtual public obj {

		//! Minimum horizontal size override
		dim_t minimum_horiz_override_thread_only=0;

		//! Minimum vertical size override
		dim_t minimum_vert_override_thread_only=0;

		//! Original, unoverridden axis.

		horizvert_axi original_axis;
	public:
		THREAD_DATA_ONLY(minimum_horiz_override);
		THREAD_DATA_ONLY(minimum_vert_override);

		/////////////////////////////////////////////////////////////
		//
		// NOTE: this entire object is ONLY IN_THREAD

		virtual void horizvert_updated(ONLY IN_THREAD)=0;

		//! Default constructor
		horizvertObj() LIBCXX_HIDDEN;

		//! Also can use the copy constructor
		horizvertObj(const horizvert_axi &c) LIBCXX_HIDDEN;

		//! Destructor

		~horizvertObj();

		//! Update the metrics.

		void set_element_metrics(ONLY IN_THREAD,
					 const axis &new_horiz,
					 const axis &new_vert)
		{
			auto prev_horiz=horiz;
			auto prev_vert=vert;

			horiz=new_horiz;
			vert=new_vert;

			original_axis.horiz=new_horiz;
			original_axis.vert=new_vert;

			horiz.set_minimum(minimum_horiz_override(IN_THREAD));
			vert.set_minimum(minimum_vert_override(IN_THREAD));

			if (horiz != prev_horiz || vert != prev_vert)
			{
				horizvert_updated(IN_THREAD);
			}
		}

		//! Install new minimum overrides

		void set_minimum_override(ONLY IN_THREAD,
					  dim_t horiz_override,
					  dim_t vert_override)
		{
			minimum_horiz_override(IN_THREAD)=horiz_override;
			minimum_vert_override(IN_THREAD)=vert_override;

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
