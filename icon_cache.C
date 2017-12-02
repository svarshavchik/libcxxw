/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "icon_cache.H"
#include "sxg/sxg_parser.H"
#include "defaulttheme.H"
#include "screen.H"
#include "pixmap.H"
#include "drawable.H"
#include "icon.H"
#include "icon_image.H"
#include "x/w/picture.H"
#include <x/refptr_hash.H>
#include <x/number_hash.H>
#include <x/weakunordered_multimap.H>
#include <x/functional.H>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

LIBCXXW_NAMESPACE_START

struct icon_cacheObj::sxg_cache_key_t {

	std::string filename;
	defaulttheme theme;

	//! Comparison operator
	bool operator==(const sxg_cache_key_t &) const;
};


bool icon_cacheObj::sxg_cache_key_t::operator==(const sxg_cache_key_t &o) const
{
	return filename == o.filename && theme == o.theme;
}

struct icon_cacheObj::sxg_cache_key_t_hash
	: public std::hash<std::string>,
	  public std::hash<defaulttheme> {

	size_t operator()(const sxg_cache_key_t &k) const
	{
		return std::hash<std::string>::operator()(k.filename) +
			std::hash<defaulttheme>::operator()(k.theme);
	};
};

// SXG images are cached by the following parameters:

struct icon_cacheObj::sxg_image_cache_key {
	sxg_parser          sxg_image;
	const_pictformat    drawable_pictformat;
	render_repeat       repeat;
	dim_t               width;
	dim_t               height;

	bool operator==(const sxg_image_cache_key &o) const;
};

bool icon_cacheObj::sxg_image_cache_key
::operator==(const sxg_image_cache_key &o) const
{
	return sxg_image == o.sxg_image &&
		drawable_pictformat == o.drawable_pictformat &&
		repeat == o.repeat &&
		width == o.width &&
		height == o.height;
}

struct icon_cacheObj::sxg_image_cache_key_hash
	: public std::hash<const_pictformat>,
	  public std::hash<sxg_parser>,
	  public std::hash<dim_t> {

	size_t operator()(const sxg_image_cache_key &k) const
	{
		return (std::hash<const_pictformat>::operator()
			(k.drawable_pictformat) +
			std::hash<sxg_parser>::operator()(k.sxg_image))
			^ (size_t)k.repeat
			^ (std::hash<dim_t>::operator()(k.width) << 16)
			^ (std::hash<dim_t>::operator()(k.height) << 4);
	}
};

//////////////////////////////////////////////////////////////////////////

struct icon_cacheObj::mm_image_cache_key {

	std::string      filename;
	defaulttheme     theme;
	const_pictformat image_pictformat;
	render_repeat    repeat;
	double           widthmm;
	double           heightmm;

	bool operator==(const mm_image_cache_key &o) const
	{
		return filename == o.filename &&
			theme == o.theme &&
			image_pictformat == o.image_pictformat &&
			repeat == o.repeat &&
			widthmm == o.widthmm &&
			heightmm == o.heightmm;
	}
};

struct icon_cacheObj::mm_image_cache_key_hash
	: public std::hash<std::string>,
	  public std::hash<defaulttheme>,
	  public std::hash<const_pictformat> {

	size_t operator()(const mm_image_cache_key &k) const
	{
		return (std::hash<defaulttheme>::operator()(k.theme) +
			std::hash<const_pictformat>::operator()
			(k.image_pictformat)) ^
			(std::hash<std::string>::operator()(k.filename) +
			 (size_t)k.repeat + (((size_t)k.widthmm) << 4)
			 + (((size_t)k.heightmm) << 12));
	}
};

//////////////////////////////////////////////////////////////////////////

icon_cacheObj::icon_cacheObj()
	: sxg_parser_cache{sxg_parser_cache_t::create()},
	  sxg_image_cache(sxg_image_cache_t::create()),
	  mm_image_cache(mm_image_cache_t::create())
{
}

icon_cacheObj::~icon_cacheObj()=default;

///////////////////////////////////////////////////////////////////////

//! An SXG-based icon.

template<typename dim_type>
class LIBCXX_HIDDEN sxg_iconObj : public iconObj {

	//! The parsed SXG object.
	sxg_parser      current_sxg_thread_only;

	//! The image created from the object

	const_icon_image current_image_thread_only;

 public:

	THREAD_DATA_ONLY(current_sxg);
	THREAD_DATA_ONLY(current_image);

	//! Name
	const std::string name;

	//! Original requested width in millimeters
	const dim_type width;

	//! Original requested height in millimeters
	const dim_type height;

	//! Original scaling hint

	const icon_scale scale;
	//! Constructor
	sxg_iconObj(const std::string_view &name,
		    const dim_type &width,
		    const dim_type &height,
		    icon_scale scale,
		    const sxg_parser &orig_sxg,
		    const const_icon_image &orig_sxg_image)
		: iconObj(orig_sxg_image),
		current_sxg_thread_only(orig_sxg),
		current_image_thread_only(orig_sxg_image),
		name(name),
		width(width),
		height(height),
		scale(scale)
		{
		}

	//! Destructor
	~sxg_iconObj()=default;

	//! Check if a new theme is installed, if so create a replacement icon.

	icon theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme) override;

	icon resizemm(IN_THREAD_ONLY,
		      const dim_arg &width_arg, const dim_arg &height_arg)
		override
	{
		return create_sxg_image(name,
					&*current_image(IN_THREAD)
					->icon_pixmap->impl,
					current_sxg(IN_THREAD),
					current_image(IN_THREAD)->repeat,
					width_arg,
					height_arg);
	}

	icon resize(IN_THREAD_ONLY, dim_t w, dim_t h,
		      icon_scale scale) override
	{
		return create_sxg_image(name,
					&*current_image(IN_THREAD)
					->icon_pixmap->impl,
					current_sxg(IN_THREAD),
					current_image(IN_THREAD)->repeat,
					w,
					h, scale);
	}
};

//! Here's a parsed SXG image, the drawable it's for, and its dimensions.

//! Calculate the actual size, in pixels, and return a rendered image from
//! the SXG file. The returned sxg_images are cached.

static auto get_cached_sxg_image(const sxg_parser &sxg,
				 drawableObj::implObj *drawable_impl,
				 render_repeat repeat,
				 dim_t w,
				 dim_t h,
				 dim_t preadjust_w,
				 dim_t preadjust_h,
				 const std::optional<rgb> &background_color)
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

	return sxg->screenref->impl->iconcaches
		->sxg_image_cache->find_or_create
		({sxg, drawable_impl->drawable_pictformat, repeat,
				preadjust_w, preadjust_h},
		 [&]
		 {
			 auto pixmap=drawable_impl->create_pixmap(preadjust_w,
								  preadjust_h);
			 auto picture=pixmap->create_picture();

			 std::unordered_map<std::string,
					    std::pair<coord_t, coord_t>> points;

			 if (!has_background_color)
			 {
				 sxg->render(picture, pixmap, points);
			 }
			 else
			 {
				 auto temp_pixmap=
					 drawable_impl->create_pixmap(w, h);

				 auto temp_picture=
					 temp_pixmap->create_picture();

				 sxg->render(temp_picture, temp_pixmap,
					     points);

				 picture->fill_rectangle({0, 0,
							 preadjust_w,
							 preadjust_h},
					 background_color.value());

				 coord_t offset_x=coord_t::truncate
					 ((preadjust_w-w)/2);
				 coord_t offset_y=coord_t::truncate
					 ((preadjust_h-h)/2);


				 picture->composite(temp_picture, 0, 0,
						    {offset_x, offset_y,
								    w, h},
						    render_pict_op::op_over);

				 // Also adjust the points.

				 for (auto &p:points)
				 {
					 p.second.first=coord_t::truncate
						 (p.second.first+offset_x);
					 p.second.second=coord_t::truncate
						 (p.second.second+offset_y);
				 }
			 }
			 return icon_image::create(picture, pixmap, repeat,
						   points);
		 });
}

static icon create_sxg_image(const std::string_view &name,
			     drawableObj::implObj *drawable_impl,
			     const sxg_parser &sxg,
			     render_repeat repeat,
			     const dim_arg &width_arg,
			     const dim_arg &height_arg)
{
	dim_t w=sxg->theme->get_theme_width_dim_t(width_arg);
	dim_t h=sxg->theme->get_theme_height_dim_t(height_arg);

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

	auto image=get_cached_sxg_image(sxg, drawable_impl, repeat, w, h,
					w, h, std::optional<rgb>());

	return ref<sxg_iconObj<dim_arg>>::create(name,
						 width_arg,
						 height_arg,
						 icon_scale::nearest,
						 sxg, image);
}

static icon create_sxg_image(const std::string_view &name,
			     drawableObj::implObj
			     *drawable_impl,
			     const sxg_parser &sxg,
			     render_repeat repeat,
			     dim_t w, dim_t h,
			     icon_scale scale)
{
	auto background_color=sxg->background_color
		(drawable_impl->get_screen()->impl->current_theme.get());

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

	auto image=get_cached_sxg_image(sxg, drawable_impl, repeat, w, h,
					orig_w, orig_h, background_color);

	return ref<sxg_iconObj<dim_t>>::create(name,
					       w,
					       h,
					       scale,
					       sxg, image);
}

static icon
create_sxg_icon_from_filename(const std::string_view &name,
			      const std::string &filename,
			      drawableObj::implObj *for_drawable,
			      const screen &screenref,
			      const defaulttheme &theme,
			      render_repeat icon_repeat,
			      const dim_arg &width_arg,
			      const dim_arg &height_arg)
{
	auto sxg=screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({filename, theme},
		 [&]
		 {
			 return sxg_parser::create(filename, screenref,
						   theme);
		 });

        return create_sxg_image(name, for_drawable, sxg, icon_repeat,
				width_arg, height_arg);
}

static icon
create_sxg_icon_from_filename_pixels(const std::string_view &name,
				     const std::string &filename,
				     drawableObj::implObj *for_drawable,
				     const screen &screenref,
				     const defaulttheme &theme,
				     render_repeat icon_repeat,
				     dim_t w, dim_t h,
				     icon_scale scale)
{
	auto sxg=screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({filename, theme},
		 [&]
		 {
			 return sxg_parser::create(filename, screenref,
						   theme);
		 });

        return create_sxg_image(name, for_drawable, sxg, icon_repeat, w, h,
				scale);
}

static const struct LIBCXX_HIDDEN extension_info{
	const char *extension;

	icon (*create)(const std::string_view &name,
		       const std::string &filename,
		       drawableObj::implObj *for_drawable,
		       const screen &screenref,
		       const defaulttheme &theme,
		       render_repeat icon_repeat,
		       const dim_arg &width_arg,
		       const dim_arg &height_arg);

	icon (*create_pixels)(const std::string_view &name,
			      const std::string &filename,
			      drawableObj::implObj *for_drawable,
			      const screen &screenref,
			      const defaulttheme &theme,
			      render_repeat icon_repeat,
			      dim_t w, dim_t h, icon_scale scale);
} extensions[]={
	{".sxg", &create_sxg_icon_from_filename,
	 &create_sxg_icon_from_filename_pixels},
};

// Verify that the given file exists.

static bool search_file(std::string &filename,
			const defaulttheme &theme)
{
	struct stat stat_buf;

	if (stat(filename.c_str(), &stat_buf) == 0)
		return true;

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

	return false;
}

// Figure out the image format of a file by its extension. If no extension
// is given, try each extension we know about.

static void do_search_ext(const std::string_view &name,
			  const defaulttheme &theme,
			  const function<void (const std::string &,
					       const extension_info &)> &cb)
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

				cb(n, filetype);
				return;
			}
			continue;
		}

		// No extension. Append each extension until we find the file.
		std::string n{name};

		n += filetype.extension;

		if (!search_file(n, theme))
			continue;

		cb(n, filetype);
		return;
	}

	if (p != name.npos && !found_extension)
		throw EXCEPTION("Unsupported file format: " << name);

	throw EXCEPTION(name << " not found");
}

template<typename functor>
static void search_extension(const std::string_view &name,
			     const defaulttheme &theme,
			     functor &&f)
{
	do_search_ext(name, theme,
		      make_function<void (const std::string &,
					  const extension_info &)>
		      (std::forward<functor>(f)));
}

std::vector<icon> drawableObj::implObj
::create_icon_vector(const std::vector<std::string_view> &images)
{
	std::vector<icon> icons;

	icons.reserve(images.size());

	for (const auto &name:images)
		icons.push_back(create_icon(name, render_repeat::none,
					    0, 0));

	return icons;
}

icon drawableObj::implObj
::create_icon(const std::string_view &name,
	      render_repeat icon_repeat,
	      const dim_arg &width_arg,
	      const dim_arg &height_arg)
{
	auto screen=get_screen();
	auto theme=*current_theme_t::lock{screen->impl->current_theme};

	iconptr i;

	search_extension(name, theme,
			 [&, this]
			 (const auto &n, const auto &filetype)
			 {
				 i=(*filetype.create)
					 (name, n, this, screen, theme,
					  icon_repeat,
					  width_arg, height_arg);
			 });
	return i;
}

icon drawableObj::implObj
::create_icon_pixels(const std::string_view &name,
		     render_repeat icon_repeat,
		     dim_t w, dim_t h, icon_scale scale)
{
	auto screen=get_screen();
	auto theme=*current_theme_t::lock{screen->impl->current_theme};

	iconptr i;

	search_extension(name, theme,
			 [&, this]
			 (const auto &n, const auto &filetype)
			 {
				 i=(*filetype.create_pixels)
					 (name, n, this, screen, theme,
					  icon_repeat, w, h, scale);
			 });

	return i;
}

template<>
icon sxg_iconObj<dim_arg>::theme_updated(IN_THREAD_ONLY,
					 const defaulttheme &new_theme)
{

	if (new_theme == current_sxg(IN_THREAD)->theme)
		return icon(this); // Unchanged

	// All right, take it from the top.
	auto icon=image->icon_pixmap->impl
		->create_icon(name,
			      image->repeat,
			      width,
			      height);

	// We technically need to call initialize(), hopefully a mere
	// formality.
	return icon->initialize(IN_THREAD);
}

template<>
icon sxg_iconObj<dim_t>::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	auto theme= *current_theme_t::lock{current_sxg(IN_THREAD)
					   ->screenref->impl
					   ->current_theme};

	if (theme == current_sxg(IN_THREAD)->theme)
		return icon(this); // Unchanged

	// All right, take it from the top.
	auto icon=image->icon_pixmap->impl
		->create_icon_pixels(name,
				     image->repeat,
				     width, height, scale);

	// We technically need to call initialize(), hopefully a mere
	// formality.
	return icon->initialize(IN_THREAD);
}

template icon sxg_iconObj<dim_arg>::resizemm(IN_THREAD_ONLY,
					     const dim_arg &, const dim_arg &);
template icon sxg_iconObj<dim_arg>::resize(IN_THREAD_ONLY, dim_t, dim_t,
					  icon_scale scale);
template icon sxg_iconObj<dim_t>::resizemm(IN_THREAD_ONLY,
					   const dim_arg &, const dim_arg &);
template icon sxg_iconObj<dim_t>::resize(IN_THREAD_ONLY, dim_t, dim_t,
					 icon_scale scale);

LIBCXXW_NAMESPACE_END
