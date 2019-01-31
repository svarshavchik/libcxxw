/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/element.H>
#include <x/w/font.H>

#include <x/w/impl/child_element.H>
#include <x/w/impl/background_color_element.H>
#include <x/w/impl/scratch_buffer_draw.H>
#include <x/w/impl/container.H>
#include <x/w/impl/fonts/current_fontcollection.H>
#include <x/w/impl/fonts/fontcollection.H>
#include <x/w/impl/fonts/composite_text_stream.H>
#include <x/w/impl/theme_font_element.H>
#include <x/w/impl/themedim_element.H>

#include <string>
#include <iostream>
#include <optional>

#include "close_flag.H"

struct color1_tag;
struct color2_tag;

static auto create_child_element_init_params(x::w::dim_t width,
					     x::w::dim_t ascender,
					     x::w::dim_t descender,
					     x::w::dim_t shadow)
{
	x::w::child_element_init_params init_params;

	init_params.scratch_buffer_id=
		"customfontrenderer2@examples.w.libcxx.com";

	x::w::dim_t height=x::w::dim_t::truncate(ascender+descender);

	width=x::w::dim_t::truncate(width+shadow);
	height=x::w::dim_t::truncate(height+shadow);
	init_params.initial_metrics={
				     {width, width, width},
				     {height, height, height}
	};

	return init_params;
}

// Compute the size of our custom element based on the current fontcollection.

static void compute_metrics(const x::w::fontcollection &title_fontcollection,
			    const std::u32string &label,
			    x::w::dim_t &max_ascender,
			    x::w::dim_t &max_descender,
			    x::w::dim_t &width)
{
	// Load the glyphs for our unicode characters.
	title_fontcollection->load_glyphs(label.begin(), label.end(), ' ');

	// Compute font height above+below the baseline.
	max_ascender=0;
	max_descender=0;
	width=0;

	title_fontcollection->lookup(label.begin(), label.end(),
				     [&]
				     (const auto &,
				      const auto &,
				      const x::w::freetypefont &f)
				     {
					     if (f->ascender > max_ascender)
						     max_ascender=f->ascender;
					     if (f->descender > max_descender)
						     max_descender=f->descender;
				     },
				     ' ');

	// Compute the text's width.
	title_fontcollection->glyphs_size_and_kernings
		(label.begin(),
		 label.end(),
		 [&]
		 (x::w::dim_t c_width,
		  x::w::dim_t c_height,
		  int16_t x_kerning,
		  int16_t y_kerning)
		 {
			 width=x::w::dim_t::truncate(width+c_width+x_kerning);
			 return true;
		 },
		 0,
		 ' ');
}

// The implementation class of our custom element. Uses the following
// mixins:
//
// - scratch_buffer_draw, to give us a simplified draw().
//
// - font_element, to maintain and update the current font, in response to
// theme changes.
//
// - themedim_elementObj, to maintain the size of the shadow.
//
// - background_color_elementObj, to give us the two colors we'll use for
// our custom-rendered text: the main color, and the shadow color.

class my_font_renderer_implObj : public x::w::scratch_buffer_draw<
	x::w::theme_font_elementObj<x::w::themedim_elementObj<
					    x::w::background_color_elementObj
					    <x::w::child_elementObj,
					     color1_tag, color2_tag>>>> {

	// Alias for the superclass.

	typedef x::w::scratch_buffer_draw<
		x::w::theme_font_elementObj<x::w::themedim_elementObj
					    <x::w::background_color_elementObj
					     <x::w::child_elementObj,
					      color1_tag, color2_tag>>>
		> superclass_t;

	// The text.
	const std::u32string title;

	// The fontcollection for drawing the text.

	// Save a copy of the fontcollection that was used to draw the title,
	// so we can compare notes when the current display theme changes.
	x::w::fontcollection title_fontcollection;

	// Saved metrics, for convenience.
	x::w::dim_t width;
	x::w::dim_t ascender;
	x::w::dim_t descender;
	x::w::dim_t shadow;

public:

	my_font_renderer_implObj(const x::w::container_impl &parent_container,
				 const std::u32string &title,
				 const x::w::current_fontcollection
				 &current_title_fontcollection,
				 const x::w::fontcollection
				 &title_fontcollection,
				 const x::w::themedim_element_init &shadow_init,
				 x::w::dim_t width,
				 x::w::dim_t ascender,
				 x::w::dim_t descender)
		: superclass_t
		{
		 // Initial font
		 current_title_fontcollection,

		 // Shadow dimension
		 shadow_init,

		 // Background color will be a linear gradient
		 x::w::black,
		 x::w::silver,

		 // Finally, the parameters to child_elementObjs constructor:
		 parent_container,
		 create_child_element_init_params(width, ascender, descender,
						  shadow_init.pixels)},

		  title{title},
		  title_fontcollection{title_fontcollection},
		  width{width},
		  ascender{ascender},
		  descender{descender},
		  shadow{shadow_init.pixels}
	{
	}

	~my_font_renderer_implObj()=default;

	void do_draw(ONLY IN_THREAD,
		     const x::w::draw_info &di,
		     const x::w::picture &area_picture,
		     const x::w::pixmap &area_pixmap,
		     const x::w::gc &area_gc,
		     const x::w::clip_region_set &clipped,
		     const x::w::rectangle &area_entire_rect) override;

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD,
			   const x::w::defaulttheme &current_theme)
		override;

private:

	void update(ONLY IN_THREAD);
};

typedef x::ref<my_font_renderer_implObj> my_font_renderer_impl;

// Override initialize(). initialize() typically gets invoked by the
// internal library connection thread after the object gets constructed and
// added to its container.
//
// initialize is, usually, the first IN_THREAD method that gets invoked,
// but custom display element should not rely on this 100%.
//
// The purpose of initialize() is to verify that any "assumptions" that were
// made, when creating the display element in the main execution thread, are
// still valid.
//
// The main execution thread used the then-current font, to compute this
// custom display element's metrics. We're going to re-check to make sure
// that the current display theme hasn't changed since then, and the font
// is still the same. Extremely unlikely that this won't be, it's a very
// rare race condition, but I'm a stickler for details.
//
// Overridden initialize() must invoke its superclass's initialize(), first.

void my_font_renderer_implObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	update(IN_THREAD);
}

// Display theme definitely changed.

// The overriden theme_updated() must invoke its superclass's
// theme_updated() first.

void my_font_renderer_implObj::theme_updated(ONLY IN_THREAD,
					     const x::w::defaulttheme
					     &current_theme)
{
	superclass_t::theme_updated(IN_THREAD, current_theme);
	update(IN_THREAD);
}

void my_font_renderer_implObj::update(ONLY IN_THREAD)
{
	// Retrieve the current font, from the font_element mixin.

	x::w::current_fontcollection current_title_fontcollection=
		this->current_font(IN_THREAD);

	x::w::fontcollection new_title_fontcollection=
		current_title_fontcollection->fc(IN_THREAD);

	auto current_shadow=this->pixels(IN_THREAD); // themedim_elementObj

	// Ok, has our font, or shadow size, changed?

	if (new_title_fontcollection == title_fontcollection &&
	    shadow == current_shadow)
		return;

	title_fontcollection=new_title_fontcollection;
	shadow=current_shadow;

	// We need to recompute our metrics, with respect to the new font.
	compute_metrics(title_fontcollection, title, ascender, descender,
			width);

	// Recycle create_child_element_init_params(), to conveniently compute
	// our new metrics.

	auto init_params=create_child_element_init_params(width, ascender,
							  descender,
							  shadow);

	// Retrieve this display element's metrics object, and update it.

	// This updates our custom element's display metrics to the size
	// of the updated font, in the new theme. The display element gets
	// resized and redrawn.
	get_horizvert(IN_THREAD)->set_element_metrics
		(IN_THREAD,
		 init_params.initial_metrics.horiz,
		 init_params.initial_metrics.vert);
}

// Draw the text, and its shadow.

void my_font_renderer_implObj::do_draw(ONLY IN_THREAD,
				       const x::w::draw_info &di,
				       const x::w::picture &area_picture,
				       const x::w::pixmap &area_pixmap,
				       const x::w::gc &area_gc,
				       const x::w::clip_region_set &clipped,
				       const x::w::rectangle &area_entire_rect)
{
	for (size_t i=x::w::dim_t::value_type(shadow); i > 0; )
	{
		--i;

		x::w::coord_t x=i;
		x::w::coord_t y=x::w::coord_t::truncate(ascender+i);

		std::optional<x::w::composite_text_stream> composited_text;

		title_fontcollection->lookup
			(title.begin(), title.end(),
			 [&]
			 (const std::u32string::const_iterator &beg_iter,
			  const std::u32string::const_iterator &end_iter,
			  const x::w::freetypefont &f)
			 {
				 if (!composited_text)
					 composited_text.emplace(f,
								 title.size(),
								 0);

				 f->glyphs_to_stream(beg_iter, end_iter,
						     *composited_text,
						     x, y,
						     0,
						     U' ');
			 },
			 ' ');

		if (!composited_text)
			continue; // Shouldn't happen, only happens if empty.

		// Use shadow color, or the final color.
		auto color=
			i == 0 ? background_color_element<color1_tag>
			::get(IN_THREAD)
			: background_color_element<color2_tag>
			::get(IN_THREAD);

		composited_text->composite(area_picture,
					   color->get_current_color(IN_THREAD),
					   0, 0);
	}
}

// "Public" object subclasses the public element object. Not really needed
// here, just for completeness sake.

class my_font_rendererObj : public x::w::elementObj {

public:

	const my_font_renderer_impl impl; // My implementation object.

	// Constructor

	my_font_rendererObj(const my_font_renderer_impl &impl)
		: x::w::elementObj{impl}, impl{impl}
	{
	}

	~my_font_rendererObj()=default;
};

typedef x::ref<my_font_rendererObj> my_font_renderer;

// Create the custom font-rendering element.

static my_font_renderer create_my_font_renderer(const x::w::container_impl
						&parent_container,
						const std::u32string &label)
{
	// To avoid stuffing this whole thing into the constructor, we do
	// all the prep work here, then construct the internal implementation
	// object.

	// The font for the custom title.
#if 1

	// Specify the font's size in points. The actual font size depends
	// on the current theme.

	x::w::font title_font{"Liberation Serif", 32};
#else

	// Specify the font's size in pixels.

	// Changing the theme will not result in the title's font size
	// changing. However because, below, we set the shadow to three
	// millimeters, adjusting the current theme's size results in the
	// shadow's size changing, and nothing else!

	x::w::font title_font{"Liberation Serif"};

	title_font.set_pixel_size(40);
#endif
	title_font.set_slant(title_font.slant_italic);

	x::w::current_fontcollection current_title_fontcollection=
		parent_container->container_element_impl()
		.create_current_fontcollection(title_font);

	// Use current_fontcollection's current font to compute the
	// initial metrics.
	//
	// The 'fc' member is only available IN_THREAD. fc_public() is
	// a copy of the current 'fc' that's thread-safe, and mutex-protected.

	x::w::fontcollection title_fontcollection=
		current_title_fontcollection->fc_public.get();

	// Construct the theme-based length of the shadow, 3 millimeters.

	x::w::themedim_element_init shadow_dim
		{
		 parent_container,
		 3.0,
		 x::w::themedimaxis::height
		};

	x::w::dim_t max_ascender, max_descender, width;

	compute_metrics(title_fontcollection,
			label,
			max_ascender, max_descender, width);

	auto impl=my_font_renderer_impl
		::create(parent_container,
			 label,
			 current_title_fontcollection,
			 title_fontcollection,
			 shadow_dim,
			 width,
			 max_ascender,
			 max_descender);

	return my_font_renderer::create(impl);
}

void testfontrenderer()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 main_window->set_background_color(x::w::white);

			 x::w::gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 auto factory=layout->append_row();

			 x::w::container_impl parent_container=
				 factory->get_container_impl();

			 factory->created_internally
				 (create_my_font_renderer(parent_container,
							  U"Hello World!"));
		 },
		 x::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom font renderer");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testfontrenderer();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
