/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemhandle_impl_h
#define listitemhandle_impl_h

#include "x/w/listitemhandle.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/extra_list_row_infofwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class listitemhandleObj::implObj : public listitemhandleObj {

	weakptr<ptr<listlayoutmanagerObj::implObj>> listlayout_impl;

	const extra_list_row_info extra;

 public:
	//! Constructor
	implObj(const ref<listlayoutmanagerObj::implObj> &impl,
		size_t n);

	//! Constructor
	implObj(const ref<listlayoutmanagerObj::implObj> &impl,
		const extra_list_row_info &extra);

	//! Destructor
	~implObj();

	//! Implement enabled()
	bool enabled() override;

	//! Implement enabled()
	void enabled(bool flag) override;

	//! Implement enabled()
	void enabled(ONLY IN_THREAD, bool) override;

};

LIBCXXW_NAMESPACE_END

#endif
