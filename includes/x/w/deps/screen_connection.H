/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifdef x_w_screen_h
#ifdef x_w_connection_h

inline connection screenObj::conn() { return connref; }

inline const_connection screenObj::conn() const { return connref; }

#endif
#endif
