/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "textlabel.H"
#include "ellipsiscache.H"
#include "label_element.H"
#include "generic_window_handler.H"
#include "screen.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "richtext/richtext_draw_info.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/background_color.H"
#include "messages.H"
#include "defaulttheme.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/text_hotspot.H"
#include "x/w/key_event.H"
#include "x/w/motion_event.H"
#include "x/w/button_event.H"
#include "run_as.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

label factoryObj::create_label(const text_param &text)
{
	return create_label(text, {});
}

label factoryObj::create_label(const text_param &text,
			       const label_config &config)
{
	textlabel_config internal_config{config};

	internal_config.use_ellipsis=true;

#ifdef DEBUG_TRUNCATABLE_LABEL
	DEBUG_TRUNCATABLE_LABEL();
#endif
	auto label_impl=ref<label_elementObj<child_elementObj>>
		::create(get_container_impl(), text, internal_config);

	auto l=label::create(label_impl, label_impl);

	created(l);

	return l;
}

textlabelObj::implObj::implObj(const text_param &text,
			       textlabel_config &config,
			       elementObj::implObj &parent_element_impl)
	: implObj(text,
		  parent_element_impl.get_window_handler().get_screen()
		  ->impl->current_theme,
		  {parent_element_impl.create_background_color
		   (parent_element_impl.label_theme_color()),
		   parent_element_impl.create_current_fontcollection
		   (parent_element_impl.label_theme_font())},
		  config,
		  parent_element_impl)
{
}

// Compile a list of hotspots in the label.

static auto rebuild_ordered_hotspots(richtextstring &str)
{
	textlabelObj::implObj::ordered_hotspots_t s;

	size_t n=0;

	text_hotspotptr last_link;

	for (auto &m:str.get_meta())
	{
		if (m.second.link != last_link && m.second.link)
		{
			s.by_number.emplace(n, m.second.link);
			if (!s.by_hotspot.emplace(m.second.link, n).second)
				throw EXCEPTION("Internal error: multiple "
						"appearance by same hotspot");
			++n;
		}

		last_link=m.second.link;
	}
	return s;
}

textlabelObj::implObj::implObj(const text_param &text,
			       current_theme_t::lock &&theme_lock,
			       const richtextmeta &default_meta,
			       textlabel_config &config,
			       elementObj::implObj &parent_element_impl)
	: implObj{config,
		  parent_element_impl,
		  *theme_lock,
		  parent_element_impl.create_richtextstring
		  (default_meta, text, config.allow_links
		   ? hotspot_processing::create
		   : hotspot_processing::none),
		  default_meta}
{
}

static
inline richtext_options create_richtext_options(textlabel_config &config,
						char32_t unprintable_char,
						bool is_editor,
						bidi_format directional_format)
{
	richtext_options options;

	options.alignment=config.config.alignment;
	options.directional_format=directional_format;
	options.set_bidi(config.config.direction);
	options.unprintable_char=unprintable_char;
	options.is_editor=is_editor;
	return options;
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string,
			       const richtextmeta &default_meta)
	: implObj{config, parent_element_impl, initial_theme,
	std::move(string),
	rebuild_ordered_hotspots(string),
	default_meta}
{
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string,
			       ordered_hotspots_t &&ordered_hotspots,
			       const richtextmeta &default_meta)

	: implObj{config, parent_element_impl,
		  initial_theme,
		  std::move(string),
		  std::move(ordered_hotspots),
		  richtext::create(std::move(string),
				   create_richtext_options(config, '\0', 0,
							   bidi_format::standard
							   )),
		  default_meta}
{
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       bidi_format directional_format,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string)
	: implObj{config,
	directional_format,
	parent_element_impl,
	initial_theme,
	std::move(string),
	rebuild_ordered_hotspots(string)}
{
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       bidi_format directional_format,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string,
			       ordered_hotspots_t &&ordered_hotspots)
	: implObj{config,
		  parent_element_impl,
		  initial_theme,
		  std::move(string),
		  std::move(ordered_hotspots),
		  richtext::create(std::move(string),
				   create_richtext_options(config, ' ', true,
							   directional_format)),
		  string.meta_at(0)}
{
}

static inline auto clean_default_meta(const richtextmeta &meta)
{
	auto cleaned=meta;

	cleaned.link=nullptr;
	return cleaned;
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string,
			       ordered_hotspots_t &&ordered_hotspots,
			       const richtext &text,
			       const richtextmeta &default_meta)
	: richtext_alteration_config{config.use_ellipsis ? richtextptr
				     {
				      parent_element_impl.get_window_handler()
				      .get_screen()->impl->ellipsiscaches
				      ->get(parent_element_impl)}
				     : richtextptr{}},
	  word_wrap_widthmm{config.config.widthmm},
	  width_in_columns{config.width_in_columns},
	  fixed_width_metrics{config.fixed_width_metrics},
	  allow_shrinkage{config.allow_shrinkage},
	  current_theme{initial_theme},
	  ordered_hotspots_thread_only{std::move(ordered_hotspots)},
	  text{text},
	  hotspot_cursor{config.allow_links
			 ? (richtextiteratorptr)text->begin()
			 : richtextiteratorptr{}},
	  default_meta{clean_default_meta(default_meta)},
	  allow_links{config.allow_links}
{
	if (std::isnan(word_wrap_widthmm) ||
	    word_wrap_widthmm < 0)
		throw EXCEPTION(_("Invalid label width: ")
				<< config.config.widthmm);

	// We do what initialize() does here, based on the current theme.
	compute_preferred_width(initial_theme, config.config.widthmm,
				default_meta.getfont()->fc_public.get());
	text->rewrap(preferred_width);

	// We can now compute the initial metrics.
	auto metrics=calculate_current_metrics();

	config.child_element_init.initial_metrics.horiz=metrics.first;
	config.child_element_init.initial_metrics.vert=metrics.second;
}

textlabelObj::implObj::~implObj()=default;

void textlabelObj::implObj::update(const text_param &string)
{
	get_label_element_impl().THREAD->run_as
		([me=ref<implObj>(this), string]
		 (ONLY IN_THREAD)
		 {
			 me->update(IN_THREAD, string);
		 });
}

void textlabelObj::implObj::update(ONLY IN_THREAD, const text_param &string)
{
	get_label_element_impl().initialize_if_needed(IN_THREAD);
	auto s=get_label_element_impl()
		.create_richtextstring(default_meta, string,
				       allow_links
				       ? hotspot_processing::create
				       : hotspot_processing::none);

	auto new_ordered_hotspots=rebuild_ordered_hotspots(s);

	text->set(IN_THREAD, std::move(s));

	ordered_hotspots(IN_THREAD)=std::move(new_ordered_hotspots);
	hotspot_highlighted(IN_THREAD)=nullptr;
	updated(IN_THREAD);
	get_label_element_impl().schedule_full_redraw(IN_THREAD);
}

void textlabelObj::implObj::set_minimum_override(ONLY IN_THREAD,
						 dim_t horiz_override,
						 dim_t vert_override)
{
	min_horiz_override=horiz_override;
	min_vert_override=vert_override;
	recalculate(IN_THREAD);
}

void textlabelObj::implObj
::compute_preferred_width(const const_defaulttheme &theme,
			  double widthmm,
			  const fontcollection &fc)
{
	preferred_width=theme->compute_width(widthmm);


	if (width_in_columns > 0)
	{
		preferred_width=dim_t::truncate(width_in_columns *
						(dim_t::value_type)
						fc->nominal_width());
	}
}

void textlabelObj::implObj::initialize(ONLY IN_THREAD)
{
	auto screen=get_label_element_impl().get_screen()->impl;
	auto current_theme_now=screen->current_theme.get();

#ifdef DEBUG_INITIAL_METRICS
	auto hv=get_label_element_impl().get_horizvert(IN_THREAD);
	auto orig_horiz=hv->horiz;
	auto orig_vert=hv->vert;
#else
	if (current_theme == current_theme_now)
		return;
#endif
	current_theme=current_theme_now;

	text->theme_updated(IN_THREAD, current_theme);

	// Repeat what the constructor did.

	compute_preferred_width(current_theme_now, word_wrap_widthmm,
				default_meta.getfont()->fc(IN_THREAD));

	updated(IN_THREAD);

#ifdef DEBUG_INITIAL_METRICS

	if (hv->horiz != orig_horiz ||
	    hv->vert != orig_vert)
		throw EXCEPTION("Metrics have changed");
#endif
}

void textlabelObj::implObj::updated(ONLY IN_THREAD)
{
	rewrap_due_to_updated_position(IN_THREAD);

	// We can now compute and set our initial metrics.

	recalculate(IN_THREAD);
}

void textlabelObj::implObj::theme_updated(ONLY IN_THREAD,
					  const const_defaulttheme &new_theme)
{
	current_theme=new_theme;

	text->theme_updated(IN_THREAD, new_theme);
	compute_preferred_width(new_theme, word_wrap_widthmm,
				default_meta.getfont()->fc(IN_THREAD));
	updated(IN_THREAD);
}

void textlabelObj::implObj::process_updated_position(ONLY IN_THREAD)
{
	position_set_flag=true;
	rewrap_due_to_updated_position(IN_THREAD);
}

void textlabelObj::implObj::rewrap_due_to_updated_position(ONLY IN_THREAD)
{
	if (preferred_width == 0)
	{
		text->rewrap(0);
		recalculate(IN_THREAD);
		return; // Not word wrapping.
	}

	auto &element_impl=get_label_element_impl();
	element_impl.initialize_if_needed(IN_THREAD); // Just make sure

	auto rewrap_to=element_impl.data(IN_THREAD).current_position.width;

	if (rewrap_to == 0)
		rewrap_to=1;

	// Until our container places us we wish to maintain our initial
	// word-wrap width. Because the word-wrap width ultimately determines
	// the height of the label, which sets the label's vertical metrics,
	// we want to hold the initial word wrap width in order to maintain
	// the initial metrics until the container positions us. If, for
	// external reasons, the container sets our width to be something else
	// we are then free to start word-wrapping the label text to whatever
	// width the container sizes us to.

	if (!position_set_flag)
		rewrap_to=preferred_width;

	text->rewrap(rewrap_to);

	// And we should now update our metrics, accordingly.
	recalculate(IN_THREAD);
}

void textlabelObj::implObj::do_draw(ONLY IN_THREAD,
				    const draw_info &di,
				    const rectarea &areas)
{
#ifdef TEST_TEXTLABEL_DRAW
	TEST_TEXTLABEL_DRAW();
#endif
	text->full_redraw(IN_THREAD, get_label_element_impl(), {*this},
			  di, areas);
}

void textlabelObj::implObj::recalculate(ONLY IN_THREAD)
{
	auto metrics=calculate_current_metrics();

	get_label_element_impl()
		.get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 metrics.first,
		 metrics.second);
}

std::pair<metrics::axis, metrics::axis>
textlabelObj::implObj::calculate_current_metrics()
{
       auto ret=text->get_metrics(preferred_width);

       ret.first.set_minimum(min_horiz_override.get());

       if (fixed_width_metrics)
       {
	       auto w=ret.first.preferred();

	       ret.first={w, w, w};
       }
       else
       {
	       // If this is not a word-wrapping label, allow its minimum
	       // size to be reduced, and we'll show the ellipsis.

	       if (allow_shrinkage)
	       {
		       auto w=ellipsis->get_width();

		       if (w < ret.first.minimum())
		       {
			       ret.first=metrics::axis
				       {w, ret.first.preferred(),
					ret.first.maximum()};
		       }
	       }
       }
       return ret;
}

bool textlabelObj::implObj::process_button_event(ONLY IN_THREAD,
						 const button_event &be,
						 xcb_timestamp_t timestamp)
{
	if (!hotspot_highlighted(IN_THREAD))
		return false;
	link_update(IN_THREAD, hotspot_highlighted(IN_THREAD), &be);
	return true;
}

bool textlabelObj::implObj::process_key_event(ONLY IN_THREAD,
					      const key_event &ke)
{
	if (ordered_hotspots(IN_THREAD).by_hotspot.empty())
		return false;

	text_hotspotptr next_link;

	if (next_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			auto iter=ordered_hotspots(IN_THREAD).by_hotspot
				.find(hotspot_highlighted(IN_THREAD));

			if (iter==ordered_hotspots(IN_THREAD).by_hotspot.end())
			{
				const auto &logger=elementObj::implObj::logger;
				LOG_ERROR("Internal error: cannot locate hotspot");
			}
			else
			{
				auto iter2=ordered_hotspots(IN_THREAD).by_number
					.find(iter->second+1);
				if (iter2 != ordered_hotspots(IN_THREAD).by_number.end())
					next_link=iter2->second;
			}
		}
		else
		{
			auto iter2=ordered_hotspots(IN_THREAD).by_number.find(0);
			if (iter2 != ordered_hotspots(IN_THREAD)
			    .by_number.end())
				next_link=iter2->second;
		}
	}
	else if (prev_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			auto iter=ordered_hotspots(IN_THREAD).by_hotspot
				.find(hotspot_highlighted(IN_THREAD));

			if (iter==ordered_hotspots(IN_THREAD).by_hotspot.end())
			{
				const auto &logger=elementObj::implObj::logger;
				LOG_ERROR("Internal error: cannot locate hotspot");
			}
			else if (iter->second)
			{
				auto iter2=ordered_hotspots(IN_THREAD).by_number
					.find(iter->second-1);
				if (iter2 != ordered_hotspots(IN_THREAD)
				    .by_number.end())
					next_link=iter2->second;
			}
		}
		else
		{
			auto iter2=ordered_hotspots(IN_THREAD).by_number
				.find(ordered_hotspots(IN_THREAD).by_number
				      .size()-1);
			if (iter2 != ordered_hotspots(IN_THREAD)
			    .by_number.end())
				next_link=iter2->second;
		}
	}
	else if (select_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			link_update(IN_THREAD, hotspot_highlighted(IN_THREAD),
				    &ke);
			return true;
		}
	}
	else return false;

	hotspot_unhighlight(IN_THREAD);

	if (!next_link)
		// Do not process the key, let the generic_window_handler
		// claim the keypress, and move the input focus.
		return false;

	hotspot_highlighted(IN_THREAD)=next_link;
	link_update(IN_THREAD, next_link, focus_change::gained);
	return true;
}

void textlabelObj::implObj::first_hotspot(ONLY IN_THREAD)
{
	auto next_link=ordered_hotspots(IN_THREAD).by_number.find(0);

	if (next_link == ordered_hotspots(IN_THREAD).by_number.end())
		return;
	hotspot_unhighlight(IN_THREAD);

	hotspot_highlighted(IN_THREAD)=next_link->second;
	link_update(IN_THREAD, next_link->second, focus_change::gained);
}

void textlabelObj::implObj::last_hotspot(ONLY IN_THREAD)
{
	auto next_link=ordered_hotspots(IN_THREAD).by_number
		.find(ordered_hotspots(IN_THREAD).by_number.size()-1);
	if (next_link == ordered_hotspots(IN_THREAD).by_number.end())
		return;
	hotspot_unhighlight(IN_THREAD);

	hotspot_highlighted(IN_THREAD)=next_link->second;
	link_update(IN_THREAD, next_link->second, focus_change::gained);
}

void textlabelObj::implObj::report_motion_event(ONLY IN_THREAD,
						const motion_event &me)
{
	if (ordered_hotspots(IN_THREAD).by_number.empty() || !hotspot_cursor)
		return; // Shortcut

	bool flag=hotspot_cursor->moveto(IN_THREAD, me.x, me.y);

	auto link=hotspot_cursor->at(IN_THREAD).link;

	if (!link || !flag)
	{
		hotspot_unhighlight(IN_THREAD);
		return;
	}

	if (link == hotspot_highlighted(IN_THREAD))
		return;

	hotspot_unhighlight(IN_THREAD);
	hotspot_highlighted(IN_THREAD)=link;

	link_update(IN_THREAD, link, focus_change::gained);
}

void textlabelObj::implObj::pointer_focus(ONLY IN_THREAD,
					  const callback_trigger_t &trigger)
{
	if (!get_label_element_impl().current_pointer_focus(IN_THREAD))
		hotspot_unhighlight(IN_THREAD);
}

void textlabelObj::implObj::hotspot_unhighlight(ONLY IN_THREAD)
{
	if (!hotspot_highlighted(IN_THREAD))
		return;

	text_hotspot old_link=hotspot_highlighted(IN_THREAD);
	hotspot_highlighted(IN_THREAD)=nullptr;

	link_update(IN_THREAD, old_link, focus_change::lost);
}

void textlabelObj::implObj::link_update(ONLY IN_THREAD,
					const text_hotspot &link,
					const text_event_t &event_type)
{
	auto replacement_text=link->event(IN_THREAD, event_type);

	if (replacement_text.string.empty())
		return;

	// Hotspot callback provided replacement text.

	auto &e=get_label_element_impl();

	replacement_text.hotspots.clear(); // Too bad, so sad.

	auto new_str=e.create_richtextstring
		(default_meta, replacement_text,
		 hotspot_processing::update);

	if (new_str.get_string().empty())
		throw EXCEPTION(_("Replacement hotspot string cannot be empty")
				);

	text->replace_hotspot(IN_THREAD, new_str, link);

	updated(IN_THREAD);

	text->redraw_whatsneeded(IN_THREAD, e,
				 {*this},
				 e.get_draw_info(IN_THREAD));
}

LIBCXXW_NAMESPACE_END
