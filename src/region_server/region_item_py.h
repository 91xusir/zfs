//********************************************************************
//	created:	26:12:2009   13:38
//	filename: 	region_item_py.h
//	author:		tiamo
//	purpose:	item
//********************************************************************

#pragma once

//
// region item python interface
//
typedef struct _region_item_python
{
	//
	// header
	//
	PyObject_HEAD

	//
	// region item
	//
	SItemID															m_item;
}region_item_python;

//
// item
//
class CPythonRegionItem
{
public:
	//
	// create region object
	//
	static PyObject* CreatePythonObject(SItemID const& theItem);

	//
	// get type
	//
	static PyTypeObject* GetPythonType()													{return &m_pythonType;}

	//
	// init
	//
	static bool Initialize(PyObject* module);

private:
	//
	// get id
	//
	static PyObject* GetItemId(region_item_python* self, void* unused);

	//
	// get item type
	//
	static PyObject* GetItemType(region_item_python* self, void* unused);

private:
	//
	// type
	//
	static PyTypeObject														m_pythonType;

	//
	// attribute
	//
	static PyGetSetDef														m_pAttributes[];

	//
	// method
	//
	static PyMethodDef														m_pMethods[];
};


