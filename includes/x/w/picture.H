/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_picture_h
#define x_w_picture_h

#include <x/w/picturefwd.H>
#include <x/w/pictureobj.H>
#include <x/ref.H>

#include <string_view>

LIBCXXW_NAMESPACE_START

//! Base class of \ref picture "pictures".

class pictureBase : public ptrref_base {

public:

	//! typedef alias

	typedef pictureObj::clip_mask clip_mask;

	//! typedef alias

	typedef pictureObj::rectangular_clip_mask rectangular_clip_mask;

	//! typedef alias

	typedef pictureObj::fixedprec fixedprec;

	//! typedef alias

	typedef pictureObj::point point;

	//! typedef alias

	typedef pictureObj::triangle triangle;

	//! Convert operation value to string

	static std::string render_pict_op_to_string(render_pict_op);

	//! Convert operation value from string

	static render_pict_op render_pict_op_from_string(const std::string_view &);

	//! Convert repeat value to string

	static std::string render_repeat_tostring(render_repeat);

	//! Convert repeat value from string

	static render_repeat render_repeat_from_string(const std::string_view &);

};

LIBCXXW_NAMESPACE_END

#endif
