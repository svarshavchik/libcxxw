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
	const_pictformat    drawable_pictformat;
	render_repeat       repeat;
	dim_t               width;
	dim_t               height;

	bool operator==(const sxg_image_cache_key &o) const;
};

bool icon_cacheObj::sxg_image_cache_key
::operator==(const sxg_image_cache_key &o) const
{
	return drawable_pictformat == o.drawable_pictformat &&
		repeat == o.repeat &&
		width == o.width &&
		height == o.height;
}

struct icon_cacheObj::sxg_image_cache_key_hash
	: public std::hash<const_pictformat>,
	  public std::hash<dim_t> {

	size_t operator()(const sxg_image_cache_key &k) const
	{
		return std::hash<const_pictformat>::operator()
			(k.drawable_pictformat) ^ (size_t)k.repeat
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

sxg_parser get_sxg(const std::experimental::string_view &filename,
		   const screen &screenref,
		   const defaulttheme &theme)
{
	std::string f{filename};

	if (f.find('/') == f.npos && access(f.c_str(), R_OK))
	{
		f=theme->themedir + "/" + f;

		if (filename.find('.') == filename.npos)
			f += ".sxg";
	}
	return screenref->impl->iconcaches->sxg_parser_cache->find_or_create
		({f, theme},
		 [&]
		 {
			 return sxg_parser::create(f, screenref, theme);
		 });
}

///////////////////////////////////////////////////////////////////////

//! An SXG-based icon.

class LIBCXX_HIDDEN sxg_mmiconObj : public iconObj {

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
	const double widthmm;

	//! Original requested height in millimeters
	const double heightmm;

	//! Constructor
	sxg_mmiconObj(const std::experimental::string_view &name,
		      double widthmm,
		      double heightmm,
		      const sxg_parser &orig_sxg,
		      const const_sxg_image &orig_sxg_image);

	//! Destructor
	~sxg_mmiconObj();

	//! Check if a new theme is installed, if so create a replacement icon.

	icon theme_updated(IN_THREAD_ONLY) override;
};

//! Here's a parsed SXG image, the drawable it's for, and its dimensions.

//! Calculate the actual size, in pixels, and return a rendered image from
//! the SXG file. The returned sxg_images are cached.

static inline const_sxg_image create_sxg_image(drawableObj::implObj
					       *drawable_impl,
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

	return sxg->screenref->impl->iconcaches
		->sxg_image_cache->find_or_create
		({drawable_impl->drawable_pictformat, repeat, w, h},
		 [&]
		 {
			 auto pixmap=drawable_impl->create_pixmap(w, h);
			 auto picture=pixmap->create_picture();

			 auto ri=sxg_image::create(pixmap, picture, repeat);

			 sxg->render(picture, pixmap, ri->points);

			 return ri;
		 });
}

icon drawableObj::implObj
::create_icon_mm(const std::experimental::string_view &name,
		 render_repeat icon_repeat,
		 double widthmm,
		 double heightmm)
{
	auto screen=get_screen();

	auto theme=*current_theme_t::lock{screen->impl->current_theme};

	return screen->impl->iconcaches->mm_image_cache->find_or_create
		({std::string(name), theme,
				drawable_pictformat,
				icon_repeat, widthmm, heightmm},
			[&, this]
			{
				auto sxg=get_sxg(name, screen, theme);

				auto image=create_sxg_image(this, sxg,
							    icon_repeat,
							    widthmm, heightmm);

				return ref<sxg_mmiconObj>::create(name,
								  widthmm,
								  heightmm,
								  sxg, image);
			});
}

sxg_mmiconObj::sxg_mmiconObj(const std::experimental::string_view &name,
			     double widthmm,
			     double heightmm,
			     const sxg_parser &orig_sxg,
			     const const_sxg_image &orig_sxg_image)
		: iconObj(orig_sxg_image),
		current_sxg_thread_only(orig_sxg),
		current_image_thread_only(orig_sxg_image),
		name(name),
		widthmm(widthmm),
		heightmm(heightmm)
{
}

sxg_mmiconObj::~sxg_mmiconObj()=default;


icon sxg_mmiconObj::theme_updated(IN_THREAD_ONLY)
{
	auto theme= *current_theme_t::lock{current_sxg(IN_THREAD)
					   ->screenref->impl
					   ->current_theme};

	if (theme == current_sxg(IN_THREAD)->theme)
		return icon(this); // Unchanged

	// All right, take it from the top.
	auto icon=image(IN_THREAD)->icon_pixmap->impl
		->create_icon_mm(name,
				 image(IN_THREAD)->repeat,
				 widthmm,
				 heightmm);

	// We technically need to call initialize(), hopefully a mere
	// formality.
	return icon->initialize(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
