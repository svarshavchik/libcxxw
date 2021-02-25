/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/input_field.H"
#include "editor.H"
#include "peepholed_focusable.H"
#include "editor_impl.H"
#include "x/w/impl/richtext/richtextobj.H"
#include "x/w/input_field.H"
#include "x/w/richtext/richtextiterator.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

input_fieldObj::implObj::implObj(const impl_mixin &impl,
				 const editor &editor_element)
	: impl(impl),
	  editor_element(editor_element)
{
}

input_fieldObj::implObj::~implObj()=default;




/////////////////////////////////////////////////////////////////////////

input_lock::input_lock(const input_field &locked_input_field)
	: const_input_lock{locked_input_field},
	  locked_input_field{locked_input_field}
{
}

input_lock::~input_lock()=default;

const_input_lock::const_input_lock(const const_input_field &locked_input_field)
	: internal_richtext_impl_t::lock
	{
	 locked_input_field->impl->editor_element
	 ->impl->text->impl
	},
	  locked_input_field{locked_input_field}
{
}

const_input_lock::~const_input_lock()=default;

size_t const_input_lock::size() const
{
	return locked_input_field->impl->editor_element->impl->size();
}

std::tuple<richtextiterator, richtextiterator> const_input_lock::pos() const
{
	return locked_input_field->impl->editor_element->impl->pos();
}

std::string const_input_lock::get(const std::optional<bidi_format> &embedding)
	const
{
	return unicode::iconvert::fromu::convert(get_unicode(embedding),
						 unicode_locale_chset()).first;
}

std::u32string const_input_lock::get_unicode(const std::optional<bidi_format>
					     &embedding) const
{
	return locked_input_field->impl->editor_element->impl->get(embedding);
}


LIBCXXW_NAMESPACE_END
