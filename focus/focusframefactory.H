/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframefactory_h
#define focusframefactory_h

#include "x/w/factory.H"
#include "x/w/container.H"
#include "x/w/elementfwd.H"
#include "x/w/gridlayoutmanagerfwd.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN focusframefactoryObj : public factoryObj {

 public:

	const gridlayoutmanager glm;

	focusframefactoryObj(const container &ffc);

	void created(const element &e) override;

	~focusframefactoryObj();
};

typedef ref<focusframefactoryObj> focusframefactory;

LIBCXXW_NAMESPACE_END

#endif
