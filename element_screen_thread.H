// Must include element.H, screen.H, and connection_thread.H
// to get this definition.

#ifdef connection_thread_h
#ifdef screen_h
#ifdef element_h

inline connection_thread elementObj::implObj::thread()
{
	return screenref->impl->thread;
}

#endif
#endif
#endif
