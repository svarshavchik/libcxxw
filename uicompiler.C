/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "defaulttheme.H"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/book_appearance.H"
#include "x/w/shortcut.H"
#include "theme_parser_lock.H"
#include "messages.H"
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

uicompiler::uicompiler(const theme_parser_lock &lock,
		       uigeneratorsObj &generators)
	: generators{generators}
{
	auto xpath=lock->get_xpath("/theme/layout | /theme/factory");

	// Build the list of uncompiled_elements, by id.

	size_t count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("Missing \"id\" element"));

		uncompiled_elements.emplace(id, lock->clone());
	}

	// Keep compiling, one at a time, until all done.

	while (!uncompiled_elements.empty())
	{
		auto first=uncompiled_elements.begin();

		auto lock=first->second;
		auto name=first->second->name();
		auto type=first->second->get_any_attribute("type");
		auto id=first->first;

		uncompiled_elements.erase(first);

		if (name == "layout")
		{
			if (type == "grid")
			{
				auto ret=gridlayout_parseconfig(lock);
				generators.gridlayoutmanager_generators
					.emplace(id, ret);
				continue;
			}

			if (type == "book")
			{
				auto ret=booklayout_parseconfig(lock);
				generators.booklayoutmanager_generators
					.emplace(id, ret);
				continue;
			}
		}
		else if (name == "factory")
		{
			if (type == "grid")
			{
				auto ret=gridfactory_parseconfig(lock);
				generators.gridfactory_generators
					.emplace(id, ret);
				continue;
			}

			if (type == "book")
			{
				auto ret=bookpagefactory_parseconfig(lock);
				generators.bookpagefactory_generators
					.emplace(id, ret);
				continue;
			}
		}

		throw EXCEPTION(gettextmsg(_("Unrecognized %1% type \"%2%\""),
					   name,
					   type));
	}
}

void uicompiler::generate(const factory &f,
			  uielements &uif,
			  const std::string &name)
{
	auto iter=uif.factories.find(name);

	if (iter == uif.factories.end())
		throw EXCEPTION(gettextmsg(_("Element \"%1%\" not defined."),
					   name));

	iter->second(f);
}

///////////////////////////////////////////////////////////////////////////
//
// Find a generator for a layout manager or a factory.
//
// Look in the compiled generators first. If not found, it must be in
// uncompiled_elements. Find it, recursively compile it now, put it into
// generators, and return it.
//
// In this manner we end up compiling any generators that are referenced
// from other generators recursively, and because we remove the definition
// from uncompiled_elements beforehand, this detects and fails infinite
// recursion.
//
// This is called from the stylesheet-generator parser upon encountering
// an element that references a generator, by name. This ends up effectively
// translating the named reference in the stylesheet into a compiled generator.
// The referenced element then gets passed as a parameter to something like
// container_gridlayoutmanager, which ends up
// invokes something that executes the generator, when it runs, like
// container_gridlayoutmanager that creates a new container with the grid
// layout manager, then executes the generator for it.
//
// A factory reference ends up

std::tuple<std::string, vector<gridlayoutmanager_generator>>
uicompiler::lookup_gridlayoutmanager_generators(const theme_parser_lock &lock,
						const char *element,
						const char *parent)
{
	auto name=single_value(lock, element, parent);
	return {name, lookup_gridlayoutmanager_generators(lock, name)};
}

vector<gridlayoutmanager_generator>
uicompiler::lookup_gridlayoutmanager_generators(const theme_parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators.gridlayoutmanager_generators.find(name);

		if (iter != generators.gridlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "layout"
	    || iter->second->get_any_attribute("type") != "grid")
	{
		throw EXCEPTION(gettextmsg(_("Layout \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=gridlayout_parseconfig(new_lock);

	generators.gridlayoutmanager_generators.emplace(name, ret);

	return ret;
}

std::tuple<std::string, vector<booklayoutmanager_generator>>
uicompiler::lookup_booklayoutmanager_generators(const theme_parser_lock &lock,
						const char *element,
						const char *parent)
{
	auto name=single_value(lock, element, parent);
	return {name, lookup_booklayoutmanager_generators(lock, name)};
}

vector<booklayoutmanager_generator>
uicompiler::lookup_booklayoutmanager_generators(const theme_parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators.booklayoutmanager_generators.find(name);

		if (iter != generators.booklayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "layout"
	    || iter->second->get_any_attribute("type") != "book")
	{
		throw EXCEPTION(gettextmsg(_("Book layout \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=booklayout_parseconfig(new_lock);

	generators.booklayoutmanager_generators.emplace(name, ret);

	return ret;
}

vector<gridfactory_generator>
uicompiler::lookup_gridfactory_generators(const theme_parser_lock &lock,
					  const char *element,
					  const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators.gridfactory_generators.find(name);

		if (iter != generators.gridfactory_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "factory"
	    || iter->second->get_any_attribute("type") != "grid")
	{
		throw EXCEPTION(gettextmsg(_("Factory \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=gridfactory_parseconfig(new_lock);

	generators.gridfactory_generators.emplace(name, ret);

	return ret;
}

vector<bookpagefactory_generator>
uicompiler::lookup_bookpagefactory_generators(const theme_parser_lock &lock,
					      const char *element,
					      const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators.bookpagefactory_generators.find(name);

		if (iter != generators.bookpagefactory_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "factory"
	    || iter->second->get_any_attribute("type") != "book")
	{
		throw EXCEPTION(gettextmsg(_("Factory \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=bookpagefactory_parseconfig(new_lock);

	generators.bookpagefactory_generators.emplace(name, ret);

	return ret;
}

void uicompiler::gridlayout_append_row(const gridlayoutmanager &layout,
				       uielements &factories,
				       const vector<gridfactory_generator>
				       &generators)
{
	auto f=layout->append_row();

	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler::booklayout_append_pages(const booklayoutmanager &blm,
					 uielements &factories,
					 const vector<bookpagefactory_generator>
					 &generators)
{
	auto f=blm->append();

	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler
::container_booklayoutmanager(const factory &f,
			      uielements &factories,
			      const std::tuple<std::string,
			      vector<booklayoutmanager_generator>> &generators,
			      const std::string &background_color,
			      const std::string &border)
{
	new_booklayoutmanager nblm;

	if (!background_color.empty() || !border.empty())
	{
		auto appearance=
			nblm.appearance->modify
			([&]
			 (const auto &appearance)
			 {
				 if (!background_color.empty())
					 appearance->background_color=
						 background_color;

				 if (!border.empty())
					 appearance->border=border;
			 });

		nblm.appearance=appearance;
	}
	f->create_focusable_container
		([&]
		 (const auto &new_container)
		 {
			 const auto &[name, c_generators] = generators;

			 factories.new_layouts.emplace(name, new_container);

			 booklayoutmanager blm=
				 new_container->get_layoutmanager();

			 for (const auto &g:*c_generators)
			 {
				 g(blm, factories);
			 }
		 },
		 nblm);
}

void uicompiler::container_gridlayoutmanager(const factory &f,
					     uielements &factories,
					     const std::tuple<std::string,
					     vector<gridlayoutmanager_generator>
					     > &generators,
					     const std::string
					     &background_color)
{
	f->create_container
		([&]
		 (const auto &new_container)
		 {
			 const auto &[name, c_generators] = generators;

			 factories.new_layouts.emplace(name, new_container);

			 gridlayoutmanager glm=
				 new_container->get_layoutmanager();
			 if (!background_color.empty())
				 new_container->set_background_color
					 (background_color);

			 for (const auto &g:*c_generators)
			 {
				 g(glm, factories);
			 }

		 },
		 new_gridlayoutmanager{});
}

void uicompiler::container_addbookpage(const bookpagefactory &f,
				       uielements &factories,
				       const std::string &label,
				       const std::string &sc,
				       const std::tuple<std::string,
				       vector<gridlayoutmanager_generator>>
				       &generators)
{
	auto shortcut_iter=factories.shortcuts.find(sc);

	f->add([&]
	       (const auto &label_factory,
		const auto &page_factory)
	       {
		       generate(label_factory, factories, label);

		       page_factory->create_container
			       ([&]
				(const auto &container)
				{
					const auto &[name, c_generators]
						= generators;

					gridlayoutmanager glm=
						container->get_layoutmanager();

					for (const auto &g:*c_generators)
					{
						g(glm, factories);
					}
				},
				new_gridlayoutmanager{});
	       },
	       shortcut_iter == factories.shortcuts.end()
	       ? shortcut{}:shortcut_iter->second);
}

#include "uicompiler.inc.C"

LIBCXXW_NAMESPACE_END
