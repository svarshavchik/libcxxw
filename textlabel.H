/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlabel_h
#define textlabel_h

#include "x/w/textlabelobj.H"
#include "x/w/elementobj.H"
#include "x/w/text_param.H"
#include "x/w/alignment.H"
#include "x/w/rectangle.H"
#include "x/w/metrics/axis.H"
#include "x/w/key_eventfwd.H"
#include "x/w/motion_eventfwd.H"
#include "x/w/button_eventfwd.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtextstring.H"
#include "richtext/richtextiteratorfwd.H"
#include "draw_infofwd.H"
#include "elementfwd.H"

#include <x/refptr_hash.H>
#include <unordered_map>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! Base class for a label implementation.

class LIBCXX_HIDDEN textlabelObj::implObj : virtual public obj {

	//! Word wrapping width, in millimeters.

	//! A width of 0 means don't word-wrap the text.
	double word_wrap_widthmm_thread_only;

 protected:
	//! Preferred width.

	//! This is word_wrap_widthmm converted to pixels.
	dim_t preferred_width=0;

	//! Compute preferred_width

	virtual void compute_preferred_width(IN_THREAD_ONLY);

 public:
	//! Info about each links.

	struct link_info {
		//! Where the link starts
		richtextiterator link_start;

		//! Where the link ends
		richtextiterator link_end;

		//! The hotspots get numbered, in order to create ordered_hotspots
		size_t n;
	};

	//! The container for the links in the text.
	typedef std::unordered_map<text_hotspot, link_info> hotspot_info_t;

 private:
	//! The links in the text.
	hotspot_info_t hotspot_info_thread_only;

	//! Currently highlighted link.
	text_hotspotptr hotspot_highlighted_thread_only;

	//! Container for hotspots kept in appearance order.
	typedef std::unordered_map<size_t, text_hotspot>
		ordered_hotspots_t;

	//! Hotspots in appearance order.
	ordered_hotspots_t ordered_hotspots;

 public:
	THREAD_DATA_ONLY(word_wrap_widthmm);
	THREAD_DATA_ONLY(hotspot_info);
	THREAD_DATA_ONLY(hotspot_highlighted);

	//! The text in the label.

	const richtext text;

	//! A cursor we use to determine whether the pointer is in a hotspot.

	const richtextiteratorptr hotspot_cursor;

	//! Default meta that was used to create the rich text

	const richtextmeta default_meta;

	//! Constructor for a label element.

	implObj(const text_param &text,
		halign alignment,
		double initial_width,
		bool allow_links,
		elementObj::implObj &element_impl);

	//! Internal constructor
	implObj(const text_param &text,
		const richtextmeta &default_meta,
		halign alignment,
		double initial_width,
		bool allow_links,
		elementObj::implObj &element_impl);

	//! Constructor used by editor subclass.

	implObj(halign alignment,
		double initial_width,
		richtextstring &&string,
		const richtextmeta &default_meta,
		bool allow_links);

 private:
	//! Internal constructor

	implObj(halign alignment,
		double initial_width,
		richtextstring &&string,
		const richtext &text,
		const richtextmeta &default_meta,
		bool allow_links);

 public:
	//! Whether links are allowed

	const bool allow_links=false;

	//! Return my element.
	virtual elementObj::implObj &get_label_element_impl()=0;

	//! Destructor
	~implObj();

	//! Replace text

	void update(const text_param &string);

	//! Replace text

	void update(IN_THREAD_ONLY, const text_param &string);

	//! Text has been updated
	void updated(IN_THREAD_ONLY);

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Calculate the metrics of the initial contents of this label.
	void initialize(IN_THREAD_ONLY);

	//! Invoke the underlying text object's theme_updated().

	//! When done, recalculate() our updated size.
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);

	//! Invoke the underlying text object's do_draw().

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas);

	//! Override process_updated_position()

	//! The label's position has changed.

	void process_updated_position(IN_THREAD_ONLY);

	//! The layout manager has set our position.

	//! Word-wrappable labels fix their metrics to their preferred
	//! width and height, until their layout manager positions it
	//! accordingly, then the fixed metrics get unfrozen, and the metrics
	//! then indicate the word-wrappability of the label.

	void position_set(IN_THREAD_ONLY);
 private:

	//! Whether the label's position has been set at least once.

	bool position_set_flag=false;

	//! Potentially re-wordwrap the label.

	//! After a label is resized we will need to re-wordwrap it, perhaps.
	//!
	//! Overriden in editor_impl.

	virtual void rewrap_due_to_updated_position(IN_THREAD_ONLY);

 protected:
	//! Recalculate and update our metrics.

	//! Invoke calculate_current_metrics(), then set_element_metrics().

	void recalculate(IN_THREAD_ONLY);

	//! Calculate current text metrics.

	//! Invokes text->get_metrics().
	//!
	//! Overridden in editor_impl to set the metrics strictly according
	//! to the preferred width.

	virtual std::pair<metrics::axis, metrics::axis>
		calculate_current_metrics(IN_THREAD_ONLY);


 public:
	//! Detect clicks over hotspots.

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp);

	//! Keyboard navigation over hotspots.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! Detect pointer movement over hotspots.
	void report_motion_event(IN_THREAD_ONLY, const motion_event &me);

	//! If no longer have pointer focus, disable any active hotspots.
	void pointer_focus(IN_THREAD_ONLY,
			   const callback_trigger_t &trigger);

 private:
	//! Unhighlight any currently-highlighted hotspots.
	void hotspot_unhighlight(IN_THREAD_ONLY);

	//! Invoke the link event, and possibly update the label accordingly.
	void link_update(IN_THREAD_ONLY, const text_hotspot &link,
			 const text_event_t &event_type);
};

LIBCXXW_NAMESPACE_END

#endif
