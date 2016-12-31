/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef render_h
#define render_h

#include <map>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xcb_renderutil.h>
#include "returned_pointer.H"
#include "x/w/pictformatfwd.H"
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

// X Render extension metadata

// After the X connection is set up, we issue a xcb_render_query_pict_formats
// query, and store the reply, then parse the data into a more convenient
// format.

class LIBCXX_HIDDEN render {

public:
	// Construct this structure

	render(xcb_connection_t *conn);

	// The xcb_render_query_pict_formats_reply_t from the server.

	returned_pointer<xcb_render_query_pict_formats_reply_t *> render_pict_formats;

	// The reply is parsed into this lookup map. This map maps a
	// xcb_render_pictformat_t to our pictformat object.

	std::map<xcb_render_pictformat_t, const_pictformat>
		available_pictformats;

	// This is a vector of pictformats on each screen.

	// For each screen there's a default pictformat, and then a map
	// keyed by depth, with the value being a map of all visuals, for that
	// depth, to their corresponding pictformats.

	struct screen_depth_info {

		// Which pictformat goes with which visual

		std::multimap<xcb_visualid_t, const_pictformat>
		    visual_to_pictformat;
	};

	std::vector<std::map<depth_t, screen_depth_info>>
		pictformats_by_screen_depth;

	// Given a xcb_pict_standard_t, return the corresponding pictformat

	const_pictformat find_standard_format(xcb_pict_standard_t s) const;

	// Find an alpha pictformat for the given bit depth.

	const_pictformat find_alpha_pictformat_by_depth(depth_t d) const;
};

LIBCXXW_NAMESPACE_END

#endif
