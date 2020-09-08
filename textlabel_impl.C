/*
** Copyright 2017-2020 Double Precision, Inc.
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

static textlabelObj::implObj::hotspot_info_t
create_hotspot_info(richtextstring &&s, const richtext &t)
{
	s.render_order();

	textlabelObj::implObj::hotspot_info_t info;

	const auto &m=s.get_meta();

	auto b=m.begin(), e=m.end();

	size_t counter=0;

	while (b != e)
	{
		if (!b->second.link)
		{
			++b;
			continue;
		}

		auto p=b;

		while (b->second.link == p->second.link)
		{
			if (b->second.rl != p->second.rl)
				throw EXCEPTION(_("Cannot change text direction"
						  " in the middle of a hotspot"
						  ));
			if (++b == e)
				throw EXCEPTION("Internal error: cannot find end of link");
		}

		info.insert({p->second.link, {t->at(p->first,
						    new_location::lr),
					      t->at(b->first,
						    new_location::lr),
					      counter++}});
	}
	return info;
}

// After create_hotspot_info() comes rebuild_ordered_hotspots.

static auto rebuild_ordered_hotspots(const textlabelObj::implObj
				     ::hotspot_info_t &hotspot_info)
{
	std::unordered_map<size_t, text_hotspot> m;

	for (const auto &h:hotspot_info)
		m.insert({h.second.n, h.first});
	return m;
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
						char32_t unprintable_char)
{
	richtext_options options;

	options.alignment=config.config.alignment;
	options.unprintable_char=unprintable_char;
	return options;
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string,
			       const richtextmeta &default_meta)
	: implObj{config, parent_element_impl,
		  initial_theme,
		  std::move(string),
		  richtext::create(std::move(string),
				   create_richtext_options(config, '\0')),
		  default_meta}
{
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string)
	: implObj{config,
		  parent_element_impl,
		  initial_theme,
		  std::move(string),
		  richtext::create(std::move(string),
				   create_richtext_options(config, ' ')),
		  string.meta_at(0)}
{
}

textlabelObj::implObj::implObj(textlabel_config &config,
			       elementObj::implObj &parent_element_impl,
			       const const_defaulttheme &initial_theme,
			       richtextstring &&string,
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
	  hotspot_info_thread_only{create_hotspot_info(std::move(string),
						       text)},
	  ordered_hotspots{rebuild_ordered_hotspots(hotspot_info_thread_only)},
	  text{text},
	  hotspot_cursor{config.allow_links
			 ? (richtextiteratorptr)text->begin()
			 : richtextiteratorptr{}},
	  default_meta{default_meta},
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

	text->set(IN_THREAD, std::move(s));

	hotspot_info(IN_THREAD)=create_hotspot_info(std::move(s), text);
	ordered_hotspots=rebuild_ordered_hotspots(hotspot_info(IN_THREAD));
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
	if (hotspot_info(IN_THREAD).empty())
		return false;

	text_hotspotptr next_link;

	if (next_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			auto iter=hotspot_info(IN_THREAD)
				.find(hotspot_highlighted(IN_THREAD));

			if (iter == hotspot_info(IN_THREAD).end())
			{
				const auto &logger=elementObj::implObj::logger;
				LOG_ERROR("Internal error: cannot locate hotspot");
			}
			else
			{
				auto iter2=
					ordered_hotspots.find(iter->second.n+1);
				if (iter2 != ordered_hotspots.end())
					next_link=iter2->second;
			}
		}
		else
		{
			auto iter2=ordered_hotspots.find(0);
			if (iter2 != ordered_hotspots.end())
				next_link=iter2->second;
		}
	}
	else if (prev_key_pressed(ke))
	{
		if (hotspot_highlighted(IN_THREAD))
		{
			auto iter=hotspot_info(IN_THREAD)
				.find(hotspot_highlighted(IN_THREAD));

			if (iter == hotspot_info(IN_THREAD).end())
			{
				const auto &logger=elementObj::implObj::logger;
				LOG_ERROR("Internal error: cannot locate hotspot");
			}
			else if (iter->second.n)
			{
				auto iter2=
					ordered_hotspots.find(iter->second.n-1);
				if (iter2 != ordered_hotspots.end())
					next_link=iter2->second;
			}
		}
		else
		{
			auto iter2=ordered_hotspots
				.find(hotspot_info(IN_THREAD).size()-1);
			if (iter2 != ordered_hotspots.end())
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
	auto next_link=ordered_hotspots.find(0);

	if (next_link == ordered_hotspots.end())
		return;
	hotspot_unhighlight(IN_THREAD);

	hotspot_highlighted(IN_THREAD)=next_link->second;
	link_update(IN_THREAD, next_link->second, focus_change::gained);
}

void textlabelObj::implObj::last_hotspot(ONLY IN_THREAD)
{
	auto next_link=ordered_hotspots.find(hotspot_info(IN_THREAD).size()-1);
	if (next_link == ordered_hotspots.end())
		return;
	hotspot_unhighlight(IN_THREAD);

	hotspot_highlighted(IN_THREAD)=next_link->second;
	link_update(IN_THREAD, next_link->second, focus_change::gained);
}

void textlabelObj::implObj::report_motion_event(ONLY IN_THREAD,
						const motion_event &me)
{
	if (hotspot_info(IN_THREAD).empty() || !hotspot_cursor)
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

	auto iter=hotspot_info(IN_THREAD).find(link);

	if (iter == hotspot_info(IN_THREAD).end())
	{
		const auto &logger=e.logger;
		LOG_ERROR("Internal error: cannot locate hotspot");
		return;
	}

	replacement_text.hotspots.clear(); // Too bad, so sad.
	replacement_text.hotspots.insert({0, link});

	auto new_str=e.create_richtextstring
		(default_meta, replacement_text,
		 hotspot_processing::update);

	auto &new_meta=new_str.get_meta();
	auto mb=new_meta.begin(), me=new_meta.end();

	if (mb == me)
		throw EXCEPTION(_("Replacehoment hotspot string cannot be empty"
				  ));
	for (auto mp=mb; mp!=me; mp++)
		if (mp->second.rl != mb->second.rl)
			throw EXCEPTION(_("Cannot change text direction"
					  " in the middle of a hotspot"
					  ));

	iter->second.link_start->replace(IN_THREAD, iter->second.link_end,
					 std::move(new_str));
	updated(IN_THREAD);

	text->redraw_whatsneeded(IN_THREAD, e,
				 {*this},
				 e.get_draw_info(IN_THREAD));
}

LIBCXXW_NAMESPACE_END
