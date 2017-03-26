// Implement element overrides for a container

// Spaghetti code for a subclass of elementObj::implObj, that defines
// methods that elementObj::implObj that get overridden and forwarded to
// containerObj::implObj.

// The class declaration #includes container_element_overrides_decl.H.
// The class definition sets ELEMENT_SUBCLASS, and #includes this.

void ELEMENT_SUBCLASS
::inherited_visibility_updated(IN_THREAD_ONLY,
			       inherited_visibility_info &info)
{
	containerObj::implObj::inherited_visibility_updated(IN_THREAD, info);
}

void ELEMENT_SUBCLASS::do_draw(IN_THREAD_ONLY,
			       const draw_info &di,
			       const rectangle_set &areas)
{
	containerObj::implObj::do_draw(IN_THREAD, di, areas);
}

void ELEMENT_SUBCLASS::process_updated_position(IN_THREAD_ONLY)
{
	containerObj::implObj::process_updated_position(IN_THREAD);
}

void ELEMENT_SUBCLASS::request_visibility_recursive(IN_THREAD_ONLY, bool flag)
{
	containerObj::implObj::request_visibility_recursive(IN_THREAD, flag);
}

void ELEMENT_SUBCLASS::do_for_each_child(IN_THREAD_ONLY,
					 const function<void (const element &)> &e)
{
	containerObj::implObj::do_for_each_child(IN_THREAD, e);
}

void ELEMENT_SUBCLASS::draw_after_visibility_updated(IN_THREAD_ONLY, bool flag)
{
	containerObj::implObj::draw_after_visibility_updated(IN_THREAD, flag);
}

void ELEMENT_SUBCLASS::theme_updated(IN_THREAD_ONLY)
{
	elementObj::implObj::theme_updated(IN_THREAD);
	containerObj::implObj::theme_updated(IN_THREAD);
}

elementObj::implObj &ELEMENT_SUBCLASS::get_element_impl()
{
	return *this;
}

const elementObj::implObj &ELEMENT_SUBCLASS::get_element_impl() const
{
	return *this;
}
