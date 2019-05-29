/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/peephole_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

peephole_appearance_properties::peephole_appearance_properties()

	: peephole_border{"peephole_border"},
	  left_padding{0},
	  right_padding{0},
	  top_padding{0},
	  bottom_padding{0},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
{
}

peephole_appearance_properties::~peephole_appearance_properties()=default;

peephole_appearanceObj::peephole_appearanceObj()=default;

peephole_appearanceObj::~peephole_appearanceObj()=default;

peephole_appearanceObj::peephole_appearanceObj
(const peephole_appearanceObj &o)
	: peephole_appearance_properties{o}
{
}

const_peephole_appearance peephole_appearanceObj
::do_modify(const function<void (const peephole_appearance &)> &closure) const
{
	auto copy=peephole_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct peephole_appearance_base_themeObj : virtual public obj {

	const const_peephole_appearance config=const_peephole_appearance::create();

};

#if 0
{
#endif
}

const_peephole_appearance peephole_appearance_base::theme()
{
	return singleton<peephole_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
