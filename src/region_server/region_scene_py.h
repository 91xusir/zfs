//********************************************************************
//	created:	24:12:2009   15:42
//	filename: 	region_scene_py.h
//	author:		tiamo
//	purpose:	python interface for region scene
//********************************************************************

#pragma once

//
// scene
//
class CPythonRegionScene
{
public:
	//
	// create region object
	//
	static PyObject* CreatePythonObject(CRegionScene* theServer);

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
	// get scene name
	//
	static PyObject* GetSceneName(region_object_python* self, void* unused);

	//
	// get averageLV
	//
	static PyObject* GetAverageLV(region_object_python* self, void* unused);

	//
	// add npc
	//
	static PyObject* AddNpc(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// set default pos
	//
	static PyObject* SetDefaultPos(region_object_python* self, PyObject* args);

	//
	// find creature by id
	//
	static PyObject* FindCreatureById(region_object_python* self, PyObject* args);

	//
	// find npc by type 
	//
	static PyObject* FindNpcByType(region_object_python* self, PyObject* args);

	//
	// remove creature by id
	//
	static PyObject* RemoveCreatureById(region_object_python* self, PyObject* args);

	//
	// Activation area by name
	//
	static PyObject* ActivationAreaByName(region_object_python* self, PyObject* args);

	static PyObject* BroadcastShowTimer(region_object_python* self, PyObject* args, PyObject* kw);
	static PyObject* BroadcastSceneData(region_object_python* self, PyObject* args, PyObject* kw);
	static PyObject* CloseDungeon(region_object_python* self, PyObject* args);

	static PyObject* FBSceneUser(region_object_python* self, PyObject* args);
	static PyObject* FBSceneBroadcast(region_object_python* self, PyObject* args);

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

