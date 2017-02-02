/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef recycled_pixmapsobj_h
#define recycled_pixmapsobj_h

#include "recycled_pixmapsfwd.H"
#include "x/w/pictformatfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include <x/obj.H>
#include <x/weakunordered_multimapfwd.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A \ref recycled_pixmaps "recycled pixmap and picture container".

class LIBCXX_HIDDEN recycled_pixmapsObj : virtual public obj {

	//! Key used by scratch buffers.

	//! The key consists of an identifier and a pictformat.

	struct scratch_buffer_key {

		//! A label for a particular scratch buffer.
		std::string identifier;

		//! Scratch buffer's pictformat.
		pictformat pf;

		//! Equality comparison
		bool operator==(const scratch_buffer_key &) const;
	};


	//! A hasher for a \c scratch_buffer_key

	struct scratch_buffer_key_hash {

		//! Hash a \c scratch_buffer_key

		size_t operator()(const scratch_buffer_key &) const;
	};

public:
	//! Container for the scratch buffer cache.

	typedef weakunordered_multimap<scratch_buffer_key,
				       scratch_bufferObj,
				       scratch_buffer_key_hash>
		scratch_buffer_cache_t;

	//! The cache of scratch buffers.

	scratch_buffer_cache_t scratch_buffer_cache;

	//! Constructor

	recycled_pixmapsObj();

	//! Destructor

	~recycled_pixmapsObj();
};

LIBCXXW_NAMESPACE_END

#endif
