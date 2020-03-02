/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef pagelayoutmanager_impl_h
#define pagelayoutmanager_impl_h

#include "x/w/pagelayoutmanager.H"
#include "x/w/containerfwd.H"
#include "x/w/alignment.H"
#include "x/w/impl/layoutmanager.H"
#include <optional>
#include <unordered_map>
#include <x/mpobj.H>
#include <x/refptr_hash.H>

LIBCXXW_NAMESPACE_START

//! An element controlled by the \ref pagelayoutmanager "pagelayoutmanager".

struct LIBCXX_HIDDEN switch_element_info {

	//! The internally-create container wrapper, for layout manager's use.
	container the_container;

	//! The official element, that was created by the factory.

	//! This is the element in the singletonlayoutmanager that the_container
	//! uses.

	element the_element;

	//! Align this element accordingly.
	halign horizontal_alignment;

	//! Align this element accordingly.
	valign vertical_alignment;
};

//! Mutex-protected metadata used by the \ref pagelayoutmanager "pagelayoutmanager".

struct LIBCXX_HIDDEN page_layout_info_s {

	//! Current elements being switched.
	std::vector<switch_element_info> elements;

	//! Look up the_element's index.

	std::unordered_map<element, size_t> element_index;

	//! Which element is visible now.
	std::optional<size_t> current_element;
};

//! Internal implementation object.

class LIBCXX_HIDDEN pagelayoutmanagerObj::implObj
	: public layoutmanagerObj::implObj {

 public:

	//! Mutex-protected metadata.
	page_layout_info_t info;

	//! Constructor
	implObj(const container_impl &container_impl);

	//! Destructor
	~implObj();

	//! Number of pages()

	size_t pages();

	//! Add new element.
	void append(const switch_element_info &);

	//! Add new element.
	void insert(size_t, const switch_element_info &);

	//! Remove an element.
	void remove(size_t);

 private:
	//! Update element_index, after append/insert/remove.
	void rebuild_index(page_layout_info_t::lock &lock);
 public:
	//! Implement do_for_each_child()

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void(const element &)> &)
		override;

	//! Implement num_children().
	size_t num_children(ONLY IN_THREAD) override;

	//! Implement create_public_object().

	//! Call create_pagelayoutmanager().
	layoutmanager create_public_object() override;

	//! Create my page layout manager.
	pagelayoutmanager create_pagelayoutmanager();

	//! Implement recalculate()
	void recalculate(ONLY IN_THREAD) override;

	//! Implement process_updated_position()
	void process_updated_position(ONLY IN_THREAD,
				      const rectangle &position) override;
};

LIBCXXW_NAMESPACE_END

#endif
