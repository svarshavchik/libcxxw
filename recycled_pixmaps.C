/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "recycled_pixmaps.H"
#include "x/w/impl/scratch_buffer.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/icon.H"
#include "x/w/impl/pixmap_with_picture.H"
#include "screen.H"
#include "x/w/impl/element.H"
#include "x/w/pictformat.H"
#include "x/w/rgb_hash.H"
#include "pixmap.H"
#include "defaulttheme.H"
#include "picture.H"
#include "xid_t.H"
#include "connection_thread.H"
#include "messages.H"
#include <x/ref.H>
#include <x/refptr_hash.H>
#include <x/weakunordered_multimap.H>
#include <x/visitor.H>
#include <variant>
#include <cmath>

LIBCXXW_NAMESPACE_START

recycled_pixmapsObj::recycled_pixmapsObj()
	: scratch_buffer_cache{scratch_buffer_cache_t::create()},
	  theme_background_color_cache{
		  theme_background_color_cache_t::create()},
	  nontheme_background_color_cache{
		  nontheme_background_color_cache_t::create()},
	  gradient_cache{gradient_cache_t::create()},
	  image_background_color_cache{image_background_color_cache_t::create()
	  },
	  element_specific_image_background_color_cache
	{
	 element_specific_image_background_color_cache_t::create()
	}
{
}

recycled_pixmapsObj::~recycled_pixmapsObj()=default;

scratch_buffer screenObj::create_scratch_buffer(const std::string &identifier,
						const const_pictformat &pf)
{
	return impl->create_scratch_buffer(screen(this),
					   identifier,
					   pf);
}

scratch_buffer screenObj::implObj
::create_scratch_buffer(const screen &public_object,
			const std::string &identifier,
			const const_pictformat &pf)
{
	return recycled_pixmaps_cache->scratch_buffer_cache
		->find_or_create({ identifier, pf },
				 [&]
				 {
					 auto i=ref<scratch_bufferObj::implObj>
						 ::create(pf,
							  public_object);

					 return scratch_buffer::create(i);
				 });
}

namespace {
#if 0
}
#endif

////////////////////////////////////////////////////////////////////////////
//
// Gradient background color.
//
// A gradient background color gets initially constructed as a solid color
// background object. When the size of the gradient's element is known,
// its get_background_color_for() construct this linear_gradient_color
// object, which becomes the background color.
//
// The gradient background color object saved the background_color it was
// created from, which may be a theme, or a nontheme color, see below.
// Other background_color methods get forwarded to the base_color.

class LIBCXX_HIDDEN gradient_colorObj : public background_colorObj {

	//! The gradient's picture.

	const const_picture gradient_color;

	//! The base color this gradient was created from.
	const background_color base_color;

public:

	//! Constructor
	gradient_colorObj(const const_picture &gradient_color,
			  const background_color &base_color)
		: background_colorObj{base_color->background_color_screen},
		gradient_color{gradient_color},
		base_color{base_color}
	{
	}

	//! Destructor
	~gradient_colorObj()=default;

	bool is_scrollable_background() override
	{
		return false;
	}

	const_picture get_current_color(ONLY IN_THREAD) override
	{
		return gradient_color;
	}

	//! Always a no-op

	void current_theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
		override
	{
	}

	//! Forward get_background_color_for to the base background color.

	background_color get_background_color_for(ONLY IN_THREAD,
						  elementObj::implObj &e,
						  dim_t width,
						  dim_t height)
		override
	{
		return base_color->get_background_color_for(IN_THREAD, e,
							    width, height);
	}
};

////////////////////////////////////////////////////////////////////////////
// Implement background_color object as a non theme-dependent color, that's
// specified either as a plain rgb value, or a gradient.

class nontheme_background_colorObj : public background_colorObj {

protected:
	// The background color specifier
	typedef std::variant<rgb, linear_gradient,
			     radial_gradient, const_picture> color_t;

	color_t color;

	// The background color picture

	const_picture fixed_color;

	// Updated fixed_color based on color

	const_picture create_fixed_color(const screen &s)
	{
		return std::visit([&, this](const auto &c)
				  {
					  return create_fixed_color(c, s);
				  }, color);
	}

public:
	static inline const_picture
	create_fixed_color(const rgb &r,
			   const screen &s)
	{
		return s->impl->create_solid_color_picture(r);
	}

	// Before we know the dimensions of the
	// gradient, the opening bid is to
	// create the first gradient color.
	//
	// This will not go to waste. When
	// we create the real gradient, this
	// color will already be cached.

	static inline const_picture
	create_fixed_color(const linear_gradient &g,
			   const screen &s)
	{
		auto iter=g.gradient.find(0);

		if (iter == g.gradient.end())
			throw EXCEPTION("Internal error: "
					"invalid gradient parameter");

		return s->impl->create_solid_color_picture(iter->second);
	}

	static inline const_picture
	create_fixed_color(const radial_gradient &g,
			   const screen &s)
	{
		auto iter=g.gradient.find(0);

		if (iter == g.gradient.end())
			throw EXCEPTION("Internal error: "
					"invalid gradient parameter");

		return s->impl->create_solid_color_picture(iter->second);
	}

	static inline const_picture
	create_fixed_color(const const_picture &c,
			   const screen &s)
	{

		return c;
	}

	template<typename Arg>
	nontheme_background_colorObj(Arg &&arg,
				     const screen &s)
		: background_colorObj{s},
		  color{std::forward<Arg>(arg)},
		  fixed_color{create_fixed_color(s)}
	{
	}

	~nontheme_background_colorObj()=default;

	const_picture get_current_color(ONLY IN_THREAD) override
	{
		return fixed_color;
	}

	bool is_scrollable_background() override
	{
		return std::holds_alternative<rgb>(color);
	}

	void current_theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
		override
	{
	}


private:

	inline background_color make_gradient(ONLY IN_THREAD,
					      elementObj::implObj &e,
					      dim_t e_width,
					      dim_t e_height,
					      const linear_gradient &g)
	{
		auto [x, y, width, height]=
			get_gradient_params(IN_THREAD, e_width, e_height,
					    background_color_screen->impl,
					    g.fixed_width,
					    g.fixed_height);

		auto picture=background_color_screen->impl
			->create_linear_gradient_picture
			(g, x, y, width, height, render_repeat::pad);

#ifdef DEBUG_LINEAR_GRADIENT_CREATED
		DEBUG_LINEAR_GRADIENT_CREATED();
#endif

		return create_new_gradient_background_color
			(background_color_screen,
			 ref{this},
			 picture);
	}

	inline background_color make_gradient(ONLY IN_THREAD,
					      elementObj::implObj &e,
					      dim_t e_width,
					      dim_t e_height,
					      const radial_gradient &g)
	{
		auto [x, y, width, height]=
			get_gradient_params(IN_THREAD,
					    e_width, e_height,
					    background_color_screen->impl,
					    g.fixed_width,
					    g.fixed_height);

		auto picture=background_color_screen->impl->create_radial_gradient_picture
			(g, x, y, width, height, render_repeat::pad);

		return create_new_gradient_background_color
			(background_color_screen,
			 ref{this},
			 picture);
	}

public:

	background_color get_background_color_for(ONLY IN_THREAD,
						  elementObj::implObj &e,
						  dim_t width, dim_t height)
		override
	{
		return std::visit(visitor{
				[&, this](const rgb &)
				{
					return background_color{this};
				},
				[&, this](const const_picture &)
				{
					return background_color{this};
				},
				[&, this](const linear_gradient &g)
				{
					return make_gradient(IN_THREAD, e,
							     width, height,
							     g);
				},
				[&, this](const radial_gradient &g)
				{
					return make_gradient(IN_THREAD, e,
							     width, height,
							     g);
				}}, color);
	}

private:
	static std::tuple<coord_t, coord_t, dim_t, dim_t
			  > get_gradient_params(ONLY IN_THREAD,
						dim_t e_width,
						dim_t e_height,
						const ref<screenObj::implObj>
						&s,
						double fixed_width,
						double fixed_height)
	{
		dim_t width=e_width;
		dim_t height=e_height;
		// Negative values mean from the opposite side.

		bool from_right=false;
		bool from_bottom=false;
		coord_t x=0;
		coord_t y=0;

		if (fixed_width < 0)
		{
			fixed_width=-fixed_width;
			from_right=true;
		}

		if (fixed_height < 0)
		{
			fixed_height=-fixed_height;
			from_bottom=true;
		}
		if (fixed_width > 0 || fixed_height > 0)
		{
			current_theme_t::lock lock{s->current_theme};

			if (fixed_width > 0)
				width= (*lock)->compute_width(fixed_width);

			if (fixed_height > 0)
				height= (*lock)->compute_height(fixed_height);
		}

		if (from_right)
			x=coord_t::truncate
				(coord_t::truncate(e_width)
				 -coord_t::truncate(width));
		if (from_bottom)
			y=coord_t::truncate
				(coord_t::truncate(e_height)
				 -coord_t::truncate(height));

		return {x, y, width, height};
	}
};

////////////////////////////////////////////////////////////////////////////
//
// Implements background_color object as a color specified by the theme.
//
// The current background color picture is cached. Each call to
// get_current_color() retrieves the color from theme, which is just an
// unordered_map lookup, compares it to the cached color, and creates a
// new picture, if necessary.

class theme_background_colorObj : public nontheme_background_colorObj {

	const std::string theme_color;

 public:

	// The constructor gets initializes with the current background color

	theme_background_colorObj(const std::string &theme_color,
				  const screen &s,
				  const const_defaulttheme &current_theme)
		: nontheme_background_colorObj
		{std::visit([&](const auto &c)
			    {
				    return color_t{c};
			    }, current_theme->get_theme_color(theme_color)),
		 s},
		  theme_color{theme_color}
	{
	}

	~theme_background_colorObj()=default;

	void current_theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override
	{
		std::visit([&, this]
			   (const auto &c)
			   {
				   this->color=c;
			   }, new_theme->get_theme_color(theme_color));

		fixed_color=create_fixed_color(background_color_screen);
	}
};

//////////////////////////////////////////////////////////////////////////
//
// A background color based on an icon.

class icon_background_colorObj : public background_colorObj {

	icon icon_handle;
	bool initialized=false;
public:
	const icon &current_icon(ONLY IN_THREAD);

	const icon_scale scale;

	icon_background_colorObj(const icon &current_icon,
				 icon_scale scale,
				 const screen &s);

	~icon_background_colorObj();

	//! Override current_theme_updated

	void current_theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
		override;

	background_color get_background_color_for(ONLY IN_THREAD,
						  elementObj::implObj &e,
						  dim_t width,
						  dim_t height) override;

	bool is_scrollable_background() override;

	const_picture get_current_color(ONLY IN_THREAD) override;

	virtual background_color get_base_background_color()
	{
		return ref{this};
	}
};

///////////////////////////////////////////////////////////////////////////
//
// An icon background color that's specific to an icon.
//
class element_specific_icon_background_colorObj
	: public icon_background_colorObj {

public:
	const ref<icon_background_colorObj> base_color;

	element_specific_icon_background_colorObj
	(const ref<icon_background_colorObj> &base_color,
	 const icon &current_icon);

	~element_specific_icon_background_colorObj();

	background_color get_base_background_color() override
	{
		return base_color;
	}
};

icon_background_colorObj::icon_background_colorObj(const icon &current_icon,
						   icon_scale scale,
						   const screen &s)
	: background_colorObj{s},
	  icon_handle{current_icon},
	  scale{scale}
{
}

icon_background_colorObj::~icon_background_colorObj()=default;

const icon &icon_background_colorObj::current_icon(ONLY IN_THREAD)
{
	if (!initialized)
	{
		initialized=true;
		icon_handle=icon_handle->initialize(IN_THREAD);
	}
	return icon_handle;
}

void icon_background_colorObj
::current_theme_updated(ONLY IN_THREAD,
			const const_defaulttheme &new_theme)
{
	icon_handle=current_icon(IN_THREAD)
		->theme_updated(IN_THREAD, new_theme);
}

background_color icon_background_colorObj
::get_background_color_for(ONLY IN_THREAD,
			   elementObj::implObj &e,
			   dim_t width,
			   dim_t height)
{
	if (current_icon(IN_THREAD)->image->repeat != render_repeat::none)
		return background_color{this};

	auto base=get_base_background_color();

	return background_color_screen->impl->recycled_pixmaps_cache
		->element_specific_image_background_color_cache
		->find_or_create
		({base, width, height},
		 [&, this]
		 {
			 auto resized_icon=
				 current_icon(IN_THREAD)
				 ->resize(IN_THREAD,
					  width, height, scale);

			 return ref<element_specific_icon_background_colorObj>
				 ::create(base, resized_icon);
		 });
}

bool icon_background_colorObj::is_scrollable_background()
{
	return false;
}

const_picture icon_background_colorObj::get_current_color(ONLY IN_THREAD)
{
	return current_icon(IN_THREAD)->image->icon_picture;
}


element_specific_icon_background_colorObj
::element_specific_icon_background_colorObj
(const ref<icon_background_colorObj> &base_color,
 const icon &current_icon)
	: icon_background_colorObj{current_icon,
				   base_color->scale,
				   base_color->background_color_screen},
	  base_color{base_color}
{
}

element_specific_icon_background_colorObj
::~element_specific_icon_background_colorObj()=default;


#if 0
{
#endif
}

background_color create_new_background_color(const screen &s,
					     const const_pictformat &pf,
					     const color_arg &color_name)
{
	// We lock the current theme for the duration of this.

	current_theme_t::lock lock{s->impl->current_theme};

	return s->impl->recycled_pixmaps_cache->theme_background_color_cache
		->find_or_create
		(color_name,
		 [&]
		 {
			 return std::visit(visitor{
				 [&](const std::string &name)
				 ->background_color
				 {
					 return ref<theme_background_colorObj>
						 ::create(name, s, *lock);
				 },
				 [&](const image_color &ic)
				 ->background_color
				 {
					 return s->impl->recycled_pixmaps_cache
						 ->image_background_color_cache
						 ->find_or_create
						 ({ic, pf},
						  [&]
						  {
							  auto i=create_new_icon
								  (s, pf, ic);

							  return ref<icon_background_colorObj>
								  ::create(i, ic.scale, s);
						  });
				 },
				 [&](const auto &nontheme_color)
					 ->background_color
				 {
					 return ref<nontheme_background_colorObj
						    >::create(nontheme_color,
							      s);
				 }}, color_name);
		 });
}


background_color create_new_background_color(const screen &s,
					     const const_pictformat &pf,
					     const const_picture &pic)
{
	if (pic->impl->picture_xid.thread() != s->impl->thread)
		throw EXCEPTION(_("Attempt to set a background color picture from a different screen."));

	return s->impl->recycled_pixmaps_cache->nontheme_background_color_cache
		->find_or_create
		(pic,
		 [&]
		 {
			 return ref<nontheme_background_colorObj>
				 ::create(pic, s);
		 });
}

background_color
create_new_gradient_background_color(const screen &s,
				     const background_color &base_color,
				     const const_picture &p)
{
	return s->impl->recycled_pixmaps_cache->gradient_cache->find_or_create
		({base_color, p},
		 [&]
		 {
			 return ref<gradient_colorObj>::create(p, base_color);
		 });
}

/////////////////////////////////////////////////////////////////////////////

bool recycled_pixmapsObj
::scratch_buffer_key::operator==(const scratch_buffer_key &o) const noexcept
{
	return identifier == o.identifier && pf == o.pf;
}

size_t recycled_pixmapsObj
::scratch_buffer_key_hash::operator()(const scratch_buffer_key &k)
	const noexcept
{
	return std::hash<std::string>()(k.identifier)
		+ std::hash<const_pictformat>()(k.pf);
}


bool recycled_pixmapsObj::gradient_key
::operator==(const gradient_key &o) const noexcept
{
	return base_background == o.base_background &&
		gradient_picture == o.gradient_picture;
}

size_t recycled_pixmapsObj::gradient_key_hash
::operator()(const gradient_key &k) const noexcept
{
	return std::hash<background_color>::operator()(k.base_background) +
		std::hash<const_picture>::operator()(k.gradient_picture);
}

size_t recycled_pixmapsObj::image_color_key_hash
::operator()(const image_color_key &v) const noexcept
{
	return std::hash<image_color>::operator()(v.key_image_color) +
		std::hash<const_pictformat>::operator()(v.key_pictformat);
}

size_t recycled_pixmapsObj::element_specific_image_key_hash
::operator()(const element_specific_image_key &v) const noexcept
{
	return std::hash<background_color>::operator()(v.key_background_color)+
		std::hash<dim_t>::operator()(v.width) +
		std::hash<dim_t>::operator()(v.height);
}

LIBCXXW_NAMESPACE_END
