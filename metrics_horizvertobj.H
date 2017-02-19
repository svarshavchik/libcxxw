/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef metrics_horizvertobj_H
#define metrics_horizvertobj_H

#include "x/w/types.H"
#include "x/w/metrics/axis.H"
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

		//! Horizontal alignment
		halign horizontal_alignment;

		//! Vertical alignment
		valign vertical_alignment;

		//! Constructor

		horizvert_axi();

		//! Constructor
		horizvert_axi(const axis &horiz,
			      const axis &vert,
			      metrics::halign h,
			      metrics::valign v);
	};

	//! A display element's metrics.

	//! Specifies a display element's horizontal and vertical sizes.
	//!
	//! \note
	//! THIS ENTIRE OBJECT IS IN_THREAD_ONLY

	class LIBCXX_HIDDEN horizvertObj : public horizvert_axi,
					   virtual public obj {

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
			if (horiz != new_horiz || vert != new_vert)
			{
				horiz=new_horiz;
				vert=new_vert;
				horizvert_updated(IN_THREAD);
			}
		}

		//! Update the metrics.

		void set_element_metrics(IN_THREAD_ONLY,
					 halign new_alignment)
		{
			if (horizontal_alignment != new_alignment)
			{
				horizontal_alignment=new_alignment;
				horizvert_updated(IN_THREAD);
			}
		}

		//! Update the metrics.

		void set_element_metrics(IN_THREAD_ONLY,
					 valign new_alignment)
		{
			if (vertical_alignment != new_alignment)
			{
				vertical_alignment=new_alignment;
				horizvert_updated(IN_THREAD);
			}
		}
	};
}

LIBCXXW_NAMESPACE_END

#endif
