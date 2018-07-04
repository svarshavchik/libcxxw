/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontcollection_impl.H"
#include "fonts/freetype.H"
#include "fonts/cached_font.H"
#include "screen.H"
#include "connection.H"
#include "x/w/impl/fonts/freetypefont.H"
#include "fonts/fontsortedlist.H"
#include "fonts/fontpattern.H"
#include "fonts/fontcharset.H"
#include <fontconfig/fontconfig.h>

LIBCXXW_NAMESPACE_START

fontcollectionObj::implObj::implObj(const font_id_t &font_idArg,
				    const fontconfig::sortedlist &sortedlistArg,
				    const const_screen &screenArg,
				    const const_freetype &ftArg)
	: font_id(font_idArg), sortedlist(sortedlistArg),
	  s(screenArg), ft(ftArg)
{
}

fontcollectionObj::implObj::~implObj()
{
}

void fontcollectionObj::implObj
::do_lookup(const function< bool(char32_t &) > &next,
	    const function< void(const freetypefont &) > &callback)
{
	std::map<size_t, cached_font>::iterator prev;

	char32_t c;
	char32_t prev_char=0;
	bool first=true;

	metadata_t::lock lock(metadata);

	for ( ; next(c); prev_char=c)
	{
		// Fast check. If consecutive characters have the same
		// unicode script value, and the first character's font
		// has an entry for the second character, use the same font.
		if (!first && unicode_script(c) == unicode_script(prev_char) &&
		    prev->second->charset->has(c))
			continue;

		// First, let's do a quick scan of fonts we have already
		// opened.

		decltype(lock->opened_fonts.begin()) b, e;

		for (b=lock->opened_fonts.begin(),
			     e=lock->opened_fonts.end(); b != e; ++b)
		{
			if (b->second->charset->has(c))
				break;
		}

		// All right, do it the hard way.

		if (b == e)
		{
			auto i=search_for_font
				(lock,[c]
				 (const fontconfig::pattern &p)
				 {
					 fontconfig::const_charsetptr cs;

					 if (!p->get_charset(FC_CHARSET, cs))
						 throw EXCEPTION("FC_CHARSET value is missing?");
					 return cs->has(c);
				 });

			// At this point, if we found a font for this character,
			// it's font #i. If not, i >= sizeof(opened_fonts)

			b=lock->opened_fonts.find(i);

			if (b == e) // Default to first opened font.
				b=lock->opened_fonts.begin();

			if (b == e)
			{
				// Open the first font

				search_for_font(lock,
						[]
						(const fontconfig::pattern &p)
						{
							return true;
						});
				b=lock->opened_fonts.begin();
			}

			if (b == e)
				throw EXCEPTION("Could not open any font!\n"
						"    (" << font_id.key << ")");
		}

		if (first || prev != b)
		{
			prev=b;
			callback(prev->second->f);
		}
		first=false;
	}
}

// We iterate over the underlying list of fonts, and invoke the callback
// function for every font that has not already been loaded. If the callback
// function gives the green light, the font gets loaded.

size_t fontcollectionObj::implObj
::do_search_for_font(metadata_t::lock &lock,
		     const function<search_for_font_callback_t> &f)
{
	size_t i=0;

	for (auto b=sortedlist->begin(),
		     e=sortedlist->end(); b != e; ++b, ++i)
	{
		if (lock->opened_fonts.find(i) !=
		    lock->opened_fonts.end())
			continue; // Already checked this one.

		auto pattern=*b;
		if (!f(pattern))
			continue;

		// Open the font and cache it.

		auto f=ft->font(s, font_id.depth, pattern);
		if (f.null())
			continue;

		// As long as the font was opened, cache it
		// here.

		lock->opened_fonts.insert(std::make_pair(i, f));
		break; // Found it.
	}
	return i;
}

fontconfig::const_charset fontcollectionObj::implObj::default_charset()
{
	auto iter=sortedlist->begin();
	if (iter==sortedlist->end())
		throw EXCEPTION("Unexpected empty font list");

	auto p=*iter;
	fontconfig::const_charsetptr cs;

	if (!p->get_charset(FC_CHARSET, cs))
		throw EXCEPTION("FC_CHARSET value is missing?");

	return cs;
}

// Add up the largest ascender+descender

dim_t fontcollectionObj::implObj::height()
{
	metadata_t::lock lock(metadata);
	lookup_default(lock);

	dim_t ascender=0, descender=0;

	for (const auto &f:lock->opened_fonts)
	{
		const auto &ft=*f.second->f;

		if (ascender < ft.ascender)
			ascender=ft.ascender;

		if (descender < ft.descender)
			descender=ft.descender;
	}
	return dim_t::truncate(ascender+descender);
}

// Add up the largest ascender

dim_t fontcollectionObj::implObj::ascender()
{
	metadata_t::lock lock(metadata);
	lookup_default(lock);

	dim_t ascender=0;

	for (const auto &f:lock->opened_fonts)
	{
		const auto &ft=*f.second->f;

		if (ascender < ft.ascender)
			ascender=ft.ascender;
	}
	return ascender;
}

// Add up the largest descender

dim_t fontcollectionObj::implObj::descender()
{
	metadata_t::lock lock(metadata);

	lookup_default(lock);

	dim_t descender=0;

	for (const auto &f:lock->opened_fonts)
	{
		const auto &ft=*f.second->f;

		if (descender < ft.descender)
			descender=ft.descender;
	}
	return descender;
}

dim_t fontcollectionObj::implObj::max_advance()
{
	metadata_t::lock lock(metadata);
	lookup_default(lock);

	dim_t advance=0;

	for (const auto &f:lock->opened_fonts)
	{
		advance=f.second->f->max_advance;
		break;
	}

	return advance;
}

dim_t fontcollectionObj::implObj::nominal_width()
{
	metadata_t::lock lock(metadata);
	lookup_default(lock);

	dim_t nominal_width=0;

	for (const auto &f:lock->opened_fonts)
	{
		nominal_width=f.second->f->nominal_width;
		break;
	}

	return nominal_width;
}

bool fontcollectionObj::implObj::fixed_width()
{
	metadata_t::lock lock(metadata);
	lookup_default(lock);

	bool flag=false;

	for (const auto &f:lock->opened_fonts)
	{
		flag=f.second->f->fixed_width;
		break;
	}

	return flag;
}

void fontcollectionObj::implObj::lookup_default(metadata_t::lock &lock)
{
	if (lock->opened_fonts.find(0) != lock->opened_fonts.end())
		return; // The best matching font is already open.

	search_for_font(lock,[]
			(const fontconfig::pattern &p)
			{
				return true;
			});
}

LIBCXXW_NAMESPACE_END
