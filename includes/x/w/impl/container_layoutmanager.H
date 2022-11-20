#ifdef x_w_impl_layoutmanager_H_defined
#ifdef x_w_impl_container_H_defined

//! If the layout manager has been installed, invoke it.

//! This nicely handles race conditions during destructions by
//! doing nothing.

template<typename functor_type>
inline void containerObj::implObj::invoke_layoutmanager(functor_type &&functor)
{
	auto p=layoutmanager_ptr.get();

	if (!p)
		return; // In the process of being destroyed

	functor(layout_impl{p});
}

//! If the layout manager has been installed, invoke it.

//! This nicely handles race conditions during destructions by
//! doing nothing.

template<typename functor_type>
inline void containerObj::implObj::invoke_layoutmanager(functor_type &&functor)
	const
{
	auto p=layoutmanager_ptr.get();

	if (!p)
		return; // In the process of being destroyed

	functor(const_layout_impl{p});
}
#endif
#endif
