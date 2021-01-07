/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "font_picker_preview_impl.H"
#include "peephole/peepholed_fontelement.H"

LIBCXXW_NAMESPACE_START

font_picker_previewObj::font_picker_previewObj(const ref<implObj> &impl)
	: superclass_t{impl, impl, impl}, impl{impl}
{
}

font_picker_previewObj::~font_picker_previewObj()=default;

void font_picker_previewObj::update_preview(ONLY IN_THREAD,
					    const font &updated_font)
{
	impl->update_preview(IN_THREAD, updated_font);
}

void font_picker_previewObj::update_preview(ONLY IN_THREAD,
					    const font_arg &updated_font)
{
	impl->update_preview(IN_THREAD, updated_font);
}

LIBCXXW_NAMESPACE_END
