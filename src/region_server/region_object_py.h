//********************************************************************
//	created:	24:12:2009   12:54
//	filename: 	region_object_py.h
//	author:		tiamo
//	purpose:	region object python interface
//********************************************************************

#pragma once

//
// get python self to type
//
#define REGION_OBJECT_PYTHON_CHECK_TYPE(self, type, name, ret)													\
	type name																= 0;								\
	do																											\
	{																											\
		if(!self)																								\
		{																										\
			PyErr_Format(PyExc_TypeError, "invalid self pointer(NULL)");										\
			return ret;																							\
		}																										\
		if(!self->m_pObject)																					\
		{																										\
			PyErr_Format(PyExc_TypeError, "invalid self pointer(NULL object)");									\
			return ret;																							\
		}																										\
		name																= (type)self->m_pObject;			\
	}while(0)

//
// region object python interface
//
typedef struct _region_object_python
{
	//
	// header
	//
	PyObject_HEAD

	//
	// region object pointer
	//
	CRegionObject*															m_pObject;
}region_object_python;

//
// object
//
class CPythonRegionObject
{
public:
	//
	// create region object
	//
	static PyObject* CreatePythonObject(CRegionObject* theServer);

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
	// get object id
	//
	static PyObject* GetObjectId(region_object_python* self, void* unused);

	//
	// get object type
	//
	static PyObject* GetObjectType(region_object_python* self, void* unused);

	//
	// get grid x
	//
	static PyObject* GetGridX(region_object_python* self, void* unused);

	//
	// get grid x
	//
	static PyObject* GetGridY(region_object_python* self, void* unused);

	//
	// get grid x
	//
	static PyObject* GetGrid(region_object_python* self, void* unused);

	//
	// set timer
	//
	static PyObject* SetTimer(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// script object state
	//
	static PyObject* GetState(region_object_python* self, PyObject* args);
	static PyObject* SetState(region_object_python* self, PyObject* args);
	static PyObject* AddState(region_object_python* self, PyObject* args);


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

