#ifndef x_w_screen_depthinfo_h
#define x_w_screen_depthinfo_h

#include <x/w/screenobj.H>
#include <x/w/pictformat.H>
#include <x/vector.H>

LIBCXXW_NAMESPACE_START

//! Screen depth visual information

class screenObj::visualObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	visualObj(const ref<implObj> &impl,
		  const const_pictformat &render_format,
		  visual_class_t visual_class_type,
		  uint8_t bits_per_rgb,
		  uint16_t colormap_entries,
		  uint32_t red_mask,
		  uint32_t green_mask,
		  uint32_t blue_mask) LIBCXX_HIDDEN;

	//! Render format
	const const_pictformat render_format;

	//! Visual class type
	const visual_class_t visual_class_type;
	//! Bits per RGB value
	const uint8_t bits_per_rgb;
	//! Number of colormaps.
	const uint16_t colormap_entries;
	//! Red color mask. direct_color or true_color only.
	const uint32_t red_mask;
	//! Green color mask. direct_color or true_color only.
	const uint32_t green_mask;
	//! Blue color mask. direct_color or true_color only.
	const uint32_t blue_mask;

	//! Destructor
	~visualObj() noexcept;

	//! Return an indication whether a pictformat matches this visual's RGB components

	bool rgb_matches(const const_pictformat &format) const;
};

//! Screen depth information

class screenObj::depthObj : virtual public obj {

public:

	//! Constructor
	depthObj(depth_t depth,
		 std::vector<const_ref<visualObj>> &&visuals)
		LIBCXX_HIDDEN;

	//! Destructor
	~depthObj() noexcept LIBCXX_HIDDEN;

	//! Depth value
	depth_t depth;

	//! Available visuals

	//! Refer to the contents of this vector as
	//! \c INSERT_LIBX_NAMESPACE::w::screen::base::visual_t.

	std::vector<const_ref<visualObj>> visuals;
};

LIBCXXW_NAMESPACE_END

#endif
