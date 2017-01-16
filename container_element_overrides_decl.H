// Declare element overrides for a container

// This is #included from a class that inherits from elementObj::implObj
// and containerObj::implObj.
//
// Declare overrides for methods inherited from elementObj::implObj that
// are implemented in containerObj::implObj.


//! Implement do_draw() in containerObj::implObj

void do_draw(IN_THREAD_ONLY,
	     const draw_info &di,
	     const rectangle_set &areas) override;

void inherited_visibility_updated(IN_THREAD_ONLY,
				  bool flag) override;
