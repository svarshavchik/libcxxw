/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_current_selection_paste_handlerobj_h
#define x_w_current_selection_paste_handlerobj_h

#include "selection/current_selection_paste_handlerfwd.H"
#include "selection/current_selection_handlerobj.H"

#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see current_selection_paste_handler

class current_selection_paste_handlerObj
	: public current_selection_handlerObj,
	  public unicode::iconvert::tou {

	//! More types to try, if we fail.
	std::vector<xcb_atom_t> fallback;

	//! mcguffin we stashed away.
	const ptr<obj> mcguffin;

	//! Converted text buffer.
	std::u32string text;
public:
	//! Constructor
	current_selection_paste_handlerObj(const std::vector<xcb_atom_t>
					   &fallback={},
					   const ptr<obj> &mcguffin={});

	//! Destructor
	~current_selection_paste_handlerObj();

 private:

	//! Implement converted().
	int converted(const char32_t *ptr, size_t cnt) override;

	//! Implement begin_converted_data()
	bool begin_converted_data(ONLY IN_THREAD,
				  xcb_atom_t type,
				  xcb_timestamp_t timestamp) override;

	//! Implement converted_data()
	void converted_data(ONLY IN_THREAD,
			    void *data,
			    size_t size,
			    generic_windowObj::handlerObj &me) override;

	//! Implement end_converted_data()
	void end_converted_data(ONLY IN_THREAD,
				generic_windowObj::handlerObj &me) override;

	//! Implemented conversion_failed()

	void conversion_failed(ONLY IN_THREAD,
			       xcb_atom_t type,
			       generic_windowObj::handlerObj &me) override;
};

LIBCXXW_NAMESPACE_END

#endif
