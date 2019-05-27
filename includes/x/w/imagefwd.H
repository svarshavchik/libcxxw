/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_imagefwd_h
#define x_w_imagefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC imageObj;

/*! An image

\code
#include <INSERT_LIBX_NAMESPACE/w/image.H>

INSERT_LIBX_NAMESPACE::w::image image=
    factory->create_image("image1.sxg", 100, 0);
\endcode

This widget shows an image loaded from a file.
The filename is followed by width and height values which are
\ref dim_arg "INSERT_LIBX_NAMESPACE::w::dim_arg"s and are optional.
The width and height values are
\ref dim_arg "INSERT_LIBX_NAMESPACE::w::dim_arg"s.

The filename's extension must
be .sxg, .jpg, .gif, or .png, and specifies the file format.
create_image() tries each one until it finds the file when the filename's
extension is not specified.

Only the .sxg format uses the optional width and height values. Other formats
always use the pixel size from the image.
The width and height values specify the scaled dimensions of the image.
A zero value for either one
computes a default value that preserves the loaded image's aspect ratio.
.sxg is the
library's internal custom image file format.
Without a specified width and height a
loade .sxg image uses its default size and aspect ratio.

If the filename does not have a pathname component and does not exist in the
process's current directory, create_image() also searches the library's
current theme directory.

\note
The loaded images get cached internally. Specifying the same filename uses
the cached image, and does not load the file again. The cache looks only
at the filename, and doesn't check the current directory.
 Changing the process's
current directory and creating a second
\c INSERT_LIBX_NAMESPACE::w::image using the same filename,
 and without a path component in the
filename, does not load the file if a different file with the same name
exists in the new directory, and does not fail if the file does not exist in
the new directory.

\note
The cached images get automatically discarded after their widget
gets removed, and creating another one with the same name tries to load the
image file again. Because image element removal gets finalized in an
internal execution thread, it is not specified whether removing one image
element, then immediately creating another one, uses the stored cached
image. That depends on whether the internal execution thread finalizes the
old image's removal before or after the new image element gets created with
the same filename. Create new images with the same filename before removing
old elements in order to guarantee cached images.

*/

typedef ref<imageObj> image;

//! A nullable pointer reference to an image.

typedef ptr<imageObj> imageptr;

//! A reference to a constant image.

//! \see image

typedef const_ref<imageObj> const_image;

//! A nullable pointer reference to a constant image.

//! \see image

typedef const_ptr<imageObj> const_imageptr;

LIBCXXW_NAMESPACE_END

#endif
