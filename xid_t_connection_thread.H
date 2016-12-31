#ifdef xid_t_h
#ifdef connection_thread_h

// Must include both connection_thread.H and xid_t.H to get these.

template<typename type>
inline auto xid_t<type>::conn() const
{
	return xid_obj->thread_->info;
}


template<typename type> auto xid_tObj<type>::conn() const
{
	return this->thread_->info;
}
#endif
#endif
