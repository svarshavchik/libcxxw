/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "appgenerator_functions.H"
#include "uicompiler.H"
#include "app.H"
#include "x/w/uielements.H"
#include "x/w/container.H"
#include "x/w/focusable_container.H"
#include "x/w/listitemhandle.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/dialog.H"
#include "x/w/uielements.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/text_param_literals.H"
#include "appgenerator_function.H"
#include <x/weakcapture.H>

current_selection_info_s
::current_selection_info_s(size_t index,
			   const x::vector<const_appgenerator_function>
			   &main_functions)
	: index{index}, main_functions{main_functions}
{
}

current_selection_info_s::~current_selection_info_s()=default;

generator_info_s::generator_info_s(const x::ref<all_generatorsObj>
				   &all_generators)
	: functions{x::vector<const_appgenerator_function>::create()},
	  all_generators{all_generators}
{
}

generator_info_s::~generator_info_s()=default;

appgenerator_functionsObj
::appgenerator_functionsObj(app_generator_elements_tptr &generator_elements,
		      const x::w::main_window &parent_window,
		      const x::w::const_uigenerators &cxxwui_generators,
		      const const_uicompiler &uicompiler_info,
		      const x::ref<all_generatorsObj> &all_generators)
	: const_app_generator_elements_t{std::move(generator_elements)},
	  parent_window{parent_window},
	  cxxwui_generators{cxxwui_generators},
	  uicompiler_info{uicompiler_info},
	  generator_info{all_generators}
{
}

appgenerator_functionsObj::~appgenerator_functionsObj()=default;

namespace {
#if 0
}
#endif

//////////////////////////////////////////////////////////////////////////
//
// Shared code that gathers stuff for generator functions' code's needs.

// The constructor's parameter is generator_contents_values list layout
// manager, and this captures what's considered to be the selected value.
//
// This is inherited by generator_value_info.

struct generator_value_info_selected_value {

	// The currently highlighted generator value.

	std::optional<size_t> selected_value;

	generator_value_info_selected_value(const x::w::listlayoutmanager &lm)
		: selected_value{lm->current_list_item()}
	{
		// What we consider to be the selected value is the one
		// that's currently highlighted. If not currently highlighted,
		// fall back ot the currently selected value.

		if (!selected_value)
			selected_value=lm->selected();
	}
};


// Move up/down need to have the listlayoutmanager to finish their work.
//
// The virtually-inherited lock object gets constructed first.
//
// generator_value_info_lm_object gets constructed next, this creates
// the listlayoutmanager after the generator lock is already constructed.
//
// We store and keep the listlayoutmanager here, then construct
// generator_value_info.
//
// The upshot of all this is that the generator_info::lock always goes up
// the flagpole first.

struct generator_value_info_lm_object {

	const x::w::listlayoutmanager lm;

	generator_value_info_lm_object(const x::w::listlayoutmanager &lm)
		: lm{lm}
	{
	}
};

#if 0
{
#endif
}

// Retrieves the currently selected generator function.
//
// Extends the generator_info_lock to also capture the currently
// selected value.
//
// After the virtual subclass is constructed. acquiring the lock, we
// construct the listlayoutmanager and get the currently selected value.
//
// Implements a bool operator that indicates whether:
//
// - there's a selected list of generator functions displayed on the
// generator tab.
//
// - the list of generator functions has a selected value
//
// Inherits the -> operator from the underlying generator_info_lock.

struct appgenerator_functionsObj::generator_value_info
	: virtual generator_info_lock, generator_value_info_selected_value {

	generator_value_info(appgenerator_functionsObj *me)
		: generator_info_lock{me->generator_info},
		  generator_value_info_selected_value{
			  me->generator_contents_values->listlayout()
		  }
	{
	}

	// Constructor used by generator_value_info_lm

	generator_value_info(appgenerator_functionsObj *me,
			     const x::w::listlayoutmanager &lm)
		: generator_info_lock{me->generator_info},
		  generator_value_info_selected_value{lm}
	{
	}

	using generator_info_lock::operator->;

	// Whether the UI action can proceed.

	operator bool() const
	{
		return all->current_selection && selected_value;
	}
};

// Retrieves the currently selected generator function and the layout manager
//
// Subclasses generator_value_info to also include the 'lm' member, the
// list layout manager for the list of generator functions
//
// This is used to implement consistent locking order. The virtually-inherited
// generator_info_lock gets constructed first, then
// generator_value_info_lm_object gets constructed, which retrieves the
// list layout manager and stores it, then the generator_value_info
// gets fully constructed, with the list layout manager getting passed in
// to its constructor.
struct appgenerator_functionsObj::generator_value_info_lm
	: generator_value_info_lm_object,
				 generator_value_info {

	generator_value_info_lm(appgenerator_functionsObj *me)
		: generator_info_lock{me->generator_info},
		  generator_value_info_lm_object{
			  me->generator_contents_values->listlayout()
		  },
		  generator_value_info{
			  me,
			  generator_value_info_lm::lm
		  }
	{
	}

	using generator_value_info::operator->;
	using generator_value_info::operator bool;
};

namespace {
#if 0
}
#endif

// Helper for invoking appgenerator_functions methods for callbacks.

// Similar to appinvoke(), but captures a weak reference on the creator
// appgenerator_functions that spawned this.
//
// An overloaded () operator attempts to recover a strong reference and
// invokes the passed in callback if successful.
//
// Additionally: the callback returns true if it modified the generators,
// if so:
//
// - update() gets called to set the modified flag, indicating that the
// theme has been modified. Ditto for the generators' current selection's
// modified flag.
//
// - generator_enable_disable_buttons() gets called to update their status.

struct appgenerators_values_invokeObj : virtual public x::obj {

	//! Weakly-captured original appgenerator_functions object.

	x::weakptr<x::ptr<appgenerator_functionsObj>> weakptr;

	//! Constructor
	appgenerators_values_invokeObj(appgenerator_functionsObj *me)
		: weakptr{ x::ptr{me} }
	{
	}

	//! Recover the appgenerator_functions object, and invoke the callback.

	template<typename Cb>
	void operator()(ONLY IN_THREAD, Cb &&cb) const
	{
		do_invoke(IN_THREAD,
			  x::make_function<bool (appgenerator_functionsObj *)>
			  ( std::forward<Cb>(cb)));
	}

	//! Type-erased () overloda.
	void do_invoke(ONLY IN_THREAD,
		       const x::function<bool (appgenerator_functionsObj *)>
		       &cb)
		const
	{
		auto p=weakptr.getptr();

		if (!p)
			return;

		auto ptr=&*p;

		if (!cb( ptr ))
			return;

		appinvoke([&]
			  (appObj *me)
		{

			appgenerator_functionsObj::generator_value_info
				lock{ptr};

			if (!lock.all->current_selection)
				return; // Shouldn't happen.


			me->update([&](auto &info)
			{
				info.modified=true;
				lock.all->current_selection->modified=true;
			});


			me->generator_enable_disable_buttons(IN_THREAD, lock);
		});
	}
};

typedef x::ref<appgenerators_values_invokeObj> appgenerators_values_invoke;

#if 0
{
#endif
}

void appgenerator_functionsObj::
generators_values_elements_initialize(app_generator_elements_tptr &elements,
				      x::w::uielements &ui,
				      const uicompiler &uicompiler_info)
{
	x::w::focusable_container generator_contents_values=
		ui.get_element("generator_contents_values");
	x::w::container generator_contents_values_popup=
		ui.get_element("generator_contents_values_popup");

	x::w::listitemhandle generator_value_move_up=
		ui.get_listitemhandle("generator_value_move_up");
	x::w::listitemhandle generator_value_move_down=
		ui.get_listitemhandle("generator_value_move_down");
	x::w::listitemhandle generator_value_update=
		ui.get_listitemhandle("generator_value_update");
	x::w::listitemhandle generator_value_create=
		ui.get_listitemhandle("generator_value_create");
	x::w::listitemhandle generator_value_delete=
		ui.get_listitemhandle("generator_value_delete");

	// Load generator types

	// Save our elements
	elements.generator_contents_values_popup=
		generator_contents_values_popup;
	elements.generator_value_move_up=generator_value_move_up;
	elements.generator_value_move_down=generator_value_move_down;
	elements.generator_value_update=generator_value_update;
	elements.generator_value_create=generator_value_create;
	elements.generator_value_delete=generator_value_delete;

	elements.generator_contents_values_grid=
		ui.get_element("generator_contents_values_grid");
	elements.generator_contents_values=generator_contents_values;
}

void appgenerator_functionsObj
::generator_values_initialize(ONLY IN_THREAD,
			      generator_info_lock &lock,
			      const const_uicompiler_generators &compiler,
			      const x::xml::readlock &xml)
{
	// Capture myself, weakly.
	auto generator_values=appgenerators_values_invoke::create(this);

	auto generator_contents_values_lm=
		generator_contents_values->listlayout();

	// Selecting a generator value opens a popup
	generator_contents_values_lm->selection_type
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &lm,
		  size_t i,
		  const auto &trigger,
		  const auto &busy)
		 {
			 (*generator_values)
				 (IN_THREAD,
				  [&]
				  (auto *me)
				 {
					 lm->unselect(IN_THREAD);
					 lm->selected(IN_THREAD, i,
						      true, trigger);

					 me->generator_contents_values_popup
						 ->show_all(IN_THREAD);

					 return false;
				   });
		 });

	// Call generator_contents_values_changed if the currently-selected
	// or highlighted lits item has changed.

	generator_contents_values_lm->on_selection_changed
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					generator_value_info_lm lock{me};

					me->generator_contents_values_changed
						(IN_THREAD, lock);

					return false;
				});
		});

	generator_contents_values_lm->on_current_list_item_changed
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					generator_value_info_lm lock{me};

					me->generator_contents_values_changed
						(IN_THREAD, lock);

					return false;
				});
		});

	// Install callbacks for move up and down.

	generator_value_move_up->on_status_update
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			if (status_info.trigger.index() ==
			    x::w::callback_trigger_initial)
				return;
			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					return me->generator_value_move_up_clicked
						(IN_THREAD);
				});
		});

	generator_value_move_down->on_status_update
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			if (status_info.trigger.index() ==
			    x::w::callback_trigger_initial)
				return;
			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					return me->generator_value_move_down_clicked
						(IN_THREAD);
				});
		});

	// Install callbacks for update and delete popup actions.
	generator_value_update->on_status_update
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			if (status_info.trigger.index() ==
			    x::w::callback_trigger_initial)
				return;
			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					me->generator_value_update_clicked
						(IN_THREAD);
					return false;
				});
		});

	generator_value_delete->on_status_update
		([generator_values]
		 (ONLY IN_THREAD,
		  const auto &status_info)
		{
			if (status_info.trigger.index() ==
			    x::w::callback_trigger_initial)
				return;

			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					return me->generator_value_delete_clicked
						(IN_THREAD);
				});
		});

	// Load this generator's functions, and record the compiler used.
	(*lock->functions)=
		compiler->parse(xml, uicompiler_info->uigenerators);

	lock->compiler=compiler;

	// Now load all the parsed functions in the generator_contents_values
	// list.

	std::vector<x::w::list_item_param> values;

	values.reserve(lock->functions->size()+1);

	for (const auto &f:*lock->functions)
	{
		values.emplace_back(f->description(description_format::list));
	}
	values.emplace_back(_("-- Create --"));

	generator_contents_values_lm
		->replace_all_items(IN_THREAD, values);

	// Load the list of possible functions that can be created, into
	// the "Create" submenu.

	std::vector<x::w::list_item_param> creator_values;

	auto available_functions=compiler->available_functions();

	creator_values.reserve(available_functions.size()*2);

	for (const auto &[function, description] : available_functions)
	{
		creator_values.push_back
			([function, generator_values]
			 (ONLY IN_THREAD,
			  const auto &info)
			{
				if (info.trigger.index() ==
				    x::w::callback_trigger_initial)
					return;

				// UI action in response to selecting a new
				// function to create.
				//
				// The generator function gets clone()d, and
				// becomes the initial, empty default, that
				// gets opened in the generator_value_edit
				// dialog window.

				(*generator_values)
					(IN_THREAD,
					 [&]
					 (auto me)
					{
						me->generator_value_edit
							(IN_THREAD,
							 function->clone(),
							 &appgenerator_functionsObj
							 ::generator_value_created);
						return false;
					});
			});
		creator_values.push_back(description);
	}

	generator_value_create->submenu_listlayout()
		->replace_all_items(IN_THREAD,
				    creator_values);
}

void appgenerator_functionsObj
::generator_contents_values_changed(ONLY IN_THREAD,
				    generator_value_info_lm &lock)
{
	if (!lock.all->current_selection)
	{
		// We shouldn't get here if there is no current selection.
		// But if the impossible happens everything gets turned off.

		generator_value_move_up->enabled(false);
		generator_value_move_down->enabled(false);
		generator_value_update->enabled(false);
		generator_value_create->enabled(false);
		generator_value_delete->enabled(false);
		return;
	}

	auto n_functions=lock->functions->size();

	if (!lock.selected_value ||
	    *lock.selected_value >= n_functions)
	{
		// -- Create -- placeholder item at the end selected.
		//
		// Only "Create" is available to be chosent (hopefully)

		generator_value_move_up->enabled(false);
		generator_value_move_down->enabled(false);
		generator_value_update->enabled(false);
		generator_value_create->enabled(lock.selected_value ?
						true:false);
		generator_value_delete->enabled(false);
	}
	else
	{
		// Up is available, unless this is the first function
		generator_value_move_up
			->enabled(lock.selected_value > 0);

		// Down is available, unless this is the last function
		generator_value_move_down
			->enabled(*lock.selected_value+1 < n_functions);

		// Update, create, and delete are available.
		generator_value_update->enabled(true);
		generator_value_create->enabled(true);
		generator_value_delete->enabled(true);
	}
}

bool appgenerator_functionsObj::generator_value_move_up_clicked(ONLY IN_THREAD)
{
	generator_value_info_lm lock{this};

	if (!lock)
		return false;

	// Make sure that the currently selected value can be moved up.
	auto n=*lock.selected_value;

	if (n <= 0 || n >= lock->functions->size())
		return false;

	std::vector<x::w::list_item_param> values;

	// We'll effect the move by removing the previous value, then adding
	// it back after the currently selected value, so that the currently
	// selected value remains the same.
	//
	// Retrieve the previous value's description.
	values.push_back((*lock->functions)[n-1]->description(description_format
							      ::list));

	// Remove the previous value
	lock.lm->remove_item(IN_THREAD, n-1);

	// Add it back after the moved-up function.
	lock.lm->insert_items(IN_THREAD, n, values);

	// And update the record, accordingly.
	std::swap((*lock->functions)[n-1],
		  (*lock->functions)[n]);

	--*lock.selected_value; // Reflects what just happened
	generator_contents_values_changed(IN_THREAD, lock);

	return true;
}

bool appgenerator_functionsObj::generator_value_move_down_clicked(ONLY IN_THREAD)
{
	generator_value_info_lm lock{this};

	if (!lock)
		return false;

	// Make sure that the currently selected value can be moved up.
	auto n=*lock.selected_value;

	if (n+1 >= (*lock->functions).size())
		return false;

	std::vector<x::w::list_item_param> values;

	// We'll effect the move by removing the next value, then adding
	// it before the currently selected value, so that the currently
	// selected value remains the same.
	//
	// Retrieve the next value's description.
	values.push_back((*lock->functions)[n+1]->description
			 (description_format::list));

	// Remove the next value
	lock.lm->remove_item(IN_THREAD, n+1);

	// Add it before the currently selected value.
	lock.lm->insert_items(IN_THREAD, n, values);

	// And update the record, accordingly.
	std::swap((*lock->functions)[n+1],
		  (*lock->functions)[n]);

	++*lock.selected_value; // Reflects what just happened
	generator_contents_values_changed(IN_THREAD, lock);

	return true;
}

void appgenerator_functionsObj::generator_value_update_clicked(ONLY IN_THREAD)
{
	generator_value_info lock{this};

	if (!lock)
		return;

	auto n=*lock.selected_value;

	if (n >= (*lock->functions).size())
		return;

	// Our maintained functions are constant, for thread-related issues,
	// so for editing purposes we can simply clone and pass down the
	// existing function.

	auto new_generator_function=(*lock->functions)[n]->clone();
	(*lock->functions)[n]=new_generator_function;

	generator_value_edit
		(IN_THREAD, new_generator_function,
		 &appgenerator_functionsObj::generator_value_updated);
}

void appgenerator_functionsObj
::generator_value_edit(ONLY IN_THREAD,
		       const appgenerator_function &function,
		       void (appgenerator_functionsObj::*save)
		       (ONLY IN_THREAD,
			const appgenerator_function &function))
{
	generator_value_info lock{this};

	if (!lock)
		return;

	// Prepare to open a dialog where the generator function's parameters
	// will get shown and edited.

	auto description=function->description(description_format::title);

	auto dialog_id=std::string{description.string.begin(),
		description.string.end()} + "@cxxwcreator.libcxx.com";

	auto generator_values=appgenerators_values_invoke::create(this);

	// Close the dialog in response to the dialog window's close button,
	// or the "Cancel" button getting pressed. Nothing needs to be done
	// besides closing the modal dialog.

	auto close_value_dialog=
		[generator_values, dialog_id]
		(ONLY IN_THREAD, const auto &busy)
		{
			(*generator_values)
				(IN_THREAD,
				 [&]
				 (auto me)
				{
					me->parent_window->remove_dialog
						(dialog_id);
					return false;
				});
		};

	auto new_dialog=parent_window->create_dialog
		({
			dialog_id,
			true,
			dialog_id
		}, [&, this](const auto &new_dialog)
		{
			// Use the generator function's description string
			// as the dialog window's title, and create it
			// as the "title" UI element.

			new_dialog->dialog_window
				->set_window_title(description.string);

			x::w::uielements elements{
				{
					{"title",
					 [&]
					 (const auto &f)
					 {
						 f->create_label
							 (description);
					 }},
				},
			};

			auto glm=new_dialog->dialog_window->gridlayout();
			glm->generate("generator_edit_title",
				      cxxwui_generators, elements);

			// Let the function object create its UI

			auto validator=function->create_ui(glm);

			// And then gerate the cancel an dok buttons.

			glm->generate("generator_edit_buttons",
				      cxxwui_generators, elements);

			x::w::button cancel_button=
				elements.get_element("generator_edit_cancel");
			x::w::button ok_button=
				elements.get_element("generator_edit_save");

			cancel_button->on_activate
				([close_value_dialog]
				 (ONLY IN_THREAD,
				  const auto &trigger,
				  const auto &mcguffin)
				{
					close_value_dialog(IN_THREAD, mcguffin);
				});

			ok_button->on_activate
				([close_value_dialog, validator, save,
				  function,
				  generator_values]
				 (ONLY IN_THREAD,
				  const auto &trigger,
				  const auto &mcguffin)
				{
					if (!validator())
						return;

					(*generator_values)
						(IN_THREAD,
						 [&]
						 (auto me)
						{
							(*me.*save)(IN_THREAD,
								    function);

							me->generator_values_modified
								(IN_THREAD);
							return true;
						});

					close_value_dialog(IN_THREAD, mcguffin);
				});
		});

	new_dialog->dialog_window->on_delete(close_value_dialog);

	new_dialog->dialog_window->show_all(IN_THREAD);
}

void appgenerator_functionsObj::generator_values_modified(ONLY IN_THREAD)
{
	generator_value_info_lm lock{this};

	generator_contents_values_changed(IN_THREAD, lock);
}

void appgenerator_functionsObj
::generator_value_created(ONLY IN_THREAD,
			  const appgenerator_function &function)
{
	generator_value_info_lm lock{this};

	if (!lock)
		return;

	auto n=*lock.selected_value;

	auto &functions=(*lock->functions);

	if (n > functions.size())
		return;

	// This is a new function.

	functions.insert(functions.begin()+n, function);

	lock.lm->insert_items(IN_THREAD, n,
			      std::vector<x::w::list_item_param>
			      {function->description(description_format::list)}
			      );
}

void appgenerator_functionsObj
::generator_value_updated(ONLY IN_THREAD,
			  const appgenerator_function &function)
{
	// Nothing to do, the function's parameters have been updated in place.
}

bool appgenerator_functionsObj::generator_value_delete_clicked(ONLY IN_THREAD)
{
	generator_value_info_lm lock{this};

	if (!lock)
		return false;

	auto n=*lock.selected_value;

	auto &functions=(*lock->functions);

	if (n >= functions.size())
		return false;

	functions.erase(functions.begin()+n);

	lock.lm->remove_item(IN_THREAD, n);
	generator_contents_values_changed(IN_THREAD, lock);
	return true;
}

std::vector<std::string>
appgenerator_functionsObj::all_generators_for(const char *layout_or_factory,
					      const std::string &type)
{
	std::vector<std::string> ids;

	generator_value_info lock{this};

	if (!lock)
		return ids;

	appinvoke([&]
		  (appObj *me)
	{
		auto this_generator=me->theme.get()->readlock();

		this_generator->get_root();

		for (const auto &id : lock.all->ids)
		{
			auto xpath=me->get_xpath_for(this_generator,
						     layout_or_factory, id);

			if (xpath->count() == 0)
				continue;
			xpath->to_node(1);

			if (this_generator->get_any_attribute("type")
			    == type)
			{
				ids.push_back(id);
			}
		}
	});

	return ids;
}
