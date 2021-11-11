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

input_lock::~input_lock()=default;

std::tuple<richtextiterator, richtextiterator> input_lock::pos()
{
	return internal_editor->pos();
}

void input_lock::set(const std::string_view &str, bool validated)
{
	set(unicode::iconvert::tou::convert(std::string{str},
					    unicode_locale_chset()).first,
	    validated);
}

void input_lock::set(const std::u32string_view &str, bool validated)
{
	internal_editor->set(str, validated);
}

void input_lock::set(ONLY IN_THREAD,
		     const std::string_view &str, bool validated)
{
	set(IN_THREAD,
	    unicode::iconvert::tou::convert(std::string{str},
					    unicode_locale_chset()).first,
	    validated);

}

void input_lock::set(ONLY IN_THREAD,
		     const std::u32string_view &str, bool validated)
{
	internal_editor->lock_and_set(IN_THREAD, str, validated);
}

const_input_lock::const_input_lock(const const_input_field &locked_input_field)
	: const_input_lock{&locked_input_field->impl->editor_element->impl}
{
}

const_input_lock::const_input_lock(
	const ref<editor_implObj> *impl
)
	: internal_richtext_impl_t::lock{(*impl)->text->impl},
	  internal_editor{*impl}
{
}

const_input_lock::~const_input_lock()=default;

size_t const_input_lock::size() const
{
	return internal_editor->size();
}

std::tuple<const_richtextiterator,
	   const_richtextiterator> const_input_lock::pos() const
{
	return internal_editor->pos();
}

std::string const_input_lock::get(
	const std::optional<bidi_format> &embedding
)
	const
{
	return unicode::iconvert::fromu::convert(get_unicode(embedding),
						 unicode_locale_chset()).first;
}

std::u32string const_input_lock::get_unicode(
	const std::optional<bidi_format> &embedding
) const
{
	return internal_editor->get(embedding);
}


LIBCXXW_NAMESPACE_END
