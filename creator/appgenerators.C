#include "libcxxw_config.h"
#include "creator/app.H"
#include "creator/appgenerator_function.H"
#include "creator/appgenerator_save.H"
#include "creator/uicompiler_generators.H"
#include "creator/uicompiler.H"
#include "x/w/uielements.H"
#include "x/w/impl/uixmlparser.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/input_field_lock.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/label.H"
#include "x/w/button.H"
#include "x/w/uigenerators.H"
#include "messages.H"
#include "catch_exceptions.H"
#include <x/messages.H>
#include <x/strtok.H>
#include <x/weakcapture.H>
#include <x/mpthreadlock.H>
#include <x/visitor.H>
#include <x/strtok.H>
#include <x/locale.H>
#include <x/messages.H>

void appObj::generators_elements_initialize(app_elements_tptr &elements,
					    x::w::uielements &ui,
					    const uicompiler &uiptr)
{
	x::w::focusable_container generator_name=
		ui.get_element("generator_name_field");
	x::w::label generator_new_name_label=
		ui.get_element("generator_new_name_label");
	x::w::input_field generator_new_name=
		ui.get_element("generator_new_name_field");
	x::w::label generator_new_type_label=
		ui.get_element("generator_new_type_label");
	x::w::focusable_container generator_new_type=
		ui.get_element("generator_new_type_field");
	x::w::button generator_new_create=
		ui.get_element("generator_new_create");
	x::w::button generator_update_button=
		ui.get_element("generator_update");
	x::w::button generator_reset_button=
		ui.get_element("generator_reset");
	x::w::button generator_delete_button=
		ui.get_element("generator_delete");

	// Name of a new generator: allow only valid characters.
	generator_new_name->on_filter(get_label_filter());

	// And enable/disable buttons. Must have a non-empty field to enable
	// the save button.
	generator_new_name->on_change
		([]
		 (ONLY IN_THREAD,
		  const auto &change_info)
		 {
			 appinvoke
				 ([&]
				  (appObj *me)
				  {
					  generator_info_lock lock{
						  me->current_generators
						  ->generator_info
					  };

					  me->generator_enable_disable_buttons
						  (IN_THREAD, lock);
				  });
		 });

	generator_new_type->standard_comboboxlayout()->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke
				 ([&]
				  (appObj *me)
				  {
					  generator_info_lock lock{
						  me->current_generators
						  ->generator_info
					  };

					  me->generator_enable_disable_buttons
						  (IN_THREAD, lock);
				  });
		 });

	generator_new_create->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &grigger,
		  const auto &busy)
		{
			appinvoke(&appObj::update_theme,
				  IN_THREAD, busy,
				  &appObj::generator_new_create_clicked);
		});

	// Delete, update, and reset buttons

	generator_delete_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::generator_delete);
		 });

	generator_update_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 appinvoke(&appObj::update_theme,
				   IN_THREAD, busy,
				   &appObj::generator_update);
		 });

	generator_reset_button->on_activate
		([]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		{
			appinvoke(&appObj::generator_reset, IN_THREAD);
		});

	// A new generator has been selected.
	//
	// Invoke generator_function_reset() when the generator name combo-box
	// changes value.
	auto generator_name_lm=generator_name->standard_comboboxlayout();

	generator_name_lm->on_selection_changed
		([]
		 (ONLY IN_THREAD,
		  const auto &info)
		 {
			 appinvoke(&appObj::generator_reset, IN_THREAD);
		 });

	generator_new_type->standard_comboboxlayout()
		->replace_all_items(std::vector<x::w::list_item_param>{
				uiptr->sorted_available_uigenerators.begin(),
				uiptr->sorted_available_uigenerators.end()
			});

	elements.generator_name=generator_name;
	elements.generator_new_name_label=generator_new_name_label;
	elements.generator_new_name=generator_new_name;
	elements.generator_new_type_label=generator_new_type_label;
	elements.generator_new_type=generator_new_type;
	elements.generator_new_create=generator_new_create;
	elements.generator_update_button=generator_update_button;
	elements.generator_reset_button=generator_reset_button;
	elements.generator_delete_button=generator_delete_button;
}

void appObj::generators_initialize(ONLY IN_THREAD)
{
	generator_info_lock lock{current_generators->generator_info};

	// Inventory all generators.
	auto existing_generators=theme.get()->readlock();

	existing_generators->get_root();
	auto xpath=
		existing_generators->get_xpath("/theme/factory|/theme/layout");
	auto n=xpath->count();

	lock.all->ids.clear();
	lock.all->ids.reserve(n);

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		lock.all->ids.push_back(existing_generators->get_any_attribute
					("id"));
	}

	std::sort(lock.all->ids.begin(), lock.all->ids.end());

	// Duplicate IDS: TODO - report this.

	lock.all->ids.erase(std::unique(lock.all->ids.begin(),
				    lock.all->ids.end()),
			    lock.all->ids.end());

	// For the generator_name combo-box, put "-- New Generator --",
	// followed by all loaded generators.
	//
	// This is a two-column combo-box list.

	std::vector<x::w::list_item_param> combobox_items;

	combobox_items.reserve((lock.all->ids.size()+1)*2);

	combobox_items.push_back(_("-- New Generator --"));
	combobox_items.push_back(_(""));

	for (const auto &name:lock.all->ids)
	{
		auto new_element=generator_name_and_description(name);
		combobox_items.insert(combobox_items.end(),
				      new_element.description.begin(),
				      new_element.description.end());
	}

	auto lm=generator_name->standard_comboboxlayout();

	lm->replace_all_items(IN_THREAD, combobox_items);

	// The autoselect will initialize the rest.
	lm->autoselect(IN_THREAD, 0, {});
}

appObj::new_element_t
appObj::generator_name_and_description(const std::string &n)
{
	auto this_generator=theme.get()->readlock();

	this_generator->get_root();

	get_xpath_for(this_generator, "layout|factory", n)->to_node(1);

	// Generator's name and a two-column description.
	return {n, {n, this_generator->get_any_attribute("type")
			+ " " + this_generator->name()}};
}

void appObj::generator_reset(ONLY IN_THREAD)
{
	generator_info_lock lock{current_generators->generator_info};

	// Reset the currently selected generator, and the list of
	// generator's functions.

	lock.all->current_selection.reset();
	lock->functions->clear();

	auto name_lm=generator_name->standard_comboboxlayout();

	auto s=name_lm->selected();

	if (!s || *s == 0)
	{
		// New Generator, show fields for creating it.

		generator_new_name_label->show(IN_THREAD);
		generator_new_name->show(IN_THREAD);
		generator_new_type_label->show(IN_THREAD);
		generator_new_type->standard_comboboxlayout()
			->unselect(IN_THREAD);
		generator_new_type->show(IN_THREAD);
		generator_new_create->show(IN_THREAD);

		// Hide current generator values.
		current_generators->generator_contents_values_grid
			->hide(IN_THREAD);
	}
	else
	{
		size_t &n=*s;

		// Hide new generator fields.
		generator_new_name_label->hide(IN_THREAD);
		generator_new_name->set(IN_THREAD, "");
		generator_new_name->hide(IN_THREAD);
		generator_new_type_label->hide(IN_THREAD);
		generator_new_type->hide(IN_THREAD);
		generator_new_create->hide(IN_THREAD);

		// Show current generator values
		current_generators->generator_contents_values_grid
			->show(IN_THREAD);

		--n;

		// Establish the current_selection and (what will be) its
		// functions.

		lock.all->current_selection.emplace(n,
						    lock->functions);

		auto &current_selection= *lock.all->current_selection;

		// compiler_lookup() this generator, and initialize its
		// values. generator_values_initialize-s the
		// current_generator's functions.

		try {
			auto this_generator=theme.get()->readlock();

			compiler_lookup
				(lock, n, this_generator,
				 [&]
				 (const auto &compiler)
				 {
					 current_generators->
						 generator_values_initialize
						 (IN_THREAD,
						  lock, compiler,
						  this_generator);

					 current_selection.modified=false;
				 });
		} catch (const std::exception &e)
		{
			main_window->stop_message
				(std::string{"An error occured while loading"
					 " this generator: "
				} + e.what());
		}
	}

	// The currently-shown generator is not modified.
	update([]
	       (auto &info)
	{
		info.modified=false;
	});

	generator_enable_disable_buttons(IN_THREAD, lock);
}

void appObj::do_compiler_lookup(generator_info_lock &lock,
				 size_t n,
				 const x::xml::readlock &this_generator,
				 const x::function<compiler_lookup_cb_t>
				 &callback)
{
	this_generator->get_root();

	get_xpath_for(this_generator, "layout|factory",
			      lock.all->ids.at(n))->to_node(1);

	callback( compiler_lookup(this_generator));
}

const_uicompiler_generators
appObj::compiler_lookup(const x::xml::readlock &this_generator)
{
	auto type=this_generator->name();

	type_category_t type_category;

	type_category.category=
		this_generator->get_any_attribute("type");

	if (type == "layout")
		type_category.type=appuigenerator_type::layoutmanager;
	else if (type == "factory")
		type_category.type=appuigenerator_type::factory;
	else
		throw EXCEPTION("Unknown generator type: "
				<< type);

	auto iter=current_generators->uicompiler_info
		->uigenerators_lookup.find(type_category);

	if (iter == current_generators->uicompiler_info
	    ->uigenerators_lookup.end())
	{
		throw EXCEPTION("Unknown "
				<< type << ": "
				<< type_category.category);
	}
	return *iter;
}

std::tuple<std::string, std::optional<size_t>>
appObj::get_new_generator_name_and_type(generator_info_lock &)
{
	return std::tuple{
		x::w::input_lock{generator_new_name}.get(),
		generator_new_type->standard_comboboxlayout()->selected()
	};
}

void appObj::generator_enable_disable_buttons(ONLY IN_THREAD,
					      generator_info_lock &lock)
{
	const auto &[name, type] = get_new_generator_name_and_type(lock);

	// The "Create" button is enabled if the new generator name is not
	// empty and the type is selected.

	generator_new_create->set_enabled(!name.empty() && type);

	// The update/reset/delete buttons are enabled depending upon
	// whether the currently shown generator has been modified.

	if (lock.all->current_selection)
	{
		auto &current_selection=*lock.all->current_selection;

		generator_name->set_enabled(!current_selection.modified);

		generator_update_button
			->set_enabled(current_selection.modified);
		generator_reset_button
			->set_enabled(current_selection.modified);
		generator_delete_button
			->set_enabled(!current_selection.modified);
	}
	// else: update/reset/delete are hidden
}

appObj::get_updatecallbackptr
appObj::generator_new_create_clicked(ONLY IN_THREAD)
{
	generator_info_lock lock{current_generators->generator_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	{
		generator_info_lock lock{saved_lock};

		return me->generator_new_create_clicked2(lock);
	};
}

appObj::update_callback_t
appObj::generator_new_create_clicked2(generator_info_lock &lock)
{
	update_callback_t ret;

	const auto &[new_name,
		     new_type] = get_new_generator_name_and_type(lock);

	if (new_name.empty() || !new_type)
		return ret;

	// Look up the compiler for the selected generator

	auto &generator_name=current_generators->uicompiler_info
		->sorted_available_uigenerators.at(*new_type);

	auto &generator=current_generators->uicompiler_info
		->uigenerators.at(generator_name);

	auto document_lock=theme.get()->readlock();

	// And create an empty one.

	auto created=create_update("layout|factory",
				   generator->type_category.xml_node_name(),
				   new_name,
				   document_lock,
				   true);

	if (!created)
		return ret;

	auto &[doc_lock, new_generator]=*created;

	// We need to set the type attribue of the new <layout> or <factory>
	// node.

	doc_lock->attribute({"type", generator->type_category.category});

	ret.emplace(doc_lock,
		    [new_name,
		     generator_name=this->generator_name,
		     saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    generator_info_lock lock{saved_lock};

			    me->update_new_element
				    (IN_THREAD,
				     me->generator_name_and_description
				     (new_name), lock.all->ids,
				     generator_name);
		    });

	return ret;

}

appObj::get_updatecallbackptr appObj::generator_delete(ONLY IN_THREAD)
{
	generator_info_lock lock{current_generators->generator_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       generator_info_lock lock{saved_lock};

		       return me->generator_delete2(lock);
	       };
}

appObj::update_callback_t
appObj::generator_delete2(generator_info_lock &lock)
{
	update_callback_t ret;

	if (!lock.all->current_selection)
		return ret;

	// Locate what we need to delete.

	auto index=lock.all->current_selection->index;
	auto id=lock.all->ids.at(index);

	auto new_doc=theme.get()->readlock()->clone_document();

	auto doc_lock=new_doc->writelock();

	while (1)
	{
		doc_lock->get_root();

		auto xpath=get_xpath_for(doc_lock, "layout|factory", id);

		if (xpath->count() <= 0)
			break;

		xpath->to_node(1);
		doc_lock->remove();
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this})]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    generator_info_lock lock{saved_lock};

			    me->generator_delete2(IN_THREAD,
						  lock, index,
						  busy_mcguffin);
		    });

	return ret;
}

void appObj::generator_delete2(ONLY IN_THREAD,
			       generator_info_lock &lock,
			       size_t index,
			       const x::ref<x::obj> &busy_mcguffin)
{
	// Update the loaded list of generators we store here,
	// and set the current generator combo-box dropdown to "New Generator".

	lock.all->ids.erase(lock.all->ids.begin()+index);

	auto name_lm=generator_name->standard_comboboxlayout();

	name_lm->remove_item(index+1);

	lock.all->current_selection.reset();

	name_lm->autoselect(IN_THREAD, 0, {});

	generator_name->request_focus(IN_THREAD);
	status->update(_("Deleted"));
}

appObj::get_updatecallbackptr appObj::generator_update(ONLY IN_THREAD)
{
	generator_info_lock lock{current_generators->generator_info};

	return [saved_lock=lock.threadlock(x::ref{this})]
		(appObj *me)
	       {
		       generator_info_lock lock{saved_lock};

		       return me->generator_update2(lock);
	       };
}

appObj::update_callback_t
appObj::generator_update2(generator_info_lock &lock)
{
	appObj::update_callback_t ret;

	if (!lock.all->current_selection)
		return ret; // Shouldn't happen.

	// Locate what we need to update.
	auto index=lock.all->current_selection->index;
	auto id=lock.all->ids.at(index);

	auto doc=theme.get()->readlock();

	doc->get_root();

	get_xpath_for(doc, "layout|factory", id)->to_node();

	auto created_update=
		create_update("layout|factory",
			      lock->compiler->type_category.xml_node_name(),
			      id, doc, false);

	auto &[doc_lock, new_generator]=*created_update;

	// Put back the type attribute.

	doc_lock->attribute({"type", lock->compiler->type_category.category});

	appgenerator_save save_info;

	// And create the updated contents of this layout or factory
	for (auto &func:*lock.all->current_selection->main_functions)
	{
		func->save(doc_lock, save_info);
	}

	// If save() wanted to do more stuff, the time to do it now.

	for (auto &func: save_info.extra_saves)
	{
		func(doc_lock, save_info);
	}

	ret.emplace(doc_lock,
		    [=, saved_lock=lock.threadlock(x::ref{this}),
		     extra_updates=save_info.extra_updates]
		    (appObj *me,
		     ONLY IN_THREAD,
		     const x::ref<x::obj> &busy_mcguffin)
		    {
			    generator_info_lock lock{saved_lock};

			    // Any additional action on top of the
			    // changes we need to make.

			    for (auto &update:*extra_updates)
				    update(me, IN_THREAD, lock);

			    me->generator_update3(IN_THREAD,
						  lock,
						  busy_mcguffin);
		    });

	return ret;
}

void appObj
::generator_autocreate_layout_or_factory(const x::xml::writelock &lock,
					 appgenerator_save &save_info,
					 const char *layout_or_factory,
					 const std::string &type,
					 const std::string &id)
{
	// Does it exist?

	auto xpath=get_xpath_for(lock, "layout|factory", id);

	if (xpath->count())
	{
		xpath->to_node();

		// Does everything match?

		if (lock->name() == layout_or_factory &&
		    lock->get_any_attribute("type") == type)
		{
			// Nothing to do
			return;
		}

		// This will not work.
		throw EXCEPTION(x::gettextmsg
				(_("Existing \"%1%\" is not"
				   " a %2% factory"),
				 id, type));
	}

	// Autocreate it.

	(void)create_update_with_new_document("layout|factory",
					      layout_or_factory,
					      id, lock, true);

	lock->attribute({"type", type});

	// And if this change passes muster, we have an extra_updates to
	// add it to the generator page.

	save_info.extra_updates->emplace_back
		([id]
		 (appObj *me, ONLY IN_THREAD,
		  generator_info_lock &lock)
		{
			me->update_new_element
				(IN_THREAD,
				 me->generator_name_and_description(id),
				 lock.all->ids,
				 me->generator_name);
		});
}

void appObj::generator_update3(ONLY IN_THREAD,
			       generator_info_lock &lock,
			       const x::ref<x::obj> &busy_mcguffin)
{
	// generator_reset() will update the UI for us.

	generator_reset(IN_THREAD);
	status->update(_("Generator updated"));

	// Put the input focus back on the generator name combo-box.
	generator_name->request_focus(IN_THREAD);
}
