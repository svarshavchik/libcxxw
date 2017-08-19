/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemfactoryfwd_h
#define listitemfactoryfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class listitemfactoryObj;

//! A list item factory mixin.

//! A list item contains 1 or more elements, as specified by the list
//! layout manager.
//!
//! The list item factory mixin obediently collects the requisite number of
//! elements, for each column of the new list item. When all columns'
//! elements are collected, create_factory() gets created to create the
//! factory for the new row in the grid layout manager, then uses the
//! listlayoutstyle to create the new item.
//!
//! This mixin gets combined with a factoryObj, with created() overridden
//! to invoke created_element(). Once a requisite number of elements gets
//! collected, create_item() gets invoked.

typedef ref<listitemfactoryObj> listitemfactory;

LIBCXXW_NAMESPACE_END

#endif
