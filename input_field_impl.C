/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field.H"
#include "editor.H"
#include "peepholed_focusable.H"
#include "editor_impl.H"
#include "richtext/richtextobj.H"
#include "x/w/input_field.H"
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

input_lock::input_lock(const input_fieldObj &my_input_field)
	: internal_richtext_impl_t::lock(my_input_field.impl->editor_element
					 ->impl->text->impl),
	my_input_field(const_input_field{&my_input_field})
{
}

input_lock::~input_lock()=default;

size_t input_lock::size() const
{
	return my_input_field->impl->editor_element->impl->size();
}

std::tuple<size_t, size_t> input_lock::pos() const
{
	return my_input_field->impl->editor_element->impl->pos();
}

std::string input_lock::get() const
{
	return unicode::iconvert::fromu::convert(get_unicode(),
						 unicode::utf_8).first;
}

std::u32string input_lock::get_unicode() const
{
	return my_input_field->impl->editor_element->impl->get();
}


LIBCXXW_NAMESPACE_END
