#include "picture.H"
#include "pictformat.H"

LIBCXXW_NAMESPACE_START

pictureObj::pictureObj(const ref<implObj> &impl)
	: impl(impl)
{
}

pictureObj::~pictureObj() noexcept=default;

/////////////////////////////////////////////////////////////////////////////

pictureObj::implObj::implObj(const ref<connectionObj::implObj::threadObj> &thread_)
	: xid_tObj<xcb_render_picture_t>(thread_)
{
}

pictureObj::implObj::~implObj() noexcept=default;

LIBCXXW_NAMESPACE_END
