/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "icon_cache.H"
#include "sxg/sxg_parser.H"
#include "defaulttheme.H"
#include "screen.H"
#include "x/w/impl/pixmap_with_picture.H"
#include "drawable.H"
#include "x/w/impl/icon.H"
#include "themeiconobj.H"
#include "themeiconpixmapobj.H"
#include "pixmap_loader.H"
#include "x/w/picture.H"
#include "x/w/dim_arg_hash.H"
#include <x/refptr_hash.H>
#include <x/number_hash.H>
#include <x/weakunordered_multimap.H>
#include <x/functional.H>
#include <x/visitor.H>
#include <x/mmapfile.H>
#include <x/fd.H>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jpeglib.h>
#include <gif_lib.h>
#include <png.h>

LIBCXXW_NAMESPACE_START

typedef ref<icon_cacheObj::cached_filename_infoObj> cached_filename_info;

// List of known filename extensions, and functions that create icons from
// files that have this extension.
//
// Two create functions: icon size specified as dim_args, and as dim_ts.

typedef ref<pixmapObj::implObj
	    > dim_arg_loader(const std::string &name,
			     const cached_filename_info &filename,
			     const screen &screenref,
			     const const_pictformat &drawable_pictformat,
			     const const_defaulttheme &theme,
			     const dim_arg &width_arg,
			     const dim_arg &height_arg, icon_scale scale);

typedef ref<pixmapObj::implObj
	    > dim_t_loader(const std::string &name,
			   const cached_filename_info &filename,
			   const screen &screenref,
			   const const_pictformat &drawable_pictformat,
			   const const_defaulttheme &theme,
			   dim_t w, dim_t h, icon_scale scale);

struct LIBCXX_HIDDEN extension_info{
	const char *extension;

	dim_arg_loader *create;
	dim_t_loader *create_pixels;
};

static dim_arg_loader create_sxg_icon_from_filename,
	create_jpg_icon_from_filename,
	create_gif_icon_from_filename,
	create_png_icon_from_filename;

static dim_t_loader create_sxg_icon_from_filename_pixels,
	create_jpg_icon_from_filename_pixels,
	create_gif_icon_from_filename_pixels,
	create_png_icon_from_filename_pixels;

static const struct extension_info extensions[]={
	{".sxg", &create_sxg_icon_from_filename,
	 &create_sxg_icon_from_filename_pixels},
	{".jpg", &create_jpg_icon_from_filename,
	 &create_jpg_icon_from_filename_pixels},
	{".gif", &create_gif_icon_from_filename,
	 &create_gif_icon_from_filename_pixels},
	{".png", &create_png_icon_from_filename,
	 &create_png_icon_from_filename_pixels},
};

////////////////////////////////////////////////////////////////////////////
//
// The first step is to take the icon's name, and search for it, trying
// different exensions until we find the icon's file. The resulting filename
// and type is cached in the extension_cache.

class LIBCXX_HIDDEN icon_cacheObj::cached_filename_infoObj
	: virtual public obj {

public:

	const std::string filename;
	const extension_info &info;

	cached_filename_infoObj(const std::string &filename,
				const extension_info &info)
		: filename{filename}, info{info}
	{
	}

	~cached_filename_infoObj()=default;
};

struct icon_cacheObj::extension_cache_key_t {
	std::string name;
	const_defaulttheme theme;

	inline bool operator==(const extension_cache_key_t &o) const noexcept
	{
		return name==o.name && theme==o.theme;
	}
};

struct icon_cacheObj::extension_cache_key_t_hash
	: public std::hash<std::string>,
	  public std::hash<const_defaulttheme> {

	size_t operator()(const icon_cacheObj::extension_cache_key_t &k)
		const noexcept
	{
		return std::hash<std::string>::operator()(k.name) +
			std::hash<const_defaulttheme>::operator()(k.theme);
	};
};


// Verify that the given file exists.

static bool search_file(std::string &filename,
			const const_defaulttheme &theme)
{
	struct stat stat_buf;

	if (filename.find('/') == filename.npos)
	{
		std::string n=theme->themedir + "/" + filename;

		if (stat(n.c_str(), &stat_buf) == 0)
		{
			filename=n;
			return true;
		}

		// Search for the icon in the default theme.

		size_t p=theme->themedir.rfind('/');

		if (p != std::string::npos)
		{
			n=theme->themedir.substr(0, p) + "/default/"+filename;
			if (stat(n.c_str(), &stat_buf) == 0)
			{
				filename=n;
				return true;
			}
		}
	}
	else
	{
		if (stat(filename.c_str(), &stat_buf) == 0)
			return true;
	}
	return false;
}

// Figure out the image format of a file by its extension. If no extension
// is given, try each extension we know about.

static inline auto search_extension(const std::string &name,
				    const const_defaulttheme &theme)
{
	size_t p=name.rfind('/');

	if (p == name.npos)
		p=0;
	p=name.find('.', p);

	bool found_extension=false;

	for (const auto &filetype:extensions)
	{
		if (p != name.npos)
		{
			// Extension exists in the filename. Simply
			// skip until we find this extension in the extensions
			// list.

			if (name.substr(p) == filetype.extension)
			{
				std::string n{name};

				found_extension=true;

				if (!search_file(n, theme))
					break;

				return std::tuple{n, &filetype};
			}
			continue;
		}

		// No extension. Append each extension until we find the file.
		std::string n{name};

		n += filetype.extension;

		if (!search_file(n, theme))
			continue;

		return std::tuple{n, &filetype};
	}

	if (p != name.npos && !found_extension)
		throw EXCEPTION("Unsupported file format: " << name);

	throw EXCEPTION(name << " not found");
}

static auto search_extension_cached(const screen &s,
				    const std::string &name,
				    const const_defaulttheme &theme)
{
	return s->impl->iconcaches->extension_cache->find_or_create
		({name, theme},
		 [&]
		 {
			 auto [n, filetype]=search_extension(name, theme);

			 return cached_filename_info::create(n, *filetype);
		 });
}

////////////////////////////////////////////////////////////////////////////
//
// And once the filename is determined, if it's a stock image file, it
// gets loaded into a pixmap, which gets cached. The pixmap is based on the
// filename, and pixmap's pictformat.
//
// We do not scale jpg/gif/png images, they get loaded in their native size,
// so this cache needs to be keyed only on the filename, and the pictformat.

struct icon_cacheObj::std_fmt_pixmap_cache_key_t {

	cached_filename_info filename;
	const_pictformat pixmap_pictformat;

	inline bool operator==(const std_fmt_pixmap_cache_key_t &o)
		const noexcept
	{
		return filename == o.filename &&
			pixmap_pictformat == o.pixmap_pictformat;
	}
};

struct icon_cacheObj::std_fmt_pixmap_cache_key_t_hash
	: public std::hash<cached_filename_info>,
	  public std::hash<const_pictformat> {

	size_t operator()(const std_fmt_pixmap_cache_key_t &k) const noexcept
	{
		return std::hash<cached_filename_info>::operator()(k.filename) +
			std::hash<const_pictformat>::operator()
			(k.pixmap_pictformat);
	}
};


// Load a cached pixmap from the std_fmt_pixmap_cache, if nothing's cached invoke
// the load_pixmap() and cache it.

static auto get_cached_pixmap(const cached_filename_info &filename,
			      const screen &screenref,
			      const const_pictformat &drawable_pictformat,
			      ref<pixmapObj::implObj> (*load_pixmap)
			      (const std::string &,
			       const screen &,
			       const const_pictformat &))
{
	return screenref->impl->iconcaches->std_fmt_pixmap_cache
		->find_or_create
		({filename, drawable_pictformat},
		 [&]
		 {
			 return (*load_pixmap)(filename->filename,
					       screenref,
					       drawable_pictformat);
		 });
}

//////////////////////////////////////////////////////////////////////
//
// Once a pixmapObj::implObj is loaded with the icon's image, using
// a particular pictformat, it gets used to construct a pixmap_with_picture
// with a picture for that pictformat, and a particular render_repeat.
//
// This pixmap_with_picture gets cached, based on the pixmapObj::implObj,
// and render_repeat. The cache key does not need to include the pictformat,
// since a different pixmapObj::implObj gets created for different pictformats,
// if needed.

// The next caching layer is a cache of pixmap_with_pictures.

struct icon_cacheObj::pixmap_with_picture_cache_key {

	ref<pixmapObj::implObj> pixmap_impl;
	render_repeat repeat;

	inline bool operator==(const pixmap_with_picture_cache_key &o)
		const noexcept
	{
		return pixmap_impl == o.pixmap_impl && repeat == o.repeat;
	}
};

struct icon_cacheObj::pixmap_with_picture_cache_key_hash
	: public std::hash<ref<pixmapObj::implObj>> {

	inline size_t operator()(const pixmap_with_picture_cache_key &k)
		const noexcept
	{
		return std::hash<ref<pixmapObj::implObj>>::operator()
			(k.pixmap_impl) +
			(size_t)k.repeat;
	}
};


// Given a pixmapObj::implObj, construct and cache a picture with the given
// render_repeat.

static pixmap_with_picture
create_cached_pixmap_with_picture(const screen &screenref,
				  const ref<pixmapObj::implObj> &pixmap_impl,
				  render_repeat repeat)
{
	return screenref->impl->iconcaches
		->pixmap_with_picture_cache->find_or_create
		({pixmap_impl, repeat},
		 [&]
		 {
			 // pixmap_with_picture is a const_ref.
			 //
			 // weakunordered_multimap needs to see a ref here.
			 //
			 // We use a ref, and simply return a
			 // pixmap_with_picture.
			 return refptr_traits<pixmap_with_picture>::ref_t
				 ::create(pixmap_impl, repeat);
		 });
}

///////////////////////////////////////////////////////////////////////////
//
// Once a pixmap_with_picture gets cached, the final caching layer is
// a cache of the resulting icon object.

// Common code to compute a hash for width or height specified as either
// dim_args or dim_ts, that's used by several caches.

static size_t hash_icon_size(const std::variant<std::tuple<dim_arg, dim_arg>,
			     std::tuple<dim_t, dim_t>> &size)
{
	return std::visit(visitor{
			[]
			(const std::tuple<dim_arg, dim_arg> &t)
			{
				std::hash<dim_arg> h;

				return h(std::get<0>(t)) * 65536 +
					h(std::get<1>(t));
			},
			[]
			(const std::tuple<dim_t, dim_t> &t)
			{
				return ((size_t)(dim_t::value_type)
					std::get<0>(t)) * 65536 +
					(dim_t::value_type)std::get<1>(t);
			}}, size);
}

// The icons gets cached based on the icon's pixmap_with_picture, the
// original icon name that was requested, the requested size of the icon,
// and the requested size's scaling algorithm.

struct icon_cacheObj::pixmap_icon_cache_key_t {
	std::string name;
	pixmap_with_picture cached_pixmap_with_picture;
	std::variant<std::tuple<dim_arg, dim_arg>,
		     std::tuple<dim_t, dim_t>> requested_size;
	icon_scale scale;

	bool operator==(const pixmap_icon_cache_key_t &o) const noexcept
	{
		return name == o.name &&
			cached_pixmap_with_picture
			== o.cached_pixmap_with_picture &&
			requested_size == o.requested_size &&
			scale == o.scale;
	}
};

struct icon_cacheObj::pixmap_icon_cache_key_t_hash
	: public std::hash<std::string>,
	  public std::hash<pixmap_with_picture>
{
	size_t operator()(const pixmap_icon_cache_key_t &k) const noexcept
	{
		return std::hash<std::string>::operator()(k.name) +
			std::hash<pixmap_with_picture>
			::operator()(k.cached_pixmap_with_picture) +
			hash_icon_size(k.requested_size) +
			(size_t)k.scale;
	}
};

// Load the cached icon for the given icon pixmap, and attributes.
// If it's not cached, we create a new icon and cache it.

template<typename dim_type>
static icon get_cached_icon(const std::string &name,
			    const const_defaulttheme &theme,
			    const pixmap_with_picture &cached_p_with_p,
			    const dim_type &width,
			    const dim_type &height,
			    icon_scale scale)
{
	auto screen_impl=cached_p_with_p->get_screen()->impl;

	return screen_impl->iconcaches->pixmap_icon_cache->find_or_create
		({name, cached_p_with_p,
				std::tuple{width, height}, scale},
			[&]
			{
				return ref<themeiconpixmapObj<dim_type>>
					::create(name,
						 theme,
						 width, height, scale,
						 cached_p_with_p);
			});
}

//////////////////////////////////////////////////////////////////////////////
//
// SXG icons.
//
// If the filename is an SXG file, two caches get involved. The first
// cache caches the parsedcontents of the SXG file.
//
// Note that even though the SXG file is theme based, cached_filename_info's
// hash key incorporates the defaulttheme already, so we don't need to include
// it in sxg's cache key.

struct icon_cacheObj::sxg_parser_cache_key_t {

	cached_filename_info    filename;

	//! Comparison operator
	inline bool operator==(const sxg_parser_cache_key_t &o) const noexcept
	{
		return filename == o.filename;
	}

};

struct icon_cacheObj::sxg_parser_cache_key_t_hash
	: public std::hash<cached_filename_info> {

	inline size_t operator()(const sxg_parser_cache_key_t &k) const noexcept
	{
		return std::hash<cached_filename_info>::operator()(k.filename);
	};
};

// The second cache is the cache of the rendered pixmapObj::implObj.
// The cache is keyed by the cached parser, the pictformat, and the dimensions
// of the scaled SXG image.

struct icon_cacheObj::sxg_pixmap_cache_key {
	sxg_parser          sxg_image;
	const_pictformat    drawable_pictformat;
	dim_t               width;
	dim_t               height;

	inline bool operator==(const sxg_pixmap_cache_key &o) const noexcept
	{
		return sxg_image == o.sxg_image &&
			drawable_pictformat == o.drawable_pictformat &&
			width == o.width &&
			height == o.height;
	}
};

struct icon_cacheObj::sxg_pixmap_cache_key_hash
	: public std::hash<const_pictformat>,
	  public std::hash<sxg_parser>,
	  public std::hash<dim_t> {

	inline size_t operator()(const sxg_pixmap_cache_key &k) const noexcept
	{
		return (std::hash<const_pictformat>::operator()
			(k.drawable_pictformat) +
			std::hash<sxg_parser>::operator()(k.sxg_image))
			^ (std::hash<dim_t>::operator()(k.width) << 16)
			^ (std::hash<dim_t>::operator()(k.height) << 4);
	}
};

//! Here's a parsed SXG image, the drawable it's for, and its dimensions.

//! Calculate the actual size, in pixels, and return a rendered image from
//! the SXG file. The returned sxg_images are cached.

static auto get_cached_sxg_image(const sxg_parser &sxg,
				 const const_pictformat &drawable_pictformat,
				 const screen &screenref,
				 dim_t w,
				 dim_t h,
				 dim_t preadjust_w,
				 dim_t preadjust_h,
				 const std::optional<const_picture>
				 &background_color)
{
	// preadjust_[wh] is the original requested size of the rendered
	// sxg image.
	//
	// w and h is adjusted by the SXG image's asoect scaling factor.
	//
	// If the SXG image defines a "background" color, and the preadjusted
	// image is larger then (w, h) -- this means that icon_scale::nomore
	// was used -- we'll render the SXG image into a temporary pixmap and
	// picture, clear the preadjust_w and preadjust_h-sized pixmap to the
	// background color, then center the rendered image; so we wind up
	// with the reuqested size.
	//
	// First, make sure that preadjust_w/h is not smaller;

	if (preadjust_w < w)
		preadjust_w=w;

	if (preadjust_h < h)
		preadjust_h=h;

	bool has_background_color=background_color.has_value();
	if ( !( (preadjust_w > w || preadjust_h > h) && has_background_color))
	{
		has_background_color=false;
		preadjust_w=w;
		preadjust_h=h;
	}

	// At this point: creating a (preadjust_w, preadjust_h) picture.

	return screenref->impl->iconcaches
		->sxg_pixmap_cache->find_or_create
		({sxg, drawable_pictformat,
				preadjust_w, preadjust_h},
		 [&]
		 {
			 auto pixmap=
				 screenref->create_pixmap(drawable_pictformat,
							  preadjust_w,
							  preadjust_h);

			 // Compute points in the image. render_points()
			 // needs to know the size of the pixmap that render()
			 // will receive.

			 pixmap->impl->points_of_interest=
				 has_background_color
				 ? sxg->render_points(w, h, pixmap)
				 : sxg->render_points(preadjust_w,
						      preadjust_h, pixmap);

			 auto picture=pixmap->create_picture();

			 if (!has_background_color)
			 {
				 sxg->render(picture, pixmap);
			 }
			 else
			 {
				 auto temp_pixmap=
					 screenref->create_pixmap
					 (drawable_pictformat, w, h);

				 auto temp_picture=
					 temp_pixmap->create_picture();


				 sxg->render(temp_picture, temp_pixmap);

				 coord_t offset_x=coord_t::truncate
					 ((preadjust_w-w)/2);
				 coord_t offset_y=coord_t::truncate
					 ((preadjust_h-h)/2);

				 picture->composite
					 (*background_color,
					  -offset_x,
					  -offset_y,
					  {0, 0, preadjust_w, preadjust_h});

				 picture->composite(temp_picture, 0, 0,
						    {offset_x, offset_y,
								    w, h},
						    render_pict_op::op_src);

				 // Also adjust the points.

				 for (auto &p:pixmap->impl->points_of_interest)
				 {
					 p.second.first=coord_t::truncate
						 (p.second.first+offset_x);
					 p.second.second=coord_t::truncate
						 (p.second.second+offset_y);
				 }
			 }

			 // Time to discard the temporary pixmap public object
			 // that was used to build the pixmap.

			 return pixmap->impl;
		 });
}

static auto create_sxg_image(const std::string &name,
			     const screen &screenref,
			     const const_pictformat &drawable_pictformat,
			     const sxg_parser &sxg,
			     const dim_arg &width_arg,
			     const dim_arg &height_arg)
{
	dim_t w=sxg->theme->get_theme_dim_t(width_arg, themedimaxis::width);
	dim_t h=sxg->theme->get_theme_dim_t(height_arg, themedimaxis::height);

	if (w == 0 && h == 0)
	{
		w=sxg->default_width();
		h=sxg->default_height();
	}
	else
	{
		if (w != 0)
			w=sxg->adjust_width(w, icon_scale::nearest);
		if (h != 0)
			h=sxg->adjust_height(h, icon_scale::nearest);

		if (w == 0)
			w=sxg->width_for_height(h, icon_scale::nearest);

		if (h == 0)
			h=sxg->height_for_width(w, icon_scale::nearest);
	}

	return get_cached_sxg_image(sxg, drawable_pictformat, screenref, w, h,
				    w, h, {});
}

static auto create_sxg_image(const std::string &name,
			     const screen &screenref,
			     const const_pictformat &drawable_pictformat,
			     const sxg_parser &sxg,
			     dim_t w, dim_t h,
			     icon_scale scale)
{
	auto background_color=sxg->get_background_color(w, h, screenref->impl);

	if (background_color)
		scale=icon_scale::nomore; // We can scale.

	dim_t orig_w=w, orig_h=h;

	if (w == 0 && h == 0)
	{
		orig_w=w=sxg->default_width();
		orig_h=h=sxg->default_height();
	}
	else
	{
		if (w != 0)
			w=sxg->adjust_width(w, scale);
		if (h != 0)
			h=sxg->adjust_height(h, scale);

		if (w == 0)
			orig_w=w=sxg->width_for_height(h, icon_scale::nearest);

		if (h == 0)
			orig_h=h=sxg->height_for_width(w, icon_scale::nearest);
	}

	return get_cached_sxg_image(sxg, drawable_pictformat, screenref,
				    w, h,
				    orig_w, orig_h,
				    background_color);
}

////////////////////////////////////////////////////////////////////////////////

static ref<pixmapObj::implObj>
create_sxg_icon_from_filename(const std::string &name,
			      const cached_filename_info &filename,
			      const screen &screenref,
			      const const_pictformat &drawable_pictformat,
			      const const_defaulttheme &theme,
			      const dim_arg &width_arg,
			      const dim_arg &height_arg,
			      icon_scale scale)
{
	auto sxg=screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({filename},
		 [&]
		 {
			 return sxg_parser::create(filename->filename,
						   screenref,
						   theme);
		 });

        return create_sxg_image(name, screenref, drawable_pictformat, sxg,
				width_arg, height_arg);
}

static ref<pixmapObj::implObj>
create_sxg_icon_from_filename_pixels(const std::string &name,
				     const cached_filename_info &filename,
				     const screen &screenref,
				     const const_pictformat &drawable_pictformat,
				     const const_defaulttheme &theme,
				     dim_t w, dim_t h,
				     icon_scale scale)
{
	auto sxg=screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({filename},
		 [&]
		 {
			 return sxg_parser::create(filename->filename,
						   screenref,
						   theme);
		 });

        return create_sxg_image(name, screenref, drawable_pictformat, sxg,
				w, h,
				scale);
}

///////////////////////////////////////////////////////////////////////////////

// RAII wrapper for jpeg_decompress_struct

class LIBCXX_HIDDEN jpeg_decompress {

public:

	struct jpeg_decompress_struct cinfo=jpeg_decompress_struct();

	inline jpeg_decompress()
	{
		jpeg_create_decompress(&cinfo);
	}

	inline ~jpeg_decompress()
	{
		jpeg_destroy_decompress(&cinfo);
	}
};

// RAII wrapper for jpeg_start_decompress and jpeg_finish_decompress

class LIBCXX_HIDDEN jpeg_do_decompress {

	jpeg_decompress &decompress;

 public:
	inline jpeg_do_decompress(jpeg_decompress &decompressArg)
		: decompress(decompressArg)
	{
		if (!jpeg_start_decompress(&decompress.cinfo))
			throw EXCEPTION("Cannot start jpeg decompression");
	}

	inline ~jpeg_do_decompress()
	{
		jpeg_finish_decompress(&decompress.cinfo);
	}
};

// Opaque pointer to jpeg_client_data gets passed to jpeg-turbo

class LIBCXX_HIDDEN jpeg_client_data {
 public:

	std::string error_message;

};

static void jpeg_error_exit (j_common_ptr cinfo)
{
	struct jpeg_error_mgr *error_mgr=(struct jpeg_error_mgr *)cinfo->err;

	char buf[JMSG_LENGTH_MAX];

	error_mgr->format_message(cinfo, buf);

	throw EXCEPTION(buf);
}

static void jpeg_error_output (j_common_ptr cinfo)
{
	struct jpeg_error_mgr *error_mgr=(struct jpeg_error_mgr *)cinfo->err;

	char buf[JMSG_LENGTH_MAX];

	error_mgr->format_message(cinfo, buf);

	auto client_data=
		reinterpret_cast<jpeg_client_data *>(cinfo->client_data);

	client_data->error_message=buf;
}

#define OVERFLOW_CHK(x) ((x) < 0 || (x) > (decltype(x))(((uint16_t)~0) >> 1))

// Construct a pixmap from a .jpg file.
static ref<pixmapObj::implObj>
create_pixmap_from_jpg(const std::string &filename,
		       const screen &screenref,
		       const const_pictformat &drawable_pictformat)
{
	auto f=mmapfile::create(fd::base::open(filename, O_RDONLY), PROT_READ);

	jpeg_client_data client_data;
	struct jpeg_error_mgr error_mgr;

	jpeg_decompress decompress;
	decompress.cinfo.err = jpeg_std_error(&error_mgr);
	decompress.cinfo.client_data=
		reinterpret_cast<void *>(&client_data);
	error_mgr.error_exit=jpeg_error_exit;
	error_mgr.output_message=jpeg_error_output;

	jpeg_mem_src(&decompress.cinfo,
		     reinterpret_cast<unsigned char *>(f->buffer()),
		     f->size());

	if (jpeg_read_header(&decompress.cinfo, TRUE) != JPEG_HEADER_OK
	    || OVERFLOW_CHK(decompress.cinfo.output_width)
	    || OVERFLOW_CHK(decompress.cinfo.output_height))
		throw EXCEPTION("Cannot read jpeg header");

	jpeg_do_decompress do_decompress(decompress);

	auto pixmap=screenref->create_pixmap
		(drawable_pictformat,
		 decompress.cinfo.output_width,
		 decompress.cinfo.output_height);

	pixmap_loader loader{pixmap};

	auto components=decompress.cinfo.output_components;

	auto rowsize=decompress.cinfo.output_width
		* decompress.cinfo.output_components;

	auto buffer_ptr=(*decompress.cinfo.mem->alloc_sarray)
		((j_common_ptr)&decompress.cinfo, JPOOL_IMAGE,
		 rowsize, 1);

	dim_t y=0;

	while (decompress.cinfo.output_scanline
	       < decompress.cinfo.output_height)
	{
		(void) jpeg_read_scanlines(&decompress.cinfo,
					   buffer_ptr, 1);

		auto p=buffer_ptr[0];

		for (dim_t x=0; x<decompress.cinfo.output_width;
		     ++x, p += components)
		{
			auto r=*p, g=*p, b=*p;
			decltype(r) a=~0;

			// If there's only one component, load the
			// same value for r, g, and b; else grab
			// them.
			if (components >= 3)
			{
				g=p[1];
				b=p[2];
			}

			// Who knows, maybe an alpha channel, someday,
			// will magically appear.
			if (components > 3)
				a=p[3];

			loader.put_rgb8(x, y, r, g, b, a);
		}
		++y;
	}

	if (!client_data.error_message.empty())
		throw EXCEPTION(client_data.error_message);

	loader.flush();
	return pixmap->impl;
}

extern "C" {

	static int read_gif(GifFileType *f, GifByteType *, int n);
};

class LIBCXX_HIDDEN gif_open {

 public:

	char *buf;
	size_t left;

	GifFileType *gif;
	int error_code;

	inline gif_open(const mmapfile &file)
		: buf(file->buffer()), left(file->size()),
		gif(DGifOpen(reinterpret_cast<void *>(this), read_gif,
			     &error_code))
		{
			if (!gif)
				throw EXCEPTION(GifErrorString(error_code));
		}

	static void error()
	{
		throw EXCEPTION("GIF parsing error");
	}

	inline ~gif_open()
	{
		int error_code;

		DGifCloseFile(gif, &error_code);
	}
};

extern "C" {
	static int read_gif(GifFileType *f, GifByteType *ptr, int n)
	{
		auto obj=reinterpret_cast<gif_open *>(f->UserData);
		int done=0;

		while (n && obj->left)
		{
			*ptr=*obj->buf;
			++ptr;
			++obj->buf;
			--obj->left;
			--n;
			++done;
		}
		return done;
	}
};

static ref<pixmapObj::implObj>
create_pixmap_from_gif(const std::string &filename,
		       const screen &screenref,
		       const const_pictformat &drawable_pictformat)
{
	auto f=mmapfile::create(fd::base::open(filename, O_RDONLY), PROT_READ);

	gif_open gif(f);

	if (DGifSlurp(gif.gif) == GIF_ERROR)
		gif.error();

	if (gif.gif->ImageCount == 0 ||
	    OVERFLOW_CHK(gif.gif->SWidth) ||
	    OVERFLOW_CHK(gif.gif->SHeight))
		gif.error();

	dim_t width=gif.gif->SWidth;
	dim_t height=gif.gif->SHeight;
	auto transparent=gif.gif->SBackGroundColor;

	auto pixmap=screenref->create_pixmap(drawable_pictformat,
					     width, height);

	pixmap_loader loader{pixmap};

	// We'll only look at the first image in the file.

	auto &image=gif.gif->SavedImages[0];

	auto &desc=image.ImageDesc;
	auto raster=image.RasterBits;
	auto colormap=desc.ColorMap ? desc.ColorMap
		: gif.gif->SColorMap;

	if (!raster || !colormap)
		gif.error();

	if (desc.Left < 0 ||
	    desc.Top < 0 ||
	    desc.Left >= gif.gif->SWidth ||
	    desc.Top >= gif.gif->SHeight ||
	    gif.gif->SWidth - desc.Left < desc.Width ||
	    gif.gif->SHeight - desc.Top < desc.Height)
		gif.error();

	dim_t x=desc.Left;
	dim_t y=desc.Top;
	dim_t w=desc.Width;
	dim_t h=desc.Height;

	auto has_transparent_color=
		transparent >= 0 &&
		transparent < colormap->ColorCount;

	for (dim_t row=0; row<h; ++row)
		for (dim_t col=0; col<w; ++col)
		{
			if (*raster < colormap->ColorCount &&

			    (!has_transparent_color ||
			     colormap->Colors[*raster].Red !=
			     colormap->Colors[transparent].Red ||
			     colormap->Colors[*raster].Green !=
			     colormap->Colors[transparent].Green ||
			     colormap->Colors[*raster].Blue !=
			     colormap->Colors[transparent].Blue))
			{
				auto &rgb=colormap->Colors[*raster];

				loader.put_rgb8(dim_t::truncate(x+col),
						dim_t::truncate(y+row),
						rgb.Red,
						rgb.Green,
						rgb.Blue, 255);
			}
			// loader initializes the buffer with calloc, which
			// has the effect of clearing all unused buts to
			// the transparent color.

			++raster;
		}
	loader.flush();
	return pixmap->impl;
}

//! RAII wrapper for png_image.

class LIBCXX_HIDDEN png_open {

 public:

	png_image image;

	inline png_open(const x::mmapfile &file)
		: image(png_image{})
	{
		image.version = PNG_IMAGE_VERSION;

		if (png_image_begin_read_from_memory(&image,
						     (png_const_voidp)
						     file->buffer(),
						     file->size()) == 0)
			error();
	}

	static void error()
	{
		throw EXCEPTION("PNG parsing error");
	}

	inline ~png_open()
	{
		png_image_free(&image);
	}
};

static ref<pixmapObj::implObj>
create_pixmap_from_png(const std::string &filename,
		       const screen &screenref,
		       const const_pictformat &drawable_pictformat)
{
	auto f=mmapfile::create(fd::base::open(filename, O_RDONLY), PROT_READ);

	png_open png(f);

	png.image.format=PNG_FORMAT_RGBA;

	std::vector<png_byte> buffer;

	buffer.resize(PNG_IMAGE_SIZE(png.image));

	if (png_image_finish_read(&png.image, nullptr, &*buffer.begin(),
				  0, nullptr) == 0 ||
	    OVERFLOW_CHK(png.image.width) ||
	    OVERFLOW_CHK(png.image.height) ||
	    png.image.width * png.image.height * 4 != buffer.size())
		png.error();

	dim_t width=png.image.width;
	dim_t height=png.image.height;

	auto pixmap=screenref->create_pixmap(drawable_pictformat,
					     width, height);

	pixmap_loader loader{pixmap};

	auto iter=buffer.begin();

	for (dim_t y=0; y<height; ++y)
		for (dim_t x=0; x<width; ++x)
		{
			loader.put_rgb8(x, y,
					iter[0],
					iter[1],
					iter[2],
					iter[3]);
			iter += 4;
		}
	loader.flush();

	return pixmap->impl;
}

// Load a jpg into an icon object.

static ref<pixmapObj::implObj>
create_jpg_icon_from_filename_pixels(const std::string &name,
				     const cached_filename_info &filename,
				     const screen &screenref,
				     const const_pictformat &drawable_pictformat,
				     const const_defaulttheme &theme,
				     dim_t w, dim_t h,
				     icon_scale scale)
{
	return get_cached_pixmap(filename, screenref, drawable_pictformat,
				 create_pixmap_from_jpg);
}

static ref<pixmapObj::implObj>
create_jpg_icon_from_filename(const std::string &name,
			      const cached_filename_info &filename,
			      const screen &screenref,
			      const const_pictformat &drawable_pictformat,
			      const const_defaulttheme &theme,
			      const dim_arg &width_arg,
			      const dim_arg &height_arg,
			      icon_scale scale)
{
	return get_cached_pixmap(filename, screenref, drawable_pictformat,
				 create_pixmap_from_jpg);
}

// Load a gif into an icon object.

static ref<pixmapObj::implObj>
create_gif_icon_from_filename_pixels(const std::string &name,
				     const cached_filename_info &filename,
				     const screen &screenref,
				     const const_pictformat &drawable_pictformat,
				     const const_defaulttheme &theme,
				     dim_t w, dim_t h,
				     icon_scale scale)
{
	return get_cached_pixmap(filename, screenref, drawable_pictformat,
				 create_pixmap_from_gif);
}

static ref<pixmapObj::implObj>
create_gif_icon_from_filename(const std::string &name,
			      const cached_filename_info &filename,
			      const screen &screenref,
			      const const_pictformat &drawable_pictformat,
			      const const_defaulttheme &theme,
			      const dim_arg &width_arg,
			      const dim_arg &height_arg,
			      icon_scale scale)
{
	return get_cached_pixmap(filename, screenref, drawable_pictformat,
				 create_pixmap_from_gif);
}

// Load a png into an icon object.

static ref<pixmapObj::implObj>
create_png_icon_from_filename_pixels(const std::string &name,
				     const cached_filename_info &filename,
				     const screen &screenref,
				     const const_pictformat &drawable_pictformat,
				     const const_defaulttheme &theme,
				     dim_t w, dim_t h,
				     icon_scale scale)
{
	return get_cached_pixmap(filename, screenref, drawable_pictformat,
				 create_pixmap_from_png);
}

static ref<pixmapObj::implObj>
create_png_icon_from_filename(const std::string &name,
			      const cached_filename_info &filename,
			      const screen &screenref,
			      const const_pictformat &drawable_pictformat,
			      const const_defaulttheme &theme,
			      const dim_arg &width_arg,
			      const dim_arg &height_arg,
			      icon_scale scale)
{
	return get_cached_pixmap(filename, screenref, drawable_pictformat,
				 create_pixmap_from_png);
}

std::vector<icon> drawableObj::implObj
::create_icon_vector(const std::vector<std::string> &images)
{
	std::vector<icon> icons;

	icons.reserve(images.size());

	for (const auto &name:images)
		icons.push_back(create_icon({name}));

	return icons;
}

icon drawableObj::implObj::create_icon(const image_color &args)
{
	return create_new_icon(get_screen(),
			       drawable_pictformat, args);
}

icon create_new_icon(const screen &screenref,
		     const const_pictformat &drawable_pictformat,
		     const image_color &args)
{
	auto theme=screenref->impl->current_theme.get();

	auto cached_filename=
		search_extension_cached(screenref, args.name, theme);

	auto pixmap_impl=(*cached_filename->info.create)
		(args.name, cached_filename, screenref, drawable_pictformat,
		 theme,
		 args.width, args.height, args.scale);

	auto cached_pixmap_with_picture=
		create_cached_pixmap_with_picture(screenref, pixmap_impl,
						  args.repeat);

	return get_cached_icon(args.name, theme, cached_pixmap_with_picture,
			       args.width, args.height,
			       args.scale);
}

icon drawableObj::implObj
::create_icon_pixels(const std::string &name,
		     render_repeat icon_repeat,
		     dim_t w, dim_t h, icon_scale scale)
{
	auto screen=get_screen();
	auto theme=screen->impl->current_theme.get();

	auto cached_filename=search_extension_cached(screen, name, theme);

	auto pixmap_impl=(*cached_filename->info.create_pixels)
		(name, cached_filename, screen, drawable_pictformat, theme,
		 w, h, scale);
	auto cached_pixmap_with_picture=
		create_cached_pixmap_with_picture(screen, pixmap_impl,
						  icon_repeat);

	return get_cached_icon(name, theme, cached_pixmap_with_picture,
			       w, h,
			       scale);

}
//////////////////////////////////////////////////////////////////////////

icon_cacheObj::icon_cacheObj()
	: extension_cache{extension_cache_t::create()},
	  std_fmt_pixmap_cache{std_fmt_pixmap_cache_t::create()},
	  sxg_parser_cache{sxg_parser_cache_t::create()},
	  sxg_pixmap_cache{sxg_pixmap_cache_t::create()},
	  pixmap_with_picture_cache{pixmap_with_picture_cache_t::create()},
	  pixmap_icon_cache{pixmap_icon_cache_t::create()}
{
}

icon_cacheObj::~icon_cacheObj()=default;

///////////////////////////////////////////////////////////////////////

LIBCXXW_NAMESPACE_END
