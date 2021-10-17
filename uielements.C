/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/listitemhandle.H"
#include "messages.H"
#include "gridlayoutmanager.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/itemlayoutmanager.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/toolboxlayoutmanager.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/pagefactory.H"
#include "x/w/bookpagefactory.H"
#include "x/w/panefactory.H"
#include "x/w/toolboxfactory.H"
#include "x/w/menubarfactory.H"
#include "x/w/impl/container.H"
#include "x/w/synchronized_axis.H"
#include "x/w/copy_cut_paste_menu_items.H"
#include "x/w/focusable.H"
#include "screen.H"
#include "defaulttheme.H"
#include "uicompiler.H"

LIBCXXW_NAMESPACE_START

typedef uielements::new_synchronized_axis_t new_synchronized_axis_t;

new_synchronized_axis_t::new_synchronized_axis_t()=default;

new_synchronized_axis_t::~new_synchronized_axis_t()=default;

new_synchronized_axis_t
::new_synchronized_axis_t(const new_synchronized_axis_t &)=default;

new_synchronized_axis_t
::new_synchronized_axis_t(new_synchronized_axis_t &&)=default;

new_synchronized_axis_t &
new_synchronized_axis_t::operator=(const new_synchronized_axis_t &)
=default;

new_synchronized_axis_t &
new_synchronized_axis_t::operator=(new_synchronized_axis_t &&)
=default;


uielements::~uielements()=default;

element uielements::get_element(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_elements.find(std::string{name.begin(), name.end()});

	if (iter == new_elements.end())
		throw EXCEPTION(gettextmsg(_("Element %1% was not found"),
					   name));

	return iter->second;
}


layoutmanager uielements::get_layoutmanager(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_layoutmanagers.find(std::string{name.begin(),
							      name.end()});

	if (iter == new_layoutmanagers.end())
		throw EXCEPTION(gettextmsg(_("Layout manager"
					     " %1% was not found"),
					   name));

	return iter->second;
}

synchronized_axis uielements
::get_synchronized_axis(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_synchronized_axis.find(std::string{name.begin(),
								  name.end()});

	if (iter == new_synchronized_axis.end())
		throw EXCEPTION(gettextmsg
				(_("Radio button group %1% was not found"),
				 name));

	return iter->second;
}

/////////////////////////////////////////////////////////////////////////////

namespace {
#if 0
}
#endif

//! Sentry object for layout managers and factories generate() methods.

//! All generate()rs construct this in auto scope, passing to the
//! template constructor: the forwarded uielements object, this,
//! the container in uielements that contains the generator, and the
//! forwarded name.
//!
//! The template function looks up the named generator and runs it.

struct generate_sentry {
	uielements &elements;

	template<typename layout, typename container_type>
	generate_sentry(uielements &elements,
			layout *me,
			const container_type &generators,
			const std::string_view &name)
		: generate_sentry(elements)
	{
		// TODO: C++20
		auto iter=generators.find({name.begin(), name.end()});

		if (iter == generators.end())
			layout_not_found(name);

		// Run the generator
		auto ref_me=ref{me};

		for (const auto &g:*iter->second)
			g(ref_me, elements);

		finish();
	}

	static void layout_not_found(const std::string_view &name)
		__attribute__((noreturn))
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	// Delegated constructor
	generate_sentry(uielements &elements) : elements{elements}
	{
		// Just in case there's some junk here.
		elements.element_factories_to_generate.clear();
	}

	~generate_sentry()
	{
		// Clean up after ourselves
		elements.element_factories_to_generate.clear();
	}

	void finish()
	{
		// Run any element factory generators we picked up along
		// the way.
		//
		// generate_factory() stashed away the element factory
		// in element_factories_to_generate. The main generator
		// is done now, so we can run it.

		const uielements *me=&elements;

		for (const auto &g:elements.element_factories_to_generate)
			for (const auto &f:*g.second)
				f(me);
	}
};

#if 0
{
#endif
}

#include "uielements.inc.C"

void uielements::generate_factory(const named_element_factory &name_and_factory)
{
	element_factories_to_generate
		.insert_or_assign(name_and_factory.name,
				  name_and_factory.generator);
}

void uielements::generate(const std::string_view &name,
			  const const_uigenerators &generators)
{
	// TODO: C++20
	auto iter=generators->elements_generators.find({name.begin(),
							name.end()});

	if (iter == generators->elements_generators.end())
	{
		throw EXCEPTION(gettextmsg
				(_("Elements factory %1% not defined."),
				 name));
	}

	auto me=this;

	for (const auto &g: *iter->second)
		g(me);
}

static focusable get_focusable(const uielements &uie,
			       const std::string &name)
{
	auto e=uie.get_element(name);

	if (!e->isa<focusable>())
	{
		throw EXCEPTION(gettextmsg
				(_("Element %1% is not a focusable widget"),
				 name));
	}

	return e;
}

static std::vector<focusable> get_focusables(const uielements &uie,
					     const std::vector<std::string>
					     &names)
{
	std::vector<focusable> focusables;

	focusables.reserve(names.size());

	for (const auto &name:names)
		focusables.emplace_back(get_focusable(uie, name));

	return focusables;
}

void uielements::get_focus_first(const std::string &focusable_value) const
{
	get_focusable(*this, focusable_value)->get_focus_first();
}

void uielements::get_focus_before(const std::string &focusable_value,
				  const std::string &before_focusable_value)
	const
{
	get_focusable(*this, focusable_value)
		->get_focus_before(get_focusable(*this,
						 before_focusable_value));
}

void uielements::get_focus_after(const std::string &focusable_value,
				 const std::string &after_focusable_value) const
{
	get_focusable(*this, focusable_value)
		->get_focus_after(get_focusable(*this,
						after_focusable_value));
}

void uielements::get_focus_before_me(const std::string &focusable_value,
				     const std::vector<std::string>
				     &other_focusables_value)
	const
{
	get_focusable(*this, focusable_value)
		->get_focus_before_me(get_focusables(*this,
						     other_focusables_value));
}

void uielements::get_focus_after_me(const std::string &focusable_value,
				    const std::vector<std::string>
				    &other_focusables_value)
	const
{
	get_focusable(*this, focusable_value)
		->get_focus_after_me(get_focusables(*this,
						    other_focusables_value));
}

void uielements::request_focus(const std::string &focusable_value,
			       bool now_or_never)
	const
{
	get_focusable(*this, focusable_value)->request_focus(now_or_never);
}


void uielements::show_all(const std::string &element_value) const
{
	get_element(element_value)->show_all();
}

void uielements::hide_all(const std::string &element_value) const
{
	get_element(element_value)->hide_all();
}

void uielements::show(const std::string &element_value) const
{
	get_element(element_value)->show();
}

void uielements::hide(const std::string &element_value) const
{
	get_element(element_value)->hide();
}

LIBCXXW_NAMESPACE_END
