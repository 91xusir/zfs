#pragma once

#include "RegionActManager.h"

class CPythonRegionActivity
{
public:
	//
	// create region object
	//
	static PyObject* CreatePythonObject(CRegionActivityBase* theActivity);

	//
	// get type
	//
	static PyTypeObject* GetPythonType()	{return &m_pythonType;}

	//
	// init
	//
	static bool Initialize(PyObject* module);


private:

	static PyObject* GetScene(region_object_python* self, void* unused);

	static PyObject* AddLinkNPC(region_object_python* self, PyObject* args);
	static PyObject* GetLinkNPC(region_object_python* self, PyObject* args);
	static PyObject* GetAllLinkNPC(region_object_python* self, PyObject* args);

	static PyObject* BroadcastShowTimer(region_object_python* self, PyObject* args);
	static PyObject* BroadcastSceneData(region_object_python* self, PyObject* args);

private:
	//
	// type
	//
	static PyTypeObject	m_pythonType;

	//
	// attribute
	//
	static PyGetSetDef	m_pAttributes[];

	//
	// method
	//
	static PyMethodDef	m_pMethods[];
};
