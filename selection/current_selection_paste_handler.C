/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "selection/current_selection_paste_handler.H"
#include "generic_window_handler.H"
#include "connection_thread.H"
#include "connection_info.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

current_selection_paste_handlerObj
::current_selection_paste_handlerObj(const std::vector<xcb_atom_t> &fallback,
				     const ptr<obj> &mcguffin)
	: fallback{fallback}, mcguffin{mcguffin}
{
}

current_selection_paste_handlerObj::~current_selection_paste_handlerObj()
=default;

bool current_selection_paste_handlerObj
::begin_converted_data(ONLY IN_THREAD,
		       xcb_atom_t type,
		       xcb_timestamp_t timestamp)
{
	if (type == IN_THREAD->info->atoms_info.string ||
	    type == IN_THREAD->info->atoms_info.text_plain_mime ||
	    type == IN_THREAD->info->atoms_info.text_plain_iso8859_mime)
	{
		unicode::iconvert::tou::end();
		unicode::iconvert::tou::begin(unicode::iso_8859_1);
	}
	else if (type == IN_THREAD->info->atoms_info.utf8_string ||
		 type == IN_THREAD->info->atoms_info.text_plain_utf8_mime)
	{
		unicode::iconvert::tou::end();
		unicode::iconvert::tou::begin(unicode::utf_8);
	}
	else
	{
		return false;
	}
	return true;
}

void current_selection_paste_handlerObj
::converted_data(ONLY IN_THREAD,
		 void *data,
		 size_t size,
		 generic_windowObj::handlerObj &me)
{
	unicode::iconvert::tou::operator()
		(reinterpret_cast<char *>(data), size);

	try {
		if (!text.empty())
			me.pasted_string(IN_THREAD, text);
	} REPORT_EXCEPTIONS(&me);
	text.clear();
}

int current_selection_paste_handlerObj
::converted(const char32_t *ptr, size_t cnt)
{
	LOG_FUNC_SCOPE(elementObj::implObj::logger);

	// This is called from C code. Shouldn't be any exceptions here,
	// but just in case...
	try {
		text.insert(text.end(), ptr, ptr+cnt);
	} CATCH_EXCEPTIONS;
	return 0;
}

void current_selection_paste_handlerObj
::end_converted_data(ONLY IN_THREAD,
		     generic_windowObj::handlerObj &me)
{
	unicode::iconvert::tou::end();
	try {
		if (!text.empty())
			me.pasted_string(IN_THREAD, text);
	} REPORT_EXCEPTIONS(&me);
	text.clear();
}

void current_selection_paste_handlerObj
::conversion_failed(ONLY IN_THREAD,
		    xcb_atom_t type,
		    generic_windowObj::handlerObj &me)
{
	LOG_FUNC_SCOPE(elementObj::implObj::logger);

	if (fallback.empty())
	{
		LOG_DEBUG("Conversion to "
			  << IN_THREAD->info->get_atom_name(type) << " failed");
		return;
	}

	xcb_atom_t next_type=fallback.front();

	LOG_DEBUG("Conversion to "
		  << IN_THREAD->info->get_atom_name(type) << " failed, trying "
		  << IN_THREAD->info->get_atom_name(next_type));

	fallback.erase(fallback.begin());

	if (!me.convert_selection(IN_THREAD,
				  me.clipboard_being_pasted(IN_THREAD),
				  IN_THREAD->info->atoms_info.cxxwpaste,
				  next_type,
				  me.clipboard_paste_timestamp(IN_THREAD)))
	{
		LOG_ERROR("Could not reschedule a selection conversion.");
		return;
	}

	me.conversion_handler(IN_THREAD)=ref{this};
}

LIBCXXW_NAMESPACE_END
