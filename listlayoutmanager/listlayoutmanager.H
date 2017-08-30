/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutmanager_h
#define listlayoutmanager_h

#include "x/w/listlayoutmanager.H"
#include "x/w/hotspotfwd.H"
#include "x/w/factoryfwd.H"
#include "gridlayoutmanager.H"
#include "listlayoutmanager/listcontainer.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "listlayoutmanager/listitemfactoryfwd.H"
#include "listlayoutmanager/listitemfactoryobj.H"
#include "listlayoutmanager/listitemcontainerfwd.H"
#include <x/functional.H>

#include <tuple>
#include <optional>
#include <utility>

LIBCXXW_NAMESPACE_START

//! The list layout manager implementation object.

//! The list layout manager is really the grid layout manager in disguise.
//!
//! \see listlayoutmanager

class LIBCXX_HIDDEN listlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

	//! Keep track of which row is currently highlighted.

	size_t currently_highlighted_row_=-1;

	//! We also keep track of the individual column too

	//! We highlight all columns in the row, but we track of each
	//! individual column whose focus is being reported as such.

	size_t currently_highlighted_col_=-1;

	//! Which row was highlighted via keyboard focus.
	size_t previously_highlighted_keyboard_focus_row_=-1;

	//! Most recently reported hotspot temperature
	temperature current_temperature_=temperature::cold;


	inline size_t &highlighted_row(grid_map_t::lock &)
	{
		return currently_highlighted_row_;
	};


	inline size_t &highlighted_col(grid_map_t::lock &)
	{
		return currently_highlighted_col_;
	};

	inline size_t &highlighted_keyboard_focus_row(grid_map_t::lock &)
	{
		return previously_highlighted_keyboard_focus_row_;
	}

	temperature &current_temperature(grid_map_t::lock &)
	{
		return current_temperature_;
	}
 public:
	//! Parent container

	const ref<listcontainerObj::implObj> container_impl;

	//! This list layout manager's style.
	const listlayoutstyle &style;

	//! Number of columns
	const size_t columns;

 private:
	//! Selection type
	std::function<void (list_lock &, const listlayoutmanager &,
			    size_t,
			    const x::w::busy &)
		      > selection_type_thread_only;

	std::function<void (list_lock &, const listlayoutmanager &,
			    size_t, bool, const x::w::busy &)
		      > selection_changed_;
 public:
	inline auto &selection_changed(grid_map_t::lock &)
	{
		return selection_changed_;
	};

	//! The factory returned by append_list_item_factory()

	template<typename superclass_t,
		 typename listitemfactory_t>
		class append_factoryObj : public superclass_t,
					  public listitemfactory_t {
	public:

		template<typename ...Args>
		append_factoryObj(const listlayoutmanager &me,
				  Args && ...args)
			: superclass_t(std::forward<Args>(args)...),
			  listitemfactory_t(me)
		{
		}

		//! Destructor
		~append_factoryObj()=default;

		//! Override factoryObj::created()
		void created(const element &e) override
		{
			listitemfactory_t::created_element(e);
		}

		//! Forward create_item() to the append_item() implementation.

		void create_item(const listlayoutstyle::new_list_items_t
				 &new_item)
			override
		{
			this->me->impl->append_item(this->me, new_item);
		}
	};

	//! The factory returned by insert_list_item_factory().

	template<typename superclass_t, typename listitemfactory_t>
		class insert_factoryObj : public superclass_t,
					  public listitemfactory_t {
	public:

		size_t row_number;
		grid_map_t::lock lock;

		//! Constructor

		template<typename ...Args>
		insert_factoryObj(const listlayoutmanager &me,
				  size_t row_number,
				  Args && ...args)
			: superclass_t(std::forward<Args>(args)...),
			  listitemfactory_t(me),
			  row_number(row_number),
			  lock(me->impl->grid_map)
		{
		}

		//! Destructor

		~insert_factoryObj()=default;

		//! Override factoryObj::created()
		void created(const element &e) override
		{
			listitemfactory_t::created_element(e);
		}

		//! Forward create_item() to the insert_item() implementation.

		void create_item(const listlayoutstyle::new_list_items_t
				 &new_item)
			override
		{
			this->me->impl->insert_item(this->me, lock, new_item,
						    row_number++);
		}
	};

	//! The factory returned by replace_list_item_factory().

	template<typename superclass_t, typename listitemfactory_t>
		class replace_factoryObj : public superclass_t,
					   public listitemfactory_t {
	public:

		size_t row_number;
		grid_map_t::lock lock;

		//! Constructor
		template<typename ...Args>
		replace_factoryObj(const listlayoutmanager &me,
				   size_t row_number,
				   Args && ...args)
			: superclass_t(std::forward<Args>(args)...),
			  listitemfactory_t(me),
			  row_number(row_number),
			  lock(me->impl->grid_map)
		{
			me->selected(lock, row_number, false);
		}

		//! Destructor
		~replace_factoryObj()=default;

		//! Override factoryObj::created()
		void created(const element &e) override
		{
			listitemfactory_t::created_element(e);
		}

		//! Forward create_item() to the replace_item() implementation.

		void create_item(const listlayoutstyle::new_list_items_t
				 &new_item)
			override
		{
			this->me->impl->replace_item(this->me, lock, new_item,
						     row_number++);
		}
	};

	//! The factory returned by replace_all_list_items_factory().

	template<typename superclass_t, typename listitemfactory_t>
		class replace_all_factoryObj
			: public append_factoryObj<superclass_t,
						   listitemfactory_t> {

		grid_map_t::lock lock;

	public:

		//! Constructor
		template<typename ...Args>
		replace_all_factoryObj(const listlayoutmanager &me,
				       Args && ...args)
			: append_factoryObj<superclass_t,
					    listitemfactory_t
					    >(me, std::forward<Args>(args)...),
			lock{me->impl->grid_map}
		{
			me->unselect();
			me->impl->remove_all_items(me);
		}

		//! Destructor
		~replace_all_factoryObj() = default;
	};

	THREAD_DATA_ONLY(selection_type);

	//! Constructor
	implObj(const ref<listcontainerObj::implObj> &container_impl,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the listlayoutmanager.

	layoutmanager create_public_object() override;

	//! One of my elements gained or lost pointer focus

	void pointer_focus(IN_THREAD_ONLY,
			   const ref<elementObj::implObj> &e);

	//! List container's hotspot temperature changed

	//! Indicates that the pointer button was pressed or released.
	void temperature_changed(IN_THREAD_ONLY);

	//! I just lost or gained keyboard focus.
	void keyboard_focus(IN_THREAD_ONLY, bool flag);

	//! I just become visible or invisible.
	void inherited_visibility_updated(IN_THREAD_ONLY, bool) override;

	//! Process cursor down/up events.
	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! An list item has been selected.

	void activated(IN_THREAD_ONLY,
		       const listlayoutmanager &my_public_object);

	//! Look up the number of an alleged list item.

	//! Because the layout manager pulls a switcheroo, and wraps
	//! each item in a listitemcontainer, we need to look up its parent
	//! element, instead.
	//!
	//! We get the implementation object of what someone believes to be
	//! an item in the list, and return its current index.
	std::optional<size_t> lookup_item(grid_map_t::lock &lock,
					  const ref<child_elementObj>
					  &item_impl);
 private:

	//! Locate a given item's listitemcontainer, if the item exists.

	static listitemcontainerptr get_listitemcontainer(grid_map_t::lock &l,
							  size_t i);

	//! Refresh the visual representation of a list item.

	//! The list item has been selected or unselected. Update its
	//! visual appearance accordingly.
	void refresh(IN_THREAD_ONLY, grid_map_t::lock &lock, const element &e);

 public:
	//! Return the currently highlighted row.

	size_t get_highlighted_row();

 private:
	//! If currently_highlighted_row, unhighlight it.

	void unhighlight_current_row(IN_THREAD_ONLY,
				     grid_map_t::lock &l);

	//! Set the appropriate background color for the currently highlighted row
	void highlight_current_row(IN_THREAD_ONLY,
				   grid_map_t::lock &l);

	//! Make sure the current row is visible.
	void ensure_current_row_is_visible(IN_THREAD_ONLY,
					   grid_map_t::lock &l);

 public:

	//////////////////////////////////////////////////////////////////////


	//! Number of items in the list
	size_t size(grid_map_t::lock &lock) const;

	//! Whether list item #i is selected right now.
	bool selected(grid_map_t::lock &lock, size_t i) const;

	//! Select or unselect item #i
	void selected(const listlayoutmanager &me,
		      grid_map_t::lock &lock, size_t i, bool selected_flag);

	//! Autoselect, according to selection_type.

	void autoselect(const listlayoutmanager &me,
			grid_map_t::lock &lock, size_t i);

	//! Remove an item
	void remove_item(const listlayoutmanager &me,
			 grid_map_t::lock &lock, size_t i);

	//! Implement append_item()

	void append_item(const listlayoutmanager &me,
			 const listlayoutstyle::new_list_items_t &new_item);

	//! Implement insert_item()

	void insert_item(const listlayoutmanager &me,
			 grid_map_t::lock &lock,
			 const listlayoutstyle::new_list_items_t &new_item,
			 size_t item_number);

	//! Implement replace_item()

	void replace_item(const listlayoutmanager &me,
			  grid_map_t::lock &lock,
			  const listlayoutstyle::new_list_items_t &new_item,
			  size_t item_number);

	//! Called from the public object to remove all existing items in the list.
	void remove_all_items(const listlayoutmanager &me);

	//! Return the given item.
	element item(size_t item_number, size_t column=0);

 private:

	//! Callback used by connection_thread_op_t

	typedef void connection_thread_op_t(implObj *, IN_THREAD_ONLY,
					    const listlayoutmanager &,
					    list_lock &,
					    size_t);

	//! Signature for a class member that can be a connection_thread_op_t

	typedef void (implObj::*connection_thread_method_t)
		(IN_THREAD_ONLY,
		 const listlayoutmanager &,
		 list_lock &,
		 size_t);

	//! Execute an operation on a list item in the connection thread.

	//! This is used by the methods that get invoked from the public
	//! layout manager object which specify an item number.
	//!
	//! The publicly-invoked method passes in a reference to the
	//! public object, an acquired lock, and the item number.
	//!
	//! connection_thread_op() invokes the callback in the connection
	//! thread, with an acquired lock and the item number, so that the
	//! real work can be done.

	void connection_thread_op(const listlayoutmanager &me,
				  grid_map_t::lock &lock, size_t i,
				  const std::function<connection_thread_op_t>
				  &callback);
 public:
	//! Autoselect, according to selection_type.

	void autoselect(IN_THREAD_ONLY, const listlayoutmanager &me,
			list_lock &lock, size_t item_number);

	//! Whether this list item is enabled
	static bool enabled(grid_map_t::lock &, size_t);

	//! Enable/disable list item.

	void enabled(IN_THREAD_ONLY, grid_map_t::lock &, size_t, bool);
};

LIBCXXW_NAMESPACE_END

#endif
