#include "screen.H"
#include "picture.H"
#include "x/w/rgb.H"
#include "x/w/picture.H"
#include "x/w/pictformat.H"
#include <x/weakmultimap.H>

LIBCXXW_NAMESPACE_START

////////////////////////////////////////////////////////////////////////////
//
// Creates solid color fill pictures for a screen, of a particular colors.
//
// These are cached in a weak multimap. Requesting a solid color picture for
// the same color returns an existing object.

screen_solidcolorpictures::screen_solidcolorpictures()
	: map{map_t::create()}
{
}

screen_solidcolorpictures::~screen_solidcolorpictures() noexcept=default;

// Subclass of a picture implementation object constructs the picture
// via xcb_render_create_solid_fill().

class LIBCXX_HIDDEN solidColorPictureObj : public pictureObj::implObj {

 public:
	solidColorPictureObj(const ref<connectionObj::implObj::threadObj>
			     &thread_,
			     const rgb &color)
		: implObj(thread_)
	{
		xcb_render_create_solid_fill(conn()->conn, id(),
					     {
						     .red=color.r,
						     .green=color.g,
						     .blue=color.b,
						     .alpha=color.a
					     });
	}

	~solidColorPictureObj() noexcept
	{
		xcb_render_free_picture(conn()->conn, id());
	}
};

const_picture screenObj::create_solid_color_picture(const rgb &color) const
{
	return impl->screen_solidcolorpictures::map->
		find_or_create(color,
			       [this, &color]
			       {
				       auto impl=ref<solidColorPictureObj>
					       ::create(this->conn()
							->impl->thread,
							color);

				       return ref<pictureObj>::create(impl);
			       });
}

LIBCXXW_NAMESPACE_END
