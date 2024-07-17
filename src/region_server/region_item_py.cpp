//********************************************************************
//	created:	26:12:2009   13:46
//	filename: 	region_item_py.cpp
//	author:		tiamo
//	purpose:	item interface
//********************************************************************

#include "region.h"
#include "region_item_py.h"
#include "script_engine_py.h"

//
// get python self to type
//
#define _CHECK_ITEM_TYPE(ret)																					\
	SItemID* selfItem														= 0;								\
	do																											\
	{																											\
		if(!self)																								\
		{																										\
			PyErr_Format(PyExc_TypeError, "invalid self pointer(NULL)");										\
			return ret;																							\
		}																										\
		selfItem															= &self->m_item;					\
	}while(0)

//
// get object id
//
PyObject* CPythonRegionItem::GetItemId(region_item_python* self, void* unused)
{
	_CHECK_ITEM_TYPE(0);
	return Py_BuildValue("l", selfItem->id);
}

//
// get object type
//
PyObject* CPythonRegionItem::GetItemType(region_item_python* self, void* unused)
{
	_CHECK_ITEM_TYPE(0);
	return Py_BuildValue("l", CItemManager::GetIndexFromType(selfItem->type));
}

//
// init region object type
//
bool CPythonRegionItem::Initialize(PyObject* module)
{
	//
	// init region object type
	//
	if(PyType_Ready(&m_pythonType) < 0)
		return false;

	//
	// add item color
	//
	if(PyModule_AddIntConstant(module, "ItemColor_White", ItemColor_White) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "ItemColor_Green", ItemColor_Green) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "ItemColor_Blue", ItemColor_Blue) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "ItemColor_Golden", ItemColor_Golden) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "ItemColor_Purple", ItemColor_Purple) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "ItemColor_All", ItemColor_All) == -1)
		return false;

	//
	// add bind
	//
	if(PyModule_AddIntConstant(module, "IB_NotBind", IB_NotBind) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "IB_Binded", IB_Binded) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "IB_BindWhenPick", IB_BindWhenPick) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "IB_BindWhenEquip", IB_BindWhenEquip) == -1)
		return false;

	return true;
}

//
// create new
//
PyObject* CPythonRegionItem::CreatePythonObject(SItemID const& theItem)
{
	region_item_python* ret	= PyObject_NEW(region_item_python, &m_pythonType);
	if(!ret)
		return 0;

	//添加的memcpy并非多此一举，ret->m_item为引用
	//Python里New一个对象是用malloc分配对象内存
	//没有memcpy此步，且ret->m_item里有对象成员时一定会出问题
	SItemID item;
	memcpy(&(ret->m_item), &item, sizeof(SItemID));
	ret->m_item	= theItem;
	return reinterpret_cast<PyObject*>(ret);
}

//
// method
//
PyMethodDef CPythonRegionItem::m_pMethods[] = 
{
	{0, 0},
};

//
// attribute
//
PyGetSetDef CPythonRegionItem::m_pAttributes[] = 
{
	{(char*)"id",					(getter)CPythonRegionItem::GetItemId,						0},
	{(char*)"type",					(getter)CPythonRegionItem::GetItemType,						0},

	{0, 0},
};

//
// region object type
//
PyTypeObject CPythonRegionItem::m_pythonType = 
{
	PyObject_HEAD_INIT(NULL)
	0,																	/*ob_size*/
	SCRIPT_PYTHON_MODULE_NAME ".RegionItem",							/*tp_name*/
	sizeof(region_item_python),											/*tp_basicsize*/
	0,																	/*tp_itemsize*/
	0,																	/*tp_dealloc*/
	0,																	/*tp_print*/
	0,																	/*tp_getattr*/
	0,																	/*tp_setattr*/
	0,																	/*tp_compare*/
	0,																	/*tp_repr*/
	0,																	/*tp_as_number*/
	0,																	/*tp_as_sequence*/
	0,																	/*tp_as_mapping*/
	0,																	/*tp_hash*/
	0,																	/*tp_call*/
	0,																	/*tp_str*/
	PyObject_GenericGetAttr,											/*tp_getattro*/
	PyObject_GenericSetAttr,											/*tp_setattro*/
	0,																	/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,													/*tp_flags*/
	0,																	/*tp_doc*/
	0,																	/*tp_traverse*/
	0,																	/*tp_clear*/
	0,																	/*tp_richcompare*/
	0,																	/*tp_weaklistoffset*/
	0,																	/*tp_iter*/
	0,																	/*tp_iternext*/
	CPythonRegionItem::m_pMethods,										/*tp_methods*/
	0,																	/*tp_members*/
	CPythonRegionItem::m_pAttributes,									/*tp_getset*/
	0,																	/*tp_base*/
	0,																	/*tp_dict*/
	0,																	/*tp_descr_get*/
	0,																	/*tp_descr_set*/
	0,																	/*tp_dictoffset*/
	0,																	/*tp_init*/
	0,																	/*tp_alloc*/
	0,																	/*tp_new*/
	0,																	/*tp_free*/
	0,																	/*tp_is_gc*/
};
