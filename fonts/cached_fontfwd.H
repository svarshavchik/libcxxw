/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef cached_fontfwd_H
#define cached_fontfwd_H

#include <x/ptrfwd.H>
#include <x/weakunordered_multimap.H>

#include "fonts/fontid_t.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN cached_fontObj;

//! A cached \ref freetypefont "freetypefont".

//! When a \ref fontcollection "fontcollection" loads an individual font,
//! the font gets cached in the connection object's
//! \ref font_cache_t "font cache".

typedef ref<cached_fontObj> cached_font;

//! A nullable pointer reference to a \ref cached_font "cached font".
typedef ptr<cached_fontObj> cached_fontptr;

//! A cache of \ref cached_font "cached fonts".

typedef weakunordered_multimap<font_id_t, cached_fontObj,
			       font_id_t_hash> font_cache_t;

LIBCXXW_NAMESPACE_END

#endif
