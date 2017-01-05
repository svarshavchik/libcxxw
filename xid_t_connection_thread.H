#ifdef xid_t_included
#ifdef connection_thread_included

#ifndef xid_t_connection_thread_H_1
#define xid_t_connection_thread_H_1
// Must include both connection_thread.H and xid_t.H to get these.

inline auto new_xid::thread() const { return thread_; }

inline auto new_xid::conn() const { return this->thread_->info; }

inline auto xid_object_container::thread() const { return xid_obj->thread(); }

inline auto xid_object_container::conn() const { return xid_obj->conn(); }
#endif

#ifdef picture_included

#ifndef xid_t_connection_thread_H_2
#define xid_t_connection_thread_H_2
inline auto pictureObj::implObj::picture_conn() const { return picture_xid.conn(); }
#endif

#endif

#endif
#endif
