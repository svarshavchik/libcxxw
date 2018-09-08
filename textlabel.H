/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlabel_h
#define textlabel_h

#include "richtext/richtext_alteration_config.H"
#include "x/w/textlabelobj.H"
#include "x/w/elementobj.H"
#include "x/w/text_param.H"
#include "x/w/alignment.H"
#include "x/w/rectangle.H"
#include "x/w/metrics/axis.H"
#include "x/w/key_eventfwd.H"
#include "x/w/motion_eventfwd.H"
#include "x/w/button_eventfwd.H"
#include "x/w/fontcollectionfwd.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtextmeta.H"
#include "richtext/richtextstring.H"
#include "richtext/richtextiteratorfwd.H"
#include "x/w/impl/draw_infofwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/labelfwd.H"
#include <x/refptr_hash.H>
#include <unordered_map>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! Options for creating elements.

struct LIBCXX_HIDDEN textlabel_config {

	//! Public options.
        const label_config &config;

	//! child_elementObj initialization parameter.

	child_element_init_params child_element_init=
	default_child_element_init();

	static child_element_init_params default_child_element_init();

	//! Whether hotspot links are allowed (focusable labels only).
	bool allow_links=false;

	//! Fixed width, in columns.

	//! Set to non-0 by the editor element. This fixates the word wrap
	//! width, in columns.
	size_t width_in_columns=0;

	//! Fixed width metrics

	//! Set to true for the editor element. The width metrics of the
	//! label element are normally open-ended, allowing the label to
	//! be resizable. The editor element sets this, and we fix the
	//! width metrics.
	bool fixed_width_metrics=false;

	//! Whether ellipsis should be used if label doesn't fit.

	//! This is set of plain text label elements, in case they're used
	//! in an adjustable table. Because allow_shrinkage is false this
	//! usually has no effect.
	bool use_ellipsis=false;

	//! Whether labels metrics should specify no minimum width.

	//! Typically use_ellipsis is also set, and ellipsis get drawn.
	bool allow_shrinkage=false;
};

//! Base class for a label implementation.

class LIBCXX_HIDDEN textlabelObj::implObj
	: protected richtext_alteration_config,
	  virtual public obj {

 public:
	//! Word wrapping width, in millimeters.

	//! A width of 0 means don't word-wrap the text.
	const double word_wrap_widthmm;

 private:
	//! Preferred width.

	//! This is word_wrap_widthmm converted to pixels.
	dim_t preferred_width=0;

	//! Preferred width in columns

	//! \see textlabel_config
	const dim_t width_in_columns;

	//! Whether width metrics are fixed at the preferred width.

	//! \see textlabel_config
	const bool fixed_width_metrics;

	//! Whether the label's width can be shrinked

	//! \see textlabel_config
	const bool allow_shrinkage;

	//! Cached current theme

	defaulttheme current_theme;

	//! Compute preferred_width

	void compute_preferred_width(const defaulttheme &theme,
				     double widthmm,
				     const fontcollection &fc);

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
	THREAD_DATA_ONLY(hotspot_info);
	THREAD_DATA_ONLY(hotspot_highlighted);

	//! The text in the label.

	const richtext text;

	//! A cursor we use to determine whether the pointer is in a hotspot.

	const richtextiteratorptr hotspot_cursor;

	//! Default meta that was used to create the rich text

	const richtextmeta default_meta;


	//! Current minimum metrics override
	mpobj<dim_t> min_horiz_override=0;

	//! Current minimum vertical override
	mpobj<dim_t> min_vert_override=0;

	//! Constructor for a label element.

	implObj(const text_param &text,
		textlabel_config &config,
		elementObj::implObj &parent_element_impl);
 private:
	//! Internal constructor
	implObj(const text_param &text,
		current_theme_t::lock &&theme_lock,
		const richtextmeta &default_meta,
		textlabel_config &config,
		elementObj::implObj &parent_element_impl);

	//! Internal constructor
	implObj(textlabel_config &config,
		elementObj::implObj &parent_element_impl,
		const defaulttheme &initial_theme,
		richtextstring &&string,
		const richtextmeta &default_meta);

 public:
	//! Constructor used by editor subclass.

	implObj(textlabel_config &config,
		elementObj::implObj &parent_element_impl,
		const defaulttheme &initial_theme,
		richtextstring &&string);

 private:
	//! Internal constructor

	implObj(textlabel_config &config,
		elementObj::implObj &parent_element_impl,
		const defaulttheme &initial_theme,
		richtextstring &&string,
		const richtext &text,
		const richtextmeta &default_meta);

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

	void update(ONLY IN_THREAD, const text_param &string);

	//! Text has been updated
	void updated(ONLY IN_THREAD);

	////////////////////////////////////////////////////////////////////
	//
	// Inherited from elementObj::implObj

	//! Calculate the metrics of the initial contents of this label.
	void initialize(ONLY IN_THREAD);

	//! Invoke the underlying text object's theme_updated().

	//! When done, recalculate() our updated size.
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme);

	//! Invoke the underlying text object's do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas);

	//! Override process_updated_position()

	//! The label's position has changed.

	void process_updated_position(ONLY IN_THREAD);

	//! Recalculate based on new overrides

	void set_minimum_override(ONLY IN_THREAD,
				  dim_t horiz_override,
				  dim_t vert_override);

 private:

	//! Whether the label's position has been set at least once.

	bool position_set_flag=false;

	//! Potentially re-wordwrap the label.

	//! After a label is resized we will need to re-wordwrap it, perhaps.
	//!
	//! Overriden in editor_impl.

	virtual void rewrap_due_to_updated_position(ONLY IN_THREAD);

 protected:
	//! Recalculate and update our metrics.

	//! Invoke calculate_current_metrics(), then set_element_metrics().

	void recalculate(ONLY IN_THREAD);

	//! Calculate current text metrics.

	//! Invokes text->get_metrics().
	//!
	//! Overridden in editor_impl to set the metrics strictly according
	//! to the preferred width.

	virtual std::pair<metrics::axis, metrics::axis>
		calculate_current_metrics();


 public:
	//! Detect clicks over hotspots.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp);

	//! Keyboard navigation over hotspots.
	bool process_key_event(ONLY IN_THREAD, const key_event &ke);

	//! Detect pointer movement over hotspots.
	void report_motion_event(ONLY IN_THREAD, const motion_event &me);

	//! If no longer have pointer focus, disable any active hotspots.
	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger);

	//! Move to the first or the last hotspot
	void first_hotspot(ONLY IN_THREAD);

	//! Move to the first or the last hotspot
	void last_hotspot(ONLY IN_THREAD);

	//! Unhighlight any currently-highlighted hotspots.
	void hotspot_unhighlight(ONLY IN_THREAD);

 private:
	//! Invoke the link event, and possibly update the label accordingly.
	void link_update(ONLY IN_THREAD, const text_hotspot &link,
			 const text_event_t &event_type);
};

LIBCXXW_NAMESPACE_END

#endif
