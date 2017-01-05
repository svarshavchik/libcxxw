#ifdef xid_t_h
#ifdef connection_thread_h

// Must include both connection_thread.H and xid_t.H to get these.

inline auto xidObj::thread() const { return thread_; }

inline auto xidObj::conn() const { return this->thread_->info; }

inline auto xid_object_container::thread() const { return xid_obj->thread(); }

inline auto xid_object_container::conn() const { return xid_obj->conn(); }

#endif
#endif
