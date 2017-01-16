// Implement element overrides for a container

// Spaghetti code for a subclass of elementObj::implObj, that defines
// methods that elementObj::implObj that get overridden and forwarded to
// containerObj::implObj.

// The class declaration #includes container_element_overrides_decl.H.
// The class definition sets ELEMENT_SUBCLASS, and #includes this.

void ELEMENT_SUBCLASS::inherited_visibility_updated(IN_THREAD_ONLY,
						    bool flag)
{
	containerObj::implObj::inherited_visibility_updated(IN_THREAD, flag);
}

void ELEMENT_SUBCLASS::do_draw(IN_THREAD_ONLY,
			       const draw_info &di,
			       const rectangle_set &areas)
{
	containerObj::implObj::do_draw(IN_THREAD, di, areas);
}
