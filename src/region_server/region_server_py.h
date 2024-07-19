//********************************************************************
//	created:	24:12:2009   15:00
//	filename: 	region_server_py.h
//	author:		tiamo
//	purpose:	python interface for region server
//********************************************************************

#pragma once

//
// region server
//
class CPythonRegionServer
{
public:
	//
	// create region server
	//
	static PyObject* CreatePythonObject(CRegionServer* theServer);

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
	// broadcast 
	//
	static PyObject* Broadcast(region_object_python* self, PyObject* args);

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

