/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef sxg_parserobj_H
#define sxg_parserobj_H

#include "sxg/sxg_parserfwd.H"
#include "defaultthemefwd.H"
#include "icon_imagefwd.H"
#include "x/w/screenfwd.H"
#include "x/w/fontfwd.H"
#include "x/w/gcfwd.H"
#include "x/w/pixmapfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/pictureobj.H"
#include "x/w/drawablefwd.H"
#include "x/w/drawableobj.H"
#include "x/w/pictformatfwd.H"
#include "x/w/rgb.H"
#include "x/w/gcobj.H"
#include "x/w/alignment.H"
#include "x/w/font.H"

#include <x/obj.H>
#include <x/xml/doc.H>
#include <x/vector.H>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <optional>

LIBCXXW_NAMESPACE_START;

//! A parsed SXG file

//! \see sxg_parser

class LIBCXX_HIDDEN sxg_parserObj : virtual public obj {

 public:
	friend class sxg_parserBase;

	//! The screen this SXG image is for.

	const screen screenref;

	//! The theme this SXG image is based on.

	const defaulttheme theme;

	//! Constructor
	sxg_parserObj(const std::string &filename,
		      const screen &screenref,
		      const defaulttheme &theme);

	//! Destructor
	~sxg_parserObj();

	//! Default width of this SXG image.

	//! This translates nominal_width_mm into pixel size.
	dim_t default_width() const;

	//! Default height of this SXG image.

	//! This translates nominal_width_mm into pixel size.
	dim_t default_height() const;

	//! Compute pixel width from millimeters.
	dim_t width_for_mm(double) const;

	//! Compute pixel height from millimeters.
	dim_t height_for_mm(double) const;

	//! Bitplane depth

	//! Returns the value in the <depth> element in the SXG file, if
	//! specified; otherwise 0.
	dim_t depth() const;

	//! Scaled width is preferred to be a multiple of this many pixels
	dim_t width_scale_factor() const;

	//! Scaled heigfht is preferred to be a multiple of this many pixels
	dim_t height_scale_factor() const;

	//! Adjust proposed_width to the nearest multiple of width_scale_factor().

	dim_t adjust_width(dim_t proposed_width, icon_scale scale) const;

	//! Adjust proposed_height to the nearest multiple of height_scale_factor().

	dim_t adjust_height(dim_t proposed_height, icon_scale scale) const;

	//! Given the width, return a height that preserved the aspect ratio.
	dim_t height_for_width(dim_t width, icon_scale scale) const;

	//! Given the height, return a width that preserved the aspect ratio.
	dim_t width_for_height(dim_t height, icon_scale scale) const;

	//! Draw this SXG into a picture.

	void render(
		    //! This PICTURE is cleared to a transparent color, then
		    //! the drawing commands in the SXG file are executed,
		    //! with the results appearing in this picture.
		    const picture &p,

		    //! The PICTURE's drawable.
		    const drawable &d,

		    //! Predefined points in the SXG image.

		    //! The SXG image may define certain points. The
		    //! locations of predefined points are stored here.
		    std::unordered_map<std::string,
		    std::pair<coord_t, coord_t>> &points)
		const;

	//! Whether this SXG image defines a background color.

	std::optional<rgb> background_color(const defaulttheme &theme) const;

 private:

	//! Nominal width
	dim_t nominal_width;

	//! Nominal height
	dim_t nominal_height;

	//! Nominal depth
	dim_t nominal_depth;

	//! Nominal width in millimeters
	double nominal_width_mm;

	//! Nominal height in millimeters
	double nominal_height_mm;

	//! Compute pixels per millimeter, horizontally.
	double pixels_per_mm_w() const;

	//! Compute pixels per millimeter, vertically.
	double pixels_per_mm_h() const;

	//! Report nominal width as a multiple of widthfactor
	dim_t widthfactor=1;

	//! Report nominal width as a multiple of heightfactor
	dim_t heightfactor=1;

	//! Dimensions of pixmaps and pictures are given in...

	class size_type_t {

	public:

		//! ... scaled size, in millimeters, or fixed pixel counts.

		enum type_t { scaled, mm, mmrounded, fixed };

		//! The type value.
		type_t type;

		//! For mmrounded, units are in scale of "factor" millimeters,
		//! rounded off.

		double factor;

		size_type_t(type_t typeArg=scaled, double factorArg=1)
			: type(typeArg), factor(factorArg)
		{
		}
	};

	//! Masking pixmaps

	class pixmap_info {
	public:

		//! What the width and height represents.

		size_type_t size;

		//! Pixmap width

		double width;

		//! Pixmap height

		double height;

		//! Pixmap depth, either 1 or 8

		uint8_t depth=8;

		//! Graphic contexts used by this pixmap

		std::unordered_set<std::string> graphic_contexts;
	};

	//! All pixmaps.

	std::unordered_map<std::string, pixmap_info> pixmaps;

	//! Picture types

	enum class picture_type_t { pixmap, solid_color, text };

	//! Parse either a theme color, or red/green/blue/alpha, in advance.

	class color_info {

	public:
		double red=0;
		double green=0;
		double blue=0;
		double alpha=1;

		std::string theme_color;

		color_info() {}

		color_info(const xml::doc::base::readlock &lock);

		rgb get_color(const defaulttheme &w) const;
	};

	//! Text picture info

	class text_picture_info {

	public:
		std::string font;
		color_info color;
		std::u32string text;

		text_picture_info(const std::string &fontArg,
				  const color_info &colorArg,
				  const std::u32string &textArg)
			: font(fontArg), color(colorArg),
			  text(textArg)
		{
		}
	};

	//! non-main picture information.

	class picture_info {

	public:

		//! The picture type

		picture_type_t type;

		//! For solid_color pictures.

		color_info color;

		//! picture_type_t::pixmap: what the width and height represents

		size_type_t size=size_type_t::scaled;

		//! Pixmap width

		double width=0;

		//! Pixmap height

		double height=0;

		//! picture_type_t::text: the text
		std::vector<text_picture_info> text_info;

		//! picture_type_t::text: its alignment
		halign align=halign::left;
	};

	//! All non-main pictures for drawing the SXG file.

	std::unordered_map<std::string, picture_info> pictures;

	//! Fonts

	std::unordered_map<std::string, font> fonts;

	class xy_t;

	//! Scaling calculations

	//! The scale_info object takes a coordinate specified in the
	//! SXG file, and scales it to the target's size. The scaling
	//! parameters, passed to the constructor, come from execution_info.

	class scale_info {
	public:
		//! Scaling type for this gc, pixmap, or picture
		size_type_t size;

		//! Computed scaling
		double scale_w;
		//! Computed scaling
		double scale_h;
		//! Computed scaling
		double pixels_per_mm_w;
		//! Computed scaling
		double pixels_per_mm_h;

		//! Offset to the center of the scaled point
		dim_t offset_x;

		//! Offset to the center of the scaled point
		dim_t offset_y;

		//! Constructor
		scale_info(const size_type_t &sizeArg,
			   double scale_wArg,
			   double scale_hArg,
			   double pixels_per_mm_wArg,
			   double pixels_per_mm_hArg);

		//! Scaling mode

		//! When an SXG image is scaled, each pixel in the original
		//! SXG coordinate space becomes, essentially, a rectangular
		//! set of pixels in the scaled image. There are several ways
		//! to scale an SXG coordinate to a single coordinate in the
		//! scaled image:
		//!
		//! - beginning: compute the top/left coordinate value
		//!
		//! - ending: compute the bottom/right coordinate value
		//!
		//! - centered: compute the coordinate for the center
		//! of the scaled rectangle.
		//!
		//! - width: if centered_width is not 0, compute the top/left
		//! coordinate of a line with the given width, centered on the
		//! center of the scaled rectangle. if centered_width is 0,
		//! this is same as "centered".

		enum mode_t { centered, width, beginning, ending };

		//! Set when executing the <line_width> instruction.
		dim_t centered_width=0;

		//! Scale X coordinate or dimension.
		coord_t x_pixel(const xy_t &value) const;

		//! Scale X coordinate or dimension.
		coord_t x_pixel(const xy_t &value, mode_t mode) const;

		//! Scale Y coordinate or dimension.
		coord_t y_pixel(const xy_t &value) const;

		//! Scale Y coordinate or dimension.
		coord_t y_pixel(const xy_t &value, mode_t mode) const;

		//! Scale X or Y coordinate or dimension.

		//! Uses the average scaling factor for X and Y dimensions.
		coord_t xy_pixel(double value) const;
	};

	//! An X or a Y coordinate

	class xy_t {

	public:
		//! The coordinate's value
		double value;

		//! Alignment of the coordinate, when scaled.

		scale_info::mode_t mode=scale_info::mode_t::centered;

		//! Constructor.
		inline xy_t(double v=0) : value(v) {}

		//! Addition operator

		inline xy_t operator+(double v) const
		{
			xy_t sum;

			sum.mode=mode;
			sum.value=value+v;
			return sum;
		}
	};

	//! A container for points and dimensions.

	class points_t {

	public:
		std::unordered_map<std::string,
				   std::pair<sxg_parserObj::xy_t,
					     sxg_parserObj::xy_t>> points;

		std::unordered_map<std::string,
				   std::pair<double, double>> dimensions;
	};

	//! Labeled points, as defined in the SXG file.

	points_t points;

	//! What needs to be known to execute SXG.

	class execution_info {

	public:
		//! The object that invoke render().

		const sxg_parserObj &sxg_parser_ref;

		//! Computed scaling, by how much to scale the SXG image.
		double scale_w=1;

		//! Computed scaling, by how much to scale the SXG image.
		double scale_h=1;

		//! Computed scaling
		double pixels_per_mm_w=3;

		//! Computed scaling
		double pixels_per_mm_h=3;

		typedef std::tuple<picture, drawable,
				   size_type_t> pixmap_picture_t;

		//! All instantiated pictures
		std::unordered_map<std::string,
				   pixmap_picture_t> pictures;

		//! All instantiated pictures
		std::unordered_map<std::string, const_picture> const_pictures;

		//! Find a source picture

		const_picture source_picture(const std::string &n) const;

		//! Construct a scale_info for the given picture

		scale_info source_scale(const std::string &n) const;

		//! Find a destination picture
		const pixmap_picture_t &dest_picture(const std::string &n)
			const;

		//! Information about graphic contexts created for execution.

		class gc_info {

		public:
			//! Name of the pixmap this GC is from.
			std::string pixmap_name;

			//! Instantiated GC
			const gc context;

			//! Pixmap's picture
			const picture p;

			//! Current properties
			gcObj::properties properties;

			//! Constructor
			gc_info(const std::string &pixmap_nameArg,
				const gc &contextArg,
				const picture &pArg);
		};

		//! All instantiated graphic contexts

		//! A tuple of pixmap+gc object+properties
		std::unordered_map<std::string, gc_info> gcs;

		//! All instantiated pixmaps
		std::unordered_map<std::string, pixmap> pixmaps;

		//! Constructor
		execution_info(const sxg_parserObj &sxg_parser_refArg);

	};

	//! A single SXG command to execute.

	//! The SXG file gets compiled in a list of these commands. To render
	//! an SXG image, each command's execute() gets invoked, passing it
	//! an execution_info object.

	class sxg_operationObj : virtual public obj {

	public:
		//! Constructor
		sxg_operationObj();

		//! Destructor
		~sxg_operationObj();

		//! Invoke to execute instructions
		virtual void execute(execution_info &info)=0;

		template<typename functor_type> class implObj;
	};

	template<typename functor_type>
		static ref<sxg_operationObj>
		make_execute(functor_type &&functor);

	//! Instructions to execute in order to render SXG into a picture
	std::vector<ref<sxg_operationObj>> operations;

	//! Information needed to execute <gc> commands.
	class gc_execute_info {

	public:
		//! The GC properties
		gcObj::properties &props;

		//! The GC
		const gc &context;

		//! The picture GC is from
		const picture &p;

		//! Scaling information
		const scale_info &scale;

		//! Execution info
		sxg_parserObj::execution_info &info;

		//! Constructor
		gc_execute_info(gcObj::properties &propsArg,
				const gc &contextArg,
				const picture &pArg,
				const scale_info &scaleArg,
				sxg_parserObj::execution_info &infoArg);
	};

	//! Execute a single <gc> command.

	//! Each <gc> command is parsed into this object, in phase 1.
	//! In phase 2 execute_gc() executes the command.
	//!
	//! The <gc> itself gets compiled into an sxg_operationObj, which
	//! implements execute() by invoking execute_gc() of each command
	//! in the <gc> element.

	class gc_operationObj : virtual public obj {

	public:
		gc_operationObj();
		~gc_operationObj();

		virtual void execute_gc(const gc_execute_info &info)=0;

		template<typename functor_type> class implObj;
	};

	template<typename functor_type>
		static ref<gc_operationObj>
		make_execute_gc(functor_type &&functor);

	//! Information needed to execute <render> commands.

	class render_execute_info {

	public:
		const picture &dest_picture;
		const scale_info &scale;
		sxg_parserObj::execution_info &info;

		render_execute_info(const picture &dest_pictureArg,
				    const scale_info &scaleArg,
				    sxg_parserObj::execution_info &infoArg);
	};

	//! Execute a single <render> command.

	//! Each <render> command is parsed into this object, in phase 1.
	//! In phase 2 execute_render() executes the command.
	//!
	//! The <render> itself gets compiled into an sxg_operationObj, which
	//! implements execute() by invoking execute_gc() of each command
	//! in the <render> element.

	class render_operationObj : virtual public obj {

	public:
		render_operationObj();
		~render_operationObj();

		virtual void execute_render(const render_execute_info &info)=0;

		template<typename functor_type> class implObj;
	};

	template<typename functor_type>
		static ref<render_operationObj>
		make_execute_render(functor_type &&functor);

	//! Create a text picture.
	std::tuple<pixmap, picture>
		create_text_picture(const picture_info &info,
				    const drawable &main) const;

	//! Internal parsing function.

	template<typename value_type>
		static void get_width_height(const xml::doc::base::readlock
					     &parent,
					     points_t &points,
					     const char *width_elem,
					     const char *height_elem,
					     const char *size_elem,
					     value_type &width,
					     value_type &height);

	//! Internal parsing function.
	template<typename value_type>
		static void get_width_height(const xml::doc::base::readlock
					     &parent,
					     points_t &points,
					     value_type &width,
					     value_type &height);

	//! Internal parsing function.
	bool parse_root(const xml::doc::base::readlock &root);

	//! Internal parsing function.
	static void get_xy_value(const xml::doc::base::readlock &parent,
				 const char *name,
				 xy_t &xy);

	//! Internal parsing function.
	static void get_xy(const xml::doc::base::readlock &lock,
			   points_t &points,
			   xy_t &x,
			   xy_t &y);

	//! Internal parsing function.
	static bool optional_xy(const xml::doc::base::readlock &render_element,
				points_t &points,
				xy_t &x, xy_t &y);

	//! Internal parsing function.
	static bool get_fill_style_pixmap(const xml::doc::base::readlock &lock,
					  points_t &points,
					  const char *name,
					  std::string &pixmap,
					  xy_t &x,
					  xy_t &y);

	//! Internal parsing function.
	static size_type_t
		parse_size_type_t(const xml::doc::base::readlock &parent);

	//! Internal parsing function.
	void parse_pixmaps(const xml::doc::base::readlock &root);

	//! Internal parsing function.
	void parse_pictures(const xml::doc::base::readlock &root);

	struct parse_render_gc_info;

	//! Internal parsing function.
	ref<sxg_operationObj>
		parse_gc(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	ref<sxg_operationObj>
		parse_gc(const xml::doc::base::readlock &lock,
			 const std::string &id,
			 const parse_render_gc_info &info);


	//! Internal parsing function.
	ref<gc_operationObj>
		parse_gc_op(const xml::doc::base::readlock &lock,
			    const std::string &id,
			    const parse_render_gc_info &info);

	//! Internal parsing function.
	ref<gc_operationObj>
		parse_gc_fill(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_function(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_foreground(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_background(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_fill_arc_mode(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	ref<gc_operationObj>
		parse_gc_arcs(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_line_width(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_line_style(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_cap_style(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_join_style(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	ref<gc_operationObj>
		parse_gc_fill_style(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	static ref<gc_operationObj>
		parse_gc_dashes(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	ref<gc_operationObj>
		parse_gc_clear(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	ref<gc_operationObj>
		parse_gc_clip(const xml::doc::base::readlock &lock,
			      const std::string &id,
			      const parse_render_gc_info &info);

	//! Internal parsing function.
	ref<sxg_parserObj::gc_operationObj>
		parse_gc_line(const xml::doc::base::readlock &lock);

	struct sxg_rectangle;
	struct sxg_arc_info;
	class sxg_point;
	struct sxg_triangle;

	vector<sxg_rectangle>
		parse_rectangles(const xml::doc::base::readlock &lock);

	//! Internal parsing function.
	ref<sxg_operationObj>
		parse_render(const xml::doc::base::readlock &render_element);

	//! Internal parsing function.
	ref<sxg_operationObj>
		parse_render(const xml::doc::base::readlock &render_element,
			     const std::string &id,
			     const parse_render_gc_info &info);

	//! Internal parsing function.
	static bool parse_src_mask(const xml::doc::base::readlock &render_element,
				   points_t &points,
				   const char *what,
				   const char *pic_or_pixmap,
				   std::string &picture,
				   xy_t &x,
				   xy_t &y);

	//! Internal parsing function.
	ref<render_operationObj>
		parse_render_composite(const xml::doc::base::readlock &render_element);

	//! Internal parsing function.
	static void parse_text_picture(const xml::doc::base::readlock &lock,
				       picture_info &info);

	//! Internal parsing function.
	static ref<render_operationObj>
		parse_render_repeat(const xml::doc::base::readlock &render_element);

	//! Internal parsing function.
	ref<render_operationObj>
		parse_render_clip(const xml::doc::base::readlock &render_element,
				  const std::string &id,
				  const parse_render_gc_info &info);

	//! Internal parsing function.
	ref<render_operationObj>
		parse_render_fill(const xml::doc::base::readlock &render_element);

	//! Internal parsing function.
	ref<render_operationObj>
		parse_render_triangles(const xml::doc::base::readlock &render_element);

	//! Internal typedef alias
	typedef void (pictureObj::*pic_func_no_mask)(const pictureObj::point *,
						     size_t,
						     const const_picture &,
						     render_pict_op,
						     coord_t,
						     coord_t);
	//! Internal typedef alias
	typedef void (pictureObj::*pic_func_mask)(const pictureObj::point *,
						  size_t,
						  const const_picture &,
						  const const_pictformat &,
						  render_pict_op,
						  coord_t,
						  coord_t);

	//! Internal parsing function.
	ref<render_operationObj>
		parse_render_tri(const xml::doc::base::readlock &render_element,
				 pic_func_no_mask func_1,
				 pic_func_mask func_2);

	//! Internal parsing function.
	ref<render_operationObj>
		parse_render_op(const xml::doc::base::readlock &render_element,
				const std::string &id,
				const parse_render_gc_info &info);

	//! Internal parsing function.
	void parse_fonts(const xml::doc::base::readlock &root);

	//! Internal parsing function.
	void parse_operations(const xml::doc::base::readlock &root);

	//! Internal parsing function.
	void get_points(const xml::doc::base::readlock &root,
			const char *element);

	//! Internal parsing function.
	void get_dimensions(const xml::doc::base::readlock &root,
			    const char *element);
};

LIBCXXW_NAMESPACE_END

#endif
