/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "icon_cache.H"
#include "sxg/sxg_image.H"
#include "sxg/sxg_parser.H"
#include "defaulttheme.H"
#include "screen.H"
#include "pixmap.H"
#include "drawable.H"
#include "icon.H"
#include "x/w/picture.H"
#include <x/refptr_hash.H>
#include <x/number_hash.H>
#include <x/weakunordered_multimap.H>
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

	const_sxg_image current_image_thread_only;

 public:

	THREAD_DATA_ONLY(current_sxg);
	THREAD_DATA_ONLY(current_image);

	//! Name
	const std::string name;

	//! Original requested width in millimeters
	const dim_type width;

	//! Original requested height in millimeters
	const dim_type height;

	//! Constructor
	sxg_iconObj(const std::experimental::string_view &name,
		    dim_type width,
		    dim_type height,
		    const sxg_parser &orig_sxg,
		    const const_sxg_image &orig_sxg_image)
		: iconObj(orig_sxg_image),
		current_sxg_thread_only(orig_sxg),
		current_image_thread_only(orig_sxg_image),
		name(name),
		width(width),
		height(height)
		{
		}

	//! Destructor
	~sxg_iconObj()=default;

	//! Check if a new theme is installed, if so create a replacement icon.

	icon theme_updated(IN_THREAD_ONLY) override;

	icon resizemm(IN_THREAD_ONLY, double widthmm, double heightmm) override
	{
		return create_sxg_image(name,
					&*current_image(IN_THREAD)
					->icon_pixmap->impl,
					current_sxg(IN_THREAD),
					current_image(IN_THREAD)->repeat,
					widthmm,
					heightmm);
	}

	icon resize(IN_THREAD_ONLY, dim_t w, dim_t h) override
	{
		return create_sxg_image(name,
					&*current_image(IN_THREAD)
					->icon_pixmap->impl,
					current_sxg(IN_THREAD),
					current_image(IN_THREAD)->repeat,
					w,
					h);
	}
};

//! Here's a parsed SXG image, the drawable it's for, and its dimensions.

//! Calculate the actual size, in pixels, and return a rendered image from
//! the SXG file. The returned sxg_images are cached.

static auto get_cached_sxg_image(const sxg_parser &sxg,
				 drawableObj::implObj *drawable_impl,
				 render_repeat repeat,
				 dim_t w,
				 dim_t h)
{
	return sxg->screenref->impl->iconcaches
		->sxg_image_cache->find_or_create
		({sxg, drawable_impl->drawable_pictformat, repeat, w, h},
		 [&]
		 {
			 auto pixmap=drawable_impl->create_pixmap(w, h);
			 auto picture=pixmap->create_picture();

			 auto ri=sxg_image::create(picture, pixmap, repeat);

			 sxg->render(picture, pixmap, ri->points);

			 return ri;
		 });
}

static icon create_sxg_image(const std::experimental::string_view &name,
			     drawableObj::implObj *drawable_impl,
			     const sxg_parser &sxg,
			     render_repeat repeat,
			     double widthmm,
			     double heightmm)
{
	dim_t w=0;
	dim_t h=0;

	if (widthmm <= 0 && heightmm <= 0)
	{
		w=sxg->default_width();
		h=sxg->default_height();
	}
	else
	{
		w=widthmm <= 0 ? 0 : sxg->width_for_mm(widthmm);
		h=heightmm <= 0 ? 0 : sxg->height_for_mm(heightmm);

		if (w == 0)
			w=sxg->width_for_height(h, icon_scale::nearest);

		if (h == 0)
			h=sxg->height_for_width(w, icon_scale::nearest);
	}

	auto image=get_cached_sxg_image(sxg, drawable_impl, repeat, w, h);

	return ref<sxg_iconObj<double>>::create(name,
						widthmm,
						heightmm,
						sxg, image);
}

static icon create_sxg_image(const std::experimental::string_view &name,
			     drawableObj::implObj
			     *drawable_impl,
			     const sxg_parser &sxg,
			     render_repeat repeat,
			     dim_t w, dim_t h)
{
	if (w == 0 && h == 0)
	{
		w=sxg->default_width();
		h=sxg->default_height();
	}
	else
	{
		if (w == 0)
			w=sxg->width_for_height(h, icon_scale::nearest);

		if (h == 0)
			h=sxg->height_for_width(w, icon_scale::nearest);
	}

	auto image=get_cached_sxg_image(sxg, drawable_impl, repeat, w, h);

	return ref<sxg_iconObj<dim_t>>::create(name,
					       w,
					       h,
					       sxg, image);
}

static icon
create_sxg_icon_from_filename_mm(const std::experimental
				 ::string_view &name,
				 const std::string &filename,
				 drawableObj::implObj *for_drawable,
				 const screen &screenref,
				 const defaulttheme &theme,
				 render_repeat icon_repeat,
				 double widthmm,
				 double heightmm)
{
	auto sxg=screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({filename, theme},
		 [&]
		 {
			 return sxg_parser::create(filename, screenref,
						   theme);
		 });

        return create_sxg_image(name, for_drawable, sxg, icon_repeat,
				widthmm, heightmm);
}

static icon
create_sxg_icon_from_filename(const std::experimental::string_view &name,
			      const std::string &filename,
			      drawableObj::implObj *for_drawable,
			      const screen &screenref,
			      const defaulttheme &theme,
			      render_repeat icon_repeat,
			      dim_t w, dim_t h)
{
	auto sxg=screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({filename, theme},
		 [&]
		 {
			 return sxg_parser::create(filename, screenref,
						   theme);
		 });

        return create_sxg_image(name, for_drawable, sxg, icon_repeat, w, h);
}

static const struct {
	const char *extension;

	icon (*create_mm)(const std::experimental::string_view &name,
			  const std::string &filename,
			  drawableObj::implObj *for_drawable,
			  const screen &screenref,
			  const defaulttheme &theme,
			  render_repeat icon_repeat,
			  double widthmm,
			  double heightmm);

	icon (*create)(const std::experimental::string_view &name,
		       const std::string &filename,
		       drawableObj::implObj *for_drawable,
		       const screen &screenref,
		       const defaulttheme &theme,
		       render_repeat icon_repeat,
		       dim_t w, dim_t h);
} extensions[]={
	{".sxg", &create_sxg_icon_from_filename_mm,
	 &create_sxg_icon_from_filename},
};

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

	}

	return false;
}

icon drawableObj::implObj
::create_icon_mm(const std::experimental::string_view &name,
		 render_repeat icon_repeat,
		 double widthmm,
		 double heightmm)
{
	auto screen=get_screen();
	auto theme=*current_theme_t::lock{screen->impl->current_theme};

	size_t p=name.rfind('/');

	if (p == name.npos)
		p=0;
	p=name.find('.', p);

	bool found_extension=false;

	for (const auto &filetype:extensions)
	{
		if (p != name.npos)
		{
			if (name.substr(p) == filetype.extension)
			{
				std::string n{name};

				found_extension=true;

				if (!search_file(n, theme))
					break;

				return (*filetype.create_mm)
					(name, n, this, screen, theme,
					 icon_repeat,
					 widthmm, heightmm);
			}
			continue;
		}

		std::string n{name};

		n += filetype.extension;

		if (!search_file(n, theme))
			continue;

		return (*filetype.create_mm)
			(name, n, this, screen, theme,
			 icon_repeat,
			 widthmm, heightmm);
	}

	if (p != name.npos && !found_extension)
		throw EXCEPTION("Unsupported file format: " << name);

	throw EXCEPTION(name << " not found");
}

icon drawableObj::implObj
::create_icon(const std::experimental::string_view &name,
	      render_repeat icon_repeat,
	      dim_t w, dim_t h)
{
	auto screen=get_screen();
	auto theme=*current_theme_t::lock{screen->impl->current_theme};

	size_t p=name.rfind('/');

	if (p == name.npos)
		p=0;
	p=name.find('.', p);

	bool found_extension=false;

	for (const auto &filetype:extensions)
	{
		if (p != name.npos)
		{
			if (name.substr(p) == filetype.extension)
			{
				std::string n{name};

				found_extension=true;

				if (!search_file(n, theme))
					break;

				return (*filetype.create)
					(name, n, this, screen, theme,
					 icon_repeat, w, h);
			}
			continue;
		}

		std::string n{name};

		n += filetype.extension;

		if (!search_file(n, theme))
			continue;

		return (*filetype.create)
			(name, n, this, screen, theme,
			 icon_repeat, w, h);
	}

	if (p != name.npos && !found_extension)
		throw EXCEPTION("Unsupported file format: " << name);

	throw EXCEPTION(name << " not found");
}

template<>
icon sxg_iconObj<double>::theme_updated(IN_THREAD_ONLY)
{
	auto theme= *current_theme_t::lock{current_sxg(IN_THREAD)
					   ->screenref->impl
					   ->current_theme};

	if (theme == current_sxg(IN_THREAD)->theme)
		return icon(this); // Unchanged

	// All right, take it from the top.
	auto icon=image->icon_pixmap->impl
		->create_icon_mm(name,
				 image->repeat,
				 width,
				 height);

	// We technically need to call initialize(), hopefully a mere
	// formality.
	return icon->initialize(IN_THREAD);
}

template<>
icon sxg_iconObj<dim_t>::theme_updated(IN_THREAD_ONLY)
{
	auto theme= *current_theme_t::lock{current_sxg(IN_THREAD)
					   ->screenref->impl
					   ->current_theme};

	if (theme == current_sxg(IN_THREAD)->theme)
		return icon(this); // Unchanged

	// All right, take it from the top.
	auto icon=image->icon_pixmap->impl
		->create_icon(name,
			      image->repeat,
			      width, height);

	// We technically need to call initialize(), hopefully a mere
	// formality.
	return icon->initialize(IN_THREAD);
}

template icon sxg_iconObj<double>::resizemm(IN_THREAD_ONLY, double, double);
template icon sxg_iconObj<double>::resize(IN_THREAD_ONLY, dim_t, dim_t);
template icon sxg_iconObj<dim_t>::resizemm(IN_THREAD_ONLY, double, double);
template icon sxg_iconObj<dim_t>::resize(IN_THREAD_ONLY, dim_t, dim_t);

LIBCXXW_NAMESPACE_END
