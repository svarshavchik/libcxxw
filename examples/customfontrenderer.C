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
#include <x/w/main_window_appearance.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/element.H>
#include <x/w/font.H>

#include <x/w/impl/child_element.H>
#include <x/w/impl/background_color_element.H>
#include <x/w/impl/scratch_buffer_draw.H>
#include <x/w/impl/container.H>
#include <x/w/impl/fonts/fontcollection.H>
#include <x/w/impl/fonts/composite_text_stream.H>

#include <string>
#include <iostream>
#include <optional>

#include "close_flag.H"

struct color1_tag;
struct color2_tag;

// Construct a child_element_init_params for our custom element. Factored
// out for readability.
//
// The width of the text, the font's height (ascender+descender), and
// the length of the shadow is already computed for us, we just add it up.
//
// Going out of our way to compute the initial metrics, here, is more
// efficient than creating the custom element first, computing its metrics,
// then setting the metrics. This avoid needless recalculations of the
// child element's size, by its container.

static inline auto create_child_element_init_params(x::w::dim_t width,
						    x::w::dim_t ascender,
						    x::w::dim_t descender,
						    size_t shadow)
{
	x::w::child_element_init_params init_params;

	init_params.scratch_buffer_id=
		"customfontrenderer@examples.w.libcxx.com";

	// The type-safe integer classes employed by the library,
	// x::w::dim_t, x::w::coord_t, et. al. overload the + operator
	// to return larger int classes, to handle overflows. If we want
	// to keep the precision the same we need to explicitly truncate()
	// the result of the addition. More work, but avoids hard to track down
	// bugs, and produces "less awful" results if an overflow does occur.

	x::w::dim_t height=x::w::dim_t::truncate(ascender+descender);

	// Expand our metrics by the length of the drop-down shadow.

	width=x::w::dim_t::truncate(width+shadow);
	height=x::w::dim_t::truncate(height+shadow);
	init_params.initial_metrics={
				     {width, width, width},
				     {height, height, height}
	};

	return init_params;
}

// The implementation class of our custom element. Uses the following
// mixins:
//
// - scratch_buffer_draw, to give us a simplified draw().
//
// - background_color_elementObj, to give us the two colors we'll use for
// our custom-rendered text: the main color, and the shadow color.

class my_font_renderer_implObj : public x::w::scratch_buffer_draw<
	x::w::background_color_elementObj<x::w::child_elementObj,
					  color1_tag, color2_tag>> {

	// Alias for the superclass.

	typedef x::w::scratch_buffer_draw<
		x::w::background_color_elementObj<x::w::child_elementObj,
						  color1_tag, color2_tag>
		> superclass_t;

	// The text.
	const std::u32string title;

	// The fontcollection for drawing the text.
	x::w::fontcollection title_fontcollection;

	// Saved metrics, for convenience.
	x::w::dim_t width;
	x::w::dim_t ascender;
	x::w::dim_t descender;
	size_t shadow;

public:

	my_font_renderer_implObj(const x::w::container_impl &parent_container,
				 const std::u32string &title,
				 const x::w::fontcollection
				 &title_fontcollection,
				 x::w::dim_t width,
				 x::w::dim_t ascender,
				 x::w::dim_t descender,
				 size_t shadow)
		: superclass_t
		{
		 // Background color will be a linear gradient
		 x::w::black,
		 x::w::silver,

		 // Finally, the parameters to child_elementObjs constructor:
		 parent_container,
		 create_child_element_init_params(width, ascender, descender,
						  shadow)},

		  title{title},
		  title_fontcollection{title_fontcollection},
		  width{width},
		  ascender{ascender},
		  descender{descender},
		  shadow{shadow}
	{
	}

	~my_font_renderer_implObj()=default;

	// Implement do_draw().

	void do_draw(ONLY IN_THREAD,
		     const x::w::draw_info &di,
		     const x::w::picture &area_picture,
		     const x::w::pixmap &area_pixmap,
		     const x::w::gc &area_gc,
		     const x::w::clip_region_set &clipped,
		     const x::w::rectangle &area_entire_rect) override;
};

typedef x::ref<my_font_renderer_implObj> my_font_renderer_impl;

// Draw the text, and its shadow.

void my_font_renderer_implObj::do_draw(ONLY IN_THREAD,
				       const x::w::draw_info &di,
				       const x::w::picture &area_picture,
				       const x::w::pixmap &area_pixmap,
				       const x::w::gc &area_gc,
				       const x::w::clip_region_set &clipped,
				       const x::w::rectangle &area_entire_rect)
{
	for (size_t i=shadow; i > 0; )
	{
		--i;

		// Compute the starting coordinates for the text.
		//
		// We repeatedly render the same text, offset for each layer
		// of the shadow, and the final text drawn on top of the
		// shadow.

		x::w::coord_t x=i;
		x::w::coord_t y=x::w::coord_t::truncate(ascender+i);

		// Prepare a composite_text_stream, with the text.
		//
		// We expect to involve only one actual font, out of our
		// fontcollection. However prepare the eventuality that
		// there will be more than one font. The first lookup()-ed
		// font will construct the composite_text_stream, and any
		// extra ones will pile up on top of it.

		std::optional<x::w::composite_text_stream> composited_text;

		title_fontcollection->lookup
			(title.begin(), title.end(),
			 [&]
			 (const std::u32string::const_iterator &beg_iter,
			  const std::u32string::const_iterator &end_iter,
			  const x::w::freetypefont &f)
			 {
				 // First font.

				 if (!composited_text)
					 composited_text.emplace(f,
								 title.size(),
								 0);

				 f->glyphs_to_stream(beg_iter, end_iter,
						     *composited_text,

						     // Text position,
						     // glyphs_to_stream()
						     // updates it.
						     x, y,

						     // Previous character that
						     // was rendered in the same
						     // font (none).
						     0,

						     // Replacement char used
						     // in case there's an
						     // unprintable character.
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

	x::w::font title_font{"Liberation Serif", 32};

	title_font.set_slant(title_font.slant_italic);

	// create_fontcollection() creates a font collection for the display
	// element. The display element is not yet created, but we have
	// the parent container, and we can use it to create_fontcollection().
	//
	// Any display element, but only in the same top level window,
	// can create_fontcollection() for use by any other display element
	// in the same top level window. However it is not necessary to
	// try to optimize and share font collections. create_fontcollection()
	// caches its objects, internally. Using the same exact x::w::font
	// returns the same, cached, object.

	auto title_fontcollection=parent_container->container_element_impl()
		.create_fontcollection(title_font);

	// Load the glyphs for our unicode characters.
	title_fontcollection->load_glyphs(label.begin(), label.end(), ' ');

	// Compute font height above+below the baseline.
	x::w::dim_t max_ascender=0, max_descender=0;
	x::w::dim_t width=0;

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

	// Now we have everything that the implementation object needs to
	// construct itself...

	auto impl=my_font_renderer_impl
		::create(parent_container,
			 label,
			 title_fontcollection,
			 width,
			 max_ascender,
			 max_descender,

			 // ... except the shadow's size, which we wing on the
			 // fly, as 1/5th the font's height.
			 x::w::dim_t::truncate((max_ascender+max_descender)/5
					       +1));

	// And create the "public" object for the custom display element.

	return my_font_renderer::create(impl);
}

void testfontrenderer()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	x::w::main_window_config config;

	// Customize the appearance of the main window
	//
	// White background color.

	config.appearance=config.appearance->modify
		([]
		 (const x::w::main_window_appearance &appearance)
		 {
			 appearance->background_color=x::w::white;
		 });

	auto main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 x::w::gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 auto factory=layout->append_row();

			 // Obtain the parent container from the factory.

			 x::w::container_impl parent_container=
				 factory->get_container_impl();

			 // Create a custom display element, then tell the
			 // container that it was created_internally().

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
