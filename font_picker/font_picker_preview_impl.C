/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "font_picker/font_picker_preview_impl.H"
#include "x/w/impl/theme_font_element.H"
#include "label_element.H"
#include "x/w/impl/always_visible_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/theme_font.H"
#include "defaulttheme.H"
#include "x/w/impl/fonts/current_fontcollection.H"
#include "x/w/impl/fonts/fontcollection.H"
#include "fonts/fontcharset.H"
#include "generic_window_handler.H"
#include "screen.H"
#include "defaulttheme.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

// Create a preview of the first 8 upper and lowercase characters.

static text_param create_preview(const font &f, const fontcollection &fc)
{
	auto cs=fc->default_charset();

	// Iterate over the first 16 pages in the fontcollection's charset.
	//
	// Stop when we reach 8 lower and uppercase letters.
	//
	// Also, as a backup plan, collect the first 16 characters seen,
	// and use them if we don't find enough letters.

	std::u32string lc, uc, any;

	lc.reserve(8);
	uc.reserve(8);
	any.reserve(16);

	size_t page_counter=0;
	cs->enumerate([&]
		      (const std::vector<char32_t> &chars)
		      {
			      for (auto c:chars)
			      {
				      if (lc.size() < 8 && unicode_islower(c))
					      lc.push_back(c);
				      if (uc.size() < 8 && unicode_isupper(c))
					      uc.push_back(c);
				      if (any.size() < 16)
					      any.push_back(c);
			      }

			      return (lc.size() < 8 || uc.size() < 8 ||
				      ++page_counter <= 8);
		      });

	// If we seen enough lowercase and uppercase letters, get rid of the
	// backup plan, and move them into the any buffer, interleaving them.

	if (lc.size() + uc.size() >= 8)
	{
		any.clear();

		auto ucb=uc.begin(), uce=uc.end();
		auto lcb=lc.begin(), lce=lc.end();

		// Insert a newline at the halfway mark.
		size_t half=(uc.size() + lc.size())/4;

		while (lcb != lce || ucb != uce)
		{
			if (ucb != uce)
				any.push_back(*ucb++);
			if (lcb != lce)
				any.push_back(*lcb++);

			if (--half == 0)
				any.push_back('\n');
		}
	}
	else	// Still insist on a newline, somewhere here:
		any=any.substr(0, any.size()/2) + U'\n' +
			any.substr(any.size()/2);
	return {f, any};
}

font_picker_previewObj::implObj::implObj(const container_impl &parent_container)
	: superclass_t{theme_font{"label"},
		parent_container,

			// create_font_picker() will manually call
			// compute_new_preview(), which will update us with the
			// initial preview font.
			text_param{" "},
			halign::center,
			0, false},
	  current_theme{parent_container->container_element_impl()
			  .get_screen()->impl->current_theme.get()}
{
}

font_picker_previewObj::implObj::~implObj()=default;

void font_picker_previewObj::implObj::update_preview(ONLY IN_THREAD,
						     const font &updated_font)
{
	current_font(IN_THREAD)=updated_font;

	refresh_preview(IN_THREAD);
}

void font_picker_previewObj::implObj::initialize(ONLY IN_THREAD)
{
	auto theme=get_screen()->impl->current_theme.get();

	if (theme == current_theme)
		return;

	current_theme=theme;
	refresh_preview(IN_THREAD);
}

void font_picker_previewObj::implObj::theme_updated(ONLY IN_THREAD,
						    const defaulttheme
						    &new_theme)
{
	current_theme=new_theme;
	refresh_preview(IN_THREAD);
}

void font_picker_previewObj::implObj::refresh_preview(ONLY IN_THREAD)
{
	auto &wh=get_window_handler();

	auto new_fc=current_fontcollectionObj::create_fc
		(current_font(IN_THREAD),
		 wh.get_screen(),
		 wh.font_alpha_depth(),
		 current_theme);

	current_fontcollection=new_fc;

	textlabelObj::implObj::update(IN_THREAD,
				      create_preview(current_font(IN_THREAD),
						     new_fc));
}

LIBCXXW_NAMESPACE_END
