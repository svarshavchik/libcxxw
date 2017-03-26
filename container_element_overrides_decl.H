// Declare element overrides for a container

// This is #included from a class that inherits from elementObj::implObj
// and containerObj::implObj.
//
// Declare overrides for methods inherited from elementObj::implObj that
// are implemented in containerObj::implObj.

void do_draw(IN_THREAD_ONLY,
	     const draw_info &di,
	     const rectangle_set &areas) override;

void inherited_visibility_updated(IN_THREAD_ONLY,
				  inherited_visibility_info &info) override;

void process_updated_position(IN_THREAD_ONLY) override;

void request_visibility_recursive(IN_THREAD_ONLY, bool flag) override;

void draw_after_visibility_updated(IN_THREAD_ONLY, bool flag) override;

void do_for_each_child(IN_THREAD_ONLY,
		       const function<void (const element &e)> &) override;

void theme_updated(IN_THREAD_ONLY);

elementObj::implObj &get_element_impl() override;

const elementObj::implObj &get_element_impl() const override;
