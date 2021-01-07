/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifdef x_w_screen_h
#ifdef x_w_connection_h

inline connection screenObj::get_connection() { return connref; }

inline const_connection screenObj::get_connection() const { return connref; }

#endif
#endif
