/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximserverfwd_H
#define ximserverfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"
#include <cstdint>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN ximserverObj;
class LIBCXX_HIDDEN ximserverBase;
class ximattrvalue;

/*! XIM Input Method ID */

typedef uint16_t xim_im_t;

/*! X Input method protocol implementation.

An opaque ximserver handle that implements the protocol for talking to an
X Input Method server.

The actual on-the-wire protocol is described in XML, in xim.xml. xim.xsl
takes the xim.xml description and robo-generated spaghetti code to construct
the actual messages to throw into the wire, and parse the received messages
that we support.

The subclass implements the actual transport, and several callbacks.

The subclass calls received(), passing the opaque message received from the
X Input Method server, and the robo-generated code takes it from there.

The subclass should call stop() to indicate that the connection to the
XIM server has been terminated, or that it could not be established.

The following virtual methods must be implemented by the subclass:

- send() - send an opaque message to the XIM server.

- xim_disconnected() - indicates that the connection to the X Input Method
server has been severed; either as part of an orderly connection close, or
a protocol-level fatal error. This is a callback from the ximserver layer.
The transport layer is still responsible for invoking disconnect() if it
fails to establish a connection, or encounters a transport-level connection
error.

- xim_fully_connected() - indicates that the connection to the X Input Method
server has been completed, and it's ready to use.

*/

typedef ref<ximserverObj, ximserverBase> ximserver;

typedef ptr<ximserverObj, ximserverBase> ximserverptr;

LIBCXXW_NAMESPACE_END

#endif
