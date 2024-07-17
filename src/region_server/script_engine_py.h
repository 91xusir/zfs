//********************************************************************
//	created:	24:12:2009   14:09
//	filename: 	script_engine_py.h
//	author:		tiamo
//	purpose:	script engine
//********************************************************************

#pragma once

#include <Python.h>
#define	SCRIPT_PYTHON_MODULE_NAME											"EFun"

//
// script engine
//
class CScriptEngine
{
public:
	//
	// constructor
	//
	CScriptEngine();

	//
	// destructor
	//
	~CScriptEngine();

	//
	// start
	//
	bool Start();

	//
	// stop
	//
	void Stop();

	//
	// build call args
	//
	PyObject* BuildCallArgs(int argc)																{return PyTuple_New(argc);}

	//
	// build int
	//
	void BuildArg(PyObject* args, int index, int arg)												{BuildArg(args, index, Py_BuildValue("i", arg));}

	//
	// build int
	//
	void BuildArg(PyObject* args, int index, unsigned int arg)										{BuildArg(args, index, static_cast<int>(arg));}

	//
	// build long
	//
	void BuildArg(PyObject* args, int index, long arg)												{BuildArg(args, index, Py_BuildValue("l", arg));}

	//
	// build long
	//
	void BuildArg(PyObject* args, int index, unsigned long arg)										{BuildArg(args, index, static_cast<long>(arg));}
	
	//
	// build string
	//
	void BuildArg(PyObject* args, int index, char const* arg)										{BuildArg(args, index, Py_BuildValue("s", arg));}

	//
	// build float
	//
	void BuildArg(PyObject* args, int index, float arg)												{BuildArg(args, index, Py_BuildValue("f", arg));}

	//
	// build region object
	//
	void BuildArg(PyObject* args, int index, CRegionObject* arg)									{BuildArg(args, index, CreatePythonObject(arg));}

	//
	// build python object
	//
	void BuildArg(PyObject* args, int index, PyObject* arg)											{PyTuple_SetItem(args, index, arg);}

	//
	// get singleton
	//
	static CScriptEngine* GetSingleton();

	//
	// create python object
	//
	static PyObject* CreatePythonObject(CRegionObject* theObject);

	//
	// create python object
	//
	static PyObject* CreatePythonObject(SItemID const& item);

	//
	// return none
	//
	static PyObject* ReturnNone();

private:
	//
	// log message
	//
	static PyObject* LogMessage(PyObject* self, PyObject* args);

	//
	// server time
	//
	static PyObject* ServerTime(PyObject* self, PyObject* args);

	//
	// return Random value x
	//
	static PyObject* RandomValue(PyObject* self, PyObject* args);

	//
	// Broadcast msg to all
	//
	static PyObject* BroadcastBulletin(PyObject* self, PyObject* args);

	static PyObject* BroadcastHTML(PyObject* self, PyObject* args);

private:
	//
	// singleton
	//
	static CScriptEngine*													m_pSingleton;

	//
	// module
	//
	PyObject*																m_pModule;
};

