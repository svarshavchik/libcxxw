/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_fieldfwd_h
#define x_w_input_fieldfwd_h

#include <x/w/input_field_configfwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class input_fieldObj;

//! A text input field.

//! \code
//! #include <INSERT_LIBX_NAMESPACE/w/input_field.H>
//!
//! INSERT_LIBX_NAMESPACE::w::input_field f=factory->create_input_field({""}, {20});
//! \endcode
//!
//! This is a text input field, with a blinking cursor.
//!
//! \code
//! std::string contents=f->get();
//!
//! std::u32string contents=f->get_unicode();
//! \endcode
//!
//! get() returns the current contents of the input field, in UTF-8; and
//! get_unicode() returns the current contents as a unicode string.
//!
//! \code
//! f->set("Hello world");
//!
//! f->set_unicode(U"Hello world");
//! \endcode
//!
//! set() specifies the new contents of the input field, in UTF-8.
//! set_unicode() specifies the new contents as a unicode string.
//!
//! \note
//! Getting the contents of the input field immediately after setting it may
//! still retrieve the original contents. The input field, like all other
//! display elements, gets updated by a separate execution thread, which
//! might be busy with other things, before focusing its attention on
//! updating the input field's contents.
//!
//! \see factory

typedef ref<input_fieldObj> input_field;

//! A nullable pointer reference to an input_field.

//! \see input_field

typedef ptr<input_fieldObj> input_fieldptr;

//! A reference to a constant input_field object.

//! \see input_field

typedef const_ref<input_fieldObj> const_input_field;

//! A nullable pointer reference to a constant input_field object.

//! \see input_field

typedef const_ptr<input_fieldObj> const_input_fieldptr;

LIBCXXW_NAMESPACE_END

#endif
