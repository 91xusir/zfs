//********************************************************************
//	created:	24:12:2009   13:22
//	filename: 	region_creature_py.cpp
//	author:		tiamo
//	purpose:	region creature
//********************************************************************

#include "region.h"
#include "PlayerLogic.h"
#include "script_engine_py.h"
#include "region_object_py.h"
#include "region_creature_py.h"
#include "RegionActManager.h"

extern CRS_CreModel    g_TableCreModel;

#define _CHECK_CREATURE_TYPE(ret)											REGION_OBJECT_PYTHON_CHECK_TYPE(self, CRegionCreature*, selfCreature, ret)
#define	MAX_HAS_TASK_COUNT	20

//
// get name
//
PyObject* CPythonRegionCreature::GetName(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("s", selfCreature->m_core.Name.c_str());
}

//
// get id
//
PyObject* CPythonRegionCreature::GetID(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->GetObjectId());
}

//
// get level
//
PyObject* CPythonRegionCreature::GetLevel(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Lev);
}

//
// get exp
//
PyObject* CPythonRegionCreature::GetExp(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Exp);
}

//
// get cul
//
PyObject* CPythonRegionCreature::GetCulLV(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.GradeLev);
}
PyObject* CPythonRegionCreature::GetCul(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Cul);
}
int CPythonRegionCreature::SetCul(region_object_python* self, PyObject* cul)
{
	_CHECK_CREATURE_TYPE(-1);
	long lSetExp = PyInt_AsLong(cul);

	if(lSetExp >= 0)
	{
		selfCreature->AddCulExp(lSetExp - selfCreature->m_core.CulExp);
	}
	else	selfCreature->AddCulExp(lSetExp);
	return 0;
}

//
// get soul
//
PyObject* CPythonRegionCreature::GetSoul(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Soul);
}
int CPythonRegionCreature::SetSoul(region_object_python* self, PyObject* soul)
{
	long lSoul = PyInt_AsLong(soul);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lSoul - selfCreature->m_core.Soul;
	selfCreature->m_core.SetSoul(lSoul);
	selfCreature->AttChangeMessageToClient(lGap > 0 ? "MSG_GET_Soul" : "MSG_LOST_Soul", lGap);
	return 0;
}

//
// get credit
//
PyObject* CPythonRegionCreature::GetCredit(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Credit);
}
int CPythonRegionCreature::SetCredit(region_object_python* self, PyObject* credit)
{
	long lCredit = PyInt_AsLong(credit);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lCredit - selfCreature->m_core.Credit;
	selfCreature->AddCredit(lGap);
	return 0;
}

//
// get contribute
//
PyObject* CPythonRegionCreature::GetContribute(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Contribute);
}
int CPythonRegionCreature::SetContribute(region_object_python* self, PyObject* contribute)
{
	long lContribute = PyInt_AsLong(contribute);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lContribute - selfCreature->m_core.Contribute;
	selfCreature->m_core.SetContribute(lContribute);
	selfCreature->AttChangeMessageToClient(lGap > 0 ? "MSG_ADD_CONTRIBUTE" : "MSG_DEL_CONTRIBUTE", lGap);
	return 0;
}

//
// get point
//
PyObject* CPythonRegionCreature::GetPoint(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Point);
}
int CPythonRegionCreature::SetPoint(region_object_python* self, PyObject* point)
{
	long lPoint = PyInt_AsLong(point);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lPoint - selfCreature->m_core.Point;
	if(lGap == 0)
		return 0;
	selfCreature->m_core.SetPoint(lPoint);

	selfCreature->AttChangeMessageToClient(lGap > 0 ? "MSG_JIFEN_GET" : "MSG_JIFEN_LOSE", lGap);
	return 0;
}

//
// get pk
//
PyObject* CPythonRegionCreature::GetPK(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Pk);
}
int CPythonRegionCreature::SetPK(region_object_python* self, PyObject* pk)
{
	long lPK = PyInt_AsLong(pk);
	_CHECK_CREATURE_TYPE(0);
	selfCreature->m_core.SetPKValue(lPK);
	return 0;
}
PyObject* CPythonRegionCreature::AddPK(region_object_python* self, PyObject* pk)
{
	long lPK = PyInt_AsLong(pk);
	_CHECK_CREATURE_TYPE(0);
	selfCreature->m_core.AddPKValue(lPK);
	return CScriptEngine::ReturnNone();
}

//
// get record
//
PyObject* CPythonRegionCreature::GetRecord(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Record);
}
int CPythonRegionCreature::SetRecord(region_object_python* self, PyObject* record)
{
	long lRecord = PyInt_AsLong(record);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lRecord - selfCreature->m_core.Record;
	selfCreature->m_core.SetRecord(lRecord);
	selfCreature->AttChangeMessageToClient(lGap > 0 ? "MSG_ADD_RECORD" : "MSG_DEL_RECORD", lGap);
	return 0;
}

//
// get union record
//
PyObject* CPythonRegionCreature::GetUnionRecord(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.UnionRecord);
}
int CPythonRegionCreature::SetUnionRecord(region_object_python* self, PyObject* unionrecord)
{
	long lUnionRecord = PyInt_AsLong(unionrecord);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lUnionRecord - selfCreature->m_core.UnionRecord;
	//selfCreature->m_core.SetUnionRecord(lUnionRecord);
	selfCreature->GetPlayerLogic()->AddUnionRecord(lGap);
	selfCreature->AttChangeMessageToClient(lGap > 0 ? "MSG_ADD_UNIONRECORD" : "MSG_DEL_UNIONRECORD", lGap);
	return 0;
}

//
// get job record
//
PyObject* CPythonRegionCreature::GetJobRecord(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.JobRecord);
}
int CPythonRegionCreature::SetJobRecord(region_object_python* self, PyObject* jobrecord)
{
	long lJobRecord = PyInt_AsLong(jobrecord);
	_CHECK_CREATURE_TYPE(-1);
	long lGap = lJobRecord - selfCreature->m_core.JobRecord;
	selfCreature->m_core.SetJobRecord(lJobRecord);
	//selfCreature->GetPlayerLogic()->AddJobRecord(lGap);
	selfCreature->AttChangeMessageToClient(lGap > 0 ? "MSG_ADD_JOBRECORD" : "MSG_DEL_JOBRECORD", lGap);
	return 0;
}

//
// get metier
//
PyObject* CPythonRegionCreature::GetMetier(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.Metier);
}

//
// get born tag
//
PyObject* CPythonRegionCreature::GetBornTag(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_core.BornTag);
}

//
// get hp
//
PyObject* CPythonRegionCreature::GetHp(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.GetHp());
}

//
// get max hp
//
PyObject* CPythonRegionCreature::GetMaxHp(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.GetMaxHp());
}

//
// set hp
//
int CPythonRegionCreature::SetHp(region_object_python* self, PyObject* hp)
{
	_CHECK_CREATURE_TYPE(-1);
	selfCreature->m_core.SetHp((int)PyInt_AsLong(hp));
	return 0;
}

//
// get mp
//
PyObject* CPythonRegionCreature::GetMp(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.GetMp());
}

//
// get max mp
//
PyObject* CPythonRegionCreature::GetMaxMp(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.GetMaxMp());
}

//
// set mp
//
int CPythonRegionCreature::SetMp(region_object_python* self, PyObject* mp)
{
	_CHECK_CREATURE_TYPE(-1);
	selfCreature->m_core.SetMp((int)PyInt_AsLong(mp));
	return 0;
}

PyObject* CPythonRegionCreature::GetAttack(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.Attack.Base);
}
int CPythonRegionCreature::SetAttack(region_object_python* self, PyObject* attack)
{
	_CHECK_CREATURE_TYPE(-1);
	long lValue = PyInt_AsLong(attack);
	if(lValue > 0)
		selfCreature->m_core.Attack.Base = lValue;
	return 0;
}

PyObject* CPythonRegionCreature::GetDamage(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	short damageMin = selfCreature->m_core.DamageMin.Base;
	short damageMax = selfCreature->m_core.DamageMax.Base;
	return Py_BuildValue("l", (damageMax - damageMin) / 2 + damageMin);
}
int CPythonRegionCreature::SetDamage(region_object_python* self, PyObject* damage)
{
	_CHECK_CREATURE_TYPE(-1);
	long lValue = PyInt_AsLong(damage);
	if(lValue > 0)
	{
		selfCreature->m_core.DamageMin.Base = lValue * 0.9f;
		selfCreature->m_core.DamageMax.Base = lValue * 1.1f;
	}
	return 0;
}

PyObject* CPythonRegionCreature::GetDodge(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.Dodge.Base);
}
int CPythonRegionCreature::SetDodge(region_object_python* self, PyObject* dodge)
{
	_CHECK_CREATURE_TYPE(-1);
	long lValue = PyInt_AsLong(dodge);
	if(lValue > 0)
		selfCreature->m_core.Dodge.Base = lValue;
	return 0;
}

PyObject* CPythonRegionCreature::GetArmor(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.Armor.Base);
}
int CPythonRegionCreature::SetArmor(region_object_python* self, PyObject* armor)
{
	_CHECK_CREATURE_TYPE(-1);
	long lValue = PyInt_AsLong(armor);
	if(lValue > 0)
		selfCreature->m_core.Armor.Base = lValue;
	return 0;
}

//
// get sex
//
PyObject* CPythonRegionCreature::GetSex(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->m_core.Sex);
}

//
// get pos
//
PyObject* CPythonRegionCreature::GetPos(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("(iii)", selfCreature->m_pos[0], selfCreature->m_pos[1], selfCreature->m_pos[2]);
}

//
// get tile pos
//
PyObject* CPythonRegionCreature::GetTilePos(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	int x																	= 0;
	int y																	= 0;
	selfCreature->m_scene->m_pTerrain->GetTerrainTile(selfCreature->m_pos[0], selfCreature->m_pos[1], x, y);
	return Py_BuildValue("(ii)", x, y);
}

//
// get dir
//
PyObject* CPythonRegionCreature::GetDir(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_dir);
}

//
// is captain
//
PyObject* CPythonRegionCreature::GetIsCaptain(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->m_userInfo && selfCreature->m_pTeam)
		return Py_BuildValue("i", (selfCreature->m_pTeam->mData.GetCaptain() == selfCreature->m_userInfo->m_userId) ? 1 : 0);
	return Py_BuildValue("i", 0);
}

//
// is team member
//
PyObject* CPythonRegionCreature::GetIsTeamMember(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	long lreturn = 0;

	if (selfCreature->m_pTeam && selfCreature->m_pTeam->mData.Size() != 0)
	{
		lreturn = 1;
	}

	return Py_BuildValue("i", lreturn);
}

//
// team member count
//
PyObject* CPythonRegionCreature::GetTeamMemberCount(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	long lcount = 0;

	if (selfCreature->m_pTeam)
	{
		lcount = selfCreature->m_pTeam->mData.Size();
	}

	return Py_BuildValue("i", lcount);
}

//
// team max min level
//
PyObject* CPythonRegionCreature::GetTeamLevel(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	long minLevel															= 1000;
	long maxLevel															= 0;
	long averageLevel														= 0;
	if(selfCreature->m_pTeam)
	{
		int count															= selfCreature->m_pTeam->mData.Size();

		if (count != 0)
		{
			for(int i = 0; i < count; i ++)
			{
				CRegionUser* theUser											= g_region->FindUser(selfCreature->m_pTeam->mData[i].mDBID, true);
				if(!theUser)
					continue;

				if(theUser->m_dummy->m_core.Lev > maxLevel)
					maxLevel													= theUser->m_dummy->m_core.Lev;

				if(theUser->m_dummy->m_core.Lev < minLevel)
					minLevel													= theUser->m_dummy->m_core.Lev;

				averageLevel += theUser->m_dummy->m_core.Lev;
			}

			averageLevel /= count;
		}
	}

	return Py_BuildValue("(iii)", minLevel, averageLevel, maxLevel);
}

//
// get scene
//
PyObject* CPythonRegionCreature::GetScene(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return CScriptEngine::CreatePythonObject(selfCreature->m_scene);
}

//
// get task count
//
PyObject* CPythonRegionCreature::GetTaskCount(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_task.GetTaskAcceptNum());
}

//
// get master id
//
PyObject* CPythonRegionCreature::GetMasterID(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_master ? selfCreature->m_master->GetObjectId() : 0);
}

//
// get task id s
//
PyObject* CPythonRegionCreature::HasTaskID(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	std::vector<int> oIDList;
	int iTaskCount = selfCreature->m_task.GetTaskInfosForID(oIDList);

	CScriptEngine* theEngine = CScriptEngine::GetSingleton();
	PyObject* pRet = theEngine->BuildCallArgs(iTaskCount + 1);
	//返回列表中第1个值为任务总数
	theEngine->BuildArg(pRet, 0, iTaskCount);
	for(int ii=0; ii<iTaskCount; ii++)
		theEngine->BuildArg(pRet, ii+1, oIDList[ii]);

	return pRet;
}

//
// is player
//
PyObject* CPythonRegionCreature::IsPlayer(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->IsUser() ? 1 : 0);
}

//
// is tour
//
PyObject* CPythonRegionCreature::IsTourNPC(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	int iIsTour = 0;
	if(!selfCreature->IsUser() && selfCreature->m_ai &&
		selfCreature->m_ai->GetType() == CRegionAI::TYPE_TOUR)
		iIsTour = 1;
	return Py_BuildValue("i", iIsTour);
}

//
// money
//
PyObject* CPythonRegionCreature::GetMoney(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	long lMoney = 0;
	if(selfCreature->IsUser() && selfCreature->m_pItemContainer)
		lMoney = selfCreature->m_pItemContainer->GetMoney();
	return Py_BuildValue("i", lMoney);
}
int CPythonRegionCreature::SetMoney(region_object_python* self, PyObject* money)
{
	long lMoney = PyInt_AsLong(money);
	_CHECK_CREATURE_TYPE(-1);
	if(lMoney != 0 && selfCreature->IsUser() && selfCreature->m_pItemContainer)
	{
		selfCreature->m_pItemContainer->SetMoney(lMoney);
		CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &(selfCreature->m_pItemContainer->m_Bag));
		selfCreature->RecvCmd(g_sendCmd);
	}
	return 0;
}
PyObject* CPythonRegionCreature::AddMoney(region_object_python* self, PyObject* money)
{
	long lMoney = PyInt_AsLong(money);
	_CHECK_CREATURE_TYPE(0);
	if(lMoney != 0 && selfCreature->IsUser() && selfCreature->m_pItemContainer)
	{
		if(lMoney > 0)
			selfCreature->m_pItemContainer->AddMoney(lMoney);
		else
			selfCreature->m_pItemContainer->RemoveMoney(lMoney);
		CItemCmdBuilder_Svr::Build_r2c_bag_refresh(ITEM_CONTAINER_PLAYER_COMMON_BAG, &(selfCreature->m_pItemContainer->m_Bag));
		selfCreature->RecvCmd(g_sendCmd);
	}
	return CScriptEngine::ReturnNone();
}

//
// union
//
PyObject* CPythonRegionCreature::GetUnionMember(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->IsUser() && selfCreature->m_unionID > 0)
		return Py_BuildValue("i", selfCreature->m_unionID);
	return Py_BuildValue("i", 0);
}

PyObject* CPythonRegionCreature::TeamUnionMember(region_object_python* self, void* unused)
{
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return Py_BuildValue("i", 0);
	DWORD dwUnionID = selfCreature->m_unionID;
	if(dwUnionID <= 0)
		return Py_BuildValue("i", 0);

	int iMemberCount = selfCreature->m_pTeam->mData.Size();
	if(iMemberCount > 0)
	{
		for(int ii=0; ii<iMemberCount; ii++)
		{
			CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
			if(pUser && pUser->m_dummy)
			{
				if(pUser->m_dummy->m_unionID != dwUnionID)
					return Py_BuildValue("i", 0);
			}
		}
		return Py_BuildValue("i", dwUnionID);
	}

	return Py_BuildValue("i", selfCreature->m_unionID > 0 ? selfCreature->m_unionID : 0);
}

PyObject* CPythonRegionCreature::GetFuncState(region_object_python* self, PyObject* args)
{
	long lIndex = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return Py_BuildValue("i", 0);
	if(lIndex < 0 || lIndex >= SCRIPT_FUNC_STATE_COUNT)
		return Py_BuildValue("i", 0);
	return Py_BuildValue("i", selfCreature->m_userInfo->m_lFunctionState[lIndex]);
}

PyObject* CPythonRegionCreature::SetFuncState(region_object_python* self, PyObject* args)
{
	long lIndex = 0;
	long lValue = 0;
	if(!PyArg_ParseTuple(args, "ll", &lIndex, &lValue))
		return PyErr_Format(PyExc_TypeError, "[SetFuncState] invalid params");
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return Py_BuildValue("i", 0);
	if(lIndex < 0 || lIndex >= SCRIPT_FUNC_STATE_COUNT)
		return Py_BuildValue("i", 0);
	selfCreature->m_userInfo->m_lFunctionState[lIndex] = lValue;
	return Py_BuildValue("i", 1);
}

//
// talk(channel, msg, to=name, title=gm_title)
//
PyObject* CPythonRegionCreature::Talk(region_object_python* self, PyObject* args, PyObject* kw)
{
	//
	// get param
	//
	long channel															= 0;
	char* msg																= 0;
	char* target															= 0;
	char* title																= 0;
	char* picName															= 0;
	static char const* keywords[]													= {"c", "m", "to", "title", "pic", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "ls|sss:Talk", (char**)keywords, &channel, &msg, &target, &title, &picName))
		return PyErr_Format(PyExc_TypeError, "[Talk] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(channel == CHAT_CHANNEL_SYSTEM)
	{
		char szMessage[256] = {0};
		rt2_snprintf(szMessage, 256, "<localfile://face/%s.tgalocalfile/> %s",
			(picName && picName[0] != 0) ? picName : "sysmsg", msg);
		selfCreature->SendSystemMessage(szMessage, NULL);
	}
	else
		selfCreature->CommonChat(channel, msg, target, title);
	return CScriptEngine::ReturnNone();
}

//
// moveto(x,y)
//
PyObject* CPythonRegionCreature::MoveTo(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long x																	= 0;
	long y																	= 0;
	if(!PyArg_ParseTuple(args, "ll", &x, &y))
		return PyErr_Format(PyExc_TypeError, "[MoveTo] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	g_region->m_gws.MoveUserToPosition(selfCreature->m_userInfo->m_userId, 0, x, y);
	return CScriptEngine::ReturnNone();
}

//
// get count
//
PyObject* CPythonRegionCreature::GetItemCountByType(region_object_python* self, PyObject* args, PyObject* kw)
{
	//
	// get param
	//
	long typeId																= 0;
	EItemColor color														= ItemColor_All;
	static char const* keywords[]											= {"typeId", "color", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "l|l:GetItemCountByType", (char**)keywords, &typeId, &color))
		return PyErr_Format(PyExc_TypeError, "[GetItemCountByType] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->GetItemNum(typeId, color));
}

//
// delete item by type
//
PyObject* CPythonRegionCreature::DeleteItemByType(region_object_python* self, PyObject* args, PyObject* kw)
{
	//
	// get param
	//
	long typeId																= 0;
	long count																= 1;
	EItemColor color														= ItemColor_All;
	static char const* keywords[]											= {"typeId", "count", "color", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "l|ll:DeleteItemByType", (char**)keywords, &typeId, &count, &color))
		return PyErr_Format(PyExc_TypeError, "[DeleteItemByType] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("l", selfCreature->RemoveItem(typeId, count, color));
}

//
// delete item by id
//
PyObject* CPythonRegionCreature::DeleteItemById(region_object_python* self, PyObject* args)
{
	//
	// get id
	//
	long id																	= PyInt_AsLong(args);

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->RemoveItem(id, 0, 0))
		if (!selfCreature->RemoveItem(id, 1, 0))
			selfCreature->RemoveItem(id, 2, 0);

	return CScriptEngine::ReturnNone();
}

//
// check equipment
//
PyObject* CPythonRegionCreature::CheckEquipment(region_object_python* self, PyObject* args, PyObject* kw)
{
	long typeId																= 0;
	long level																= 0;
	EItemColor color														= ItemColor_All;
	static char const* keywords[]											= {"typeId", "color", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "l|l:CheckEquipment", (char**)keywords, &typeId, &color))
		return PyErr_Format(PyExc_TypeError, "[CheckEquipment] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	CItemContainerBase::EEquipParts part									= CItemContainerBase::EQUIP_DEFAULT_PART;
	SItemID item															= selfCreature->CheckEquipment(typeId, color, &part);
	return Py_BuildValue("(Oi)", CScriptEngine::CreatePythonObject(item), part);
}

//
// additem(type, level, color)
//
PyObject* CPythonRegionCreature::AddItem(region_object_python* self, PyObject* args, PyObject* kw)
{
	//
	// get param
	//
	long typeId																= 0;
	long level																= 0;
	EItemColor color														= ItemColor_White;
	EItemBind bind															= IB_NotBind;
	long colorAttribID1														= 0;
	long colorRate1															= 0;
	long colorAttribID2														= 0;
	long colorRate2															= 0;
	long timeLimit															= 0;
	long count																= 1;
	static char const* keywords[]													= {"typeId", "count", "level", "color", "bind", "colorId1", "colorRate1", "colorId2", "colorRate2", "timeLimit", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "l|lllllllll:AddItem", (char**)keywords, &typeId, &count, &level, &color, &bind, &colorAttribID1, &colorRate1, &colorAttribID2, &colorRate2, &timeLimit))
		return PyErr_Format(PyExc_TypeError, "[AddItem] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	
	//
	// create item
	//
	std::vector<SItemID> allItems;
	for(long i = 0; i < count; i ++)
	{
		SItemID item														= g_region->m_pItemManager->CreateItem(typeId, color,level, bind, colorAttribID1, colorRate1, colorAttribID2, colorRate2, timeLimit);
		SItemID xx = item;
		if(!ItemID_IsValid(item))
		{
			PyErr_Format(PyExc_TypeError, "[AddItem] invalid item %d, %d, %d", typeId, color, level);
			break;
		}

		//
		// add to creature
		//
		allItems.push_back(item);
		if(!selfCreature->AddItem(item))
		{
			PyErr_Format(PyExc_TypeError, "[AddItem] unable to add item %d, %d, %d", typeId, color, level);
			break;
		}
	}

	//
	// finished
	//
	if(allItems.size() == count)
	{
		PyObject* ret														= PyTuple_New(count);
		for(long i = 0; i < count; i ++)
			PyTuple_SetItem(ret, i, CScriptEngine::CreatePythonObject(allItems[i]));

		return ret;
	}
	
	for(size_t j = 0; j < allItems.size(); j ++)
		g_region->m_pItemManager->DeleteItem(allItems[j]);


	return 0;
}

//
// use item by id
//
PyObject* CPythonRegionCreature::UseItemById(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->UseItem(PyInt_AsLong(args)));
}

//
// use item by type
//
PyObject* CPythonRegionCreature::UseItemByType(region_object_python* self, PyObject* args, PyObject* kw)
{
	long typeId																= 0;
	EItemColor color														= ItemColor_White;
	static char const* keywords[]											= {"typeId", "color", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "l|l:AddItem", (char**)keywords, &typeId, &color))
		return PyErr_Format(PyExc_TypeError, "[UseItemByType] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->UseItem(typeId, color));
}

//
// get free space
//
PyObject* CPythonRegionCreature::BagFreeSpace(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pItemContainer)
		return Py_BuildValue("i", 0);

	if(PyInt_AsLong(args))
		return Py_BuildValue("i", selfCreature->m_pItemContainer->m_Bag.FreeSpace());

	return Py_BuildValue("i", selfCreature->m_pItemContainer->m_MissionBag.FreeSpace());
}

//
// get bag items
//
PyObject* CPythonRegionCreature::BagItems(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pItemContainer)
		return CScriptEngine::ReturnNone();

	CBag* theBag = PyInt_AsLong(args) ? &selfCreature->m_pItemContainer->m_Bag : &selfCreature->m_pItemContainer->m_MissionBag;
	std::vector<SItemID> allItems;
	theBag->ToItemsArray(allItems);

	PyObject* ret = PyTuple_New(static_cast<int>(allItems.size()));
	for(size_t i = 0; i < allItems.size(); i ++)
		PyTuple_SetItem(ret, static_cast<long>(i), CScriptEngine::CreatePythonObject(allItems[i]));

	return ret;
}

//
// get equipment
//
PyObject* CPythonRegionCreature::Equipment(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pItemContainer)
		return CScriptEngine::CreatePythonObject(SItemID());

	return CScriptEngine::CreatePythonObject(selfCreature->m_pItemContainer->GetEquipItem((CItemContainerBase::EEquipParts)PyInt_AsLong(args), true));
}

//
// area check
//
PyObject* CPythonRegionCreature::AreaCheck(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long x0																		= 0;
	long y0																		= 0;
	long x1																		= 0;
	long y1																		= 0;
	if(!PyArg_ParseTuple(args, "llll", &x0, &y0, &x1, &y1))
		return PyErr_Format(PyExc_TypeError, "[AreaCheck] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);

	//
	// get pos
	//
	int x																	= 0;
	int y																	= 0;
	selfCreature->m_scene->m_pTerrain->GetTerrainTile(selfCreature->m_pos[0], selfCreature->m_pos[1], x, y);

	if((x >= x0 && x <= x1 && y >= y0 && y <= y1) || (x >= x1 && x <= x0 && y >= y1 && y <= y0))
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}
PyObject* CPythonRegionCreature::AreaCheckByName(region_object_python* self, PyObject* args)
{
	char *sName = PyString_AsString(args);
	if(!sName || sName[0] == 0)
		return Py_BuildValue("i", 0);

	_CHECK_CREATURE_TYPE(0);
	bool bAtArea = true;
	if(!selfCreature->AtAreaByName(sName))
		bAtArea = false;
	return Py_BuildValue("i", bAtArea ? 1 : 0);
}

//
// distance
//
PyObject* CPythonRegionCreature::Distance(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	region_object_python* theOtherOne										= 0;
	if(!PyArg_ParseTuple(args, "O", &theOtherOne) || !PyObject_TypeCheck(theOtherOne, &m_pythonType))
		return PyErr_Format(PyExc_TypeError, "[Distance] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	REGION_OBJECT_PYTHON_CHECK_TYPE(theOtherOne, CRegionCreature*, otherCreature, 0);

	//
	// get pos
	//
	int x0																	= 0;
	int y0																	= 0;
	selfCreature->m_scene->m_pTerrain->GetTerrainTile(selfCreature->m_pos[0], selfCreature->m_pos[1], x0, y0);
	int x1																	= 0;
	int y1																	= 0;
	otherCreature->m_scene->m_pTerrain->GetTerrainTile(otherCreature->m_pos[0], otherCreature->m_pos[1], x1, y1);

	return Py_BuildValue("(iiif)", abs(x0 - x1), abs(y0 - y1), max(abs(x0 - x1), abs(y0 - y1)), sqrt(static_cast<float>((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1))));
}

//
// connections count
//
PyObject* CPythonRegionCreature::Connections(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pFriend)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", selfCreature->m_pFriend->Size(PyInt_AsLong(args)));
}

//
// team connection
//
PyObject* CPythonRegionCreature::TeamConnection(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pTeam || selfCreature->m_pTeam->mData.Size() != 2 || !selfCreature->m_pFriend)
		return Py_BuildValue("i", 0);

	long type																= PyInt_AsLong(args);
	DWORD otherId															= selfCreature->m_pTeam->mData[0].mDBID == selfCreature->GetObjectId() ? selfCreature->m_pTeam->mData[1].mDBID : selfCreature->m_pTeam->mData[0].mDBID;
	
	return Py_BuildValue("i", selfCreature->m_pFriend->Find(PyInt_AsLong(args), otherId));
}

PyObject* CPythonRegionCreature::EnterLineReq(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long GroupID															= 0;
	long pos[3];
	pos[0]																	= 0;
	pos[1]																	= 0;
	pos[2]																	= 0;

	if(!PyArg_ParseTuple(args, "lll", &GroupID, &pos[0], &pos[1]))
		return PyErr_Format(PyExc_TypeError, "[EnterLineReq] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	

	CLineSysCmdSend_R2G::Send_r2g_linesys_EnterLineReq(GroupID, selfCreature->m_userInfo->m_userId, pos);

	return CScriptEngine::ReturnNone();
}


PyObject* CPythonRegionCreature::LeaveLineReq(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	
	char* pstrmap															= NULL;
	long pos[3];
	pos[0]																	= 0;
	pos[1]																	= 0;
	pos[2]																	= 0;

	if(!PyArg_ParseTuple(args, "sll", &pstrmap, &pos[0], &pos[1]))
		return PyErr_Format(PyExc_TypeError, "[EnterLineReq] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);


	CLineSysCmdSend_R2G::Send_r2g_linesys_LeaveLineReq(selfCreature->m_dwGroupID, selfCreature->m_dwLineID, selfCreature->m_userInfo->m_userId, pstrmap, pos);

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::Enter_fb(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	
	long fbId			= 0;
	long dif			= 0;
	long ulv			= 0;
	long lExitPos[2]	= {0};
	long lTeamEnter = 0;

	if(!PyArg_ParseTuple(args, "llllll", &fbId, &dif, &ulv, &lExitPos[0], &lExitPos[1], &lTeamEnter))
		return PyErr_Format(PyExc_TypeError, "[Enter_fb] invalid params");

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->m_dwGroupID != 0 || selfCreature->m_dwLineID != 0)
		return PyErr_Format(PyExc_TypeError, "[Enter_fb] online can`t enter dungeon");

	if(ulv <= 0)
	{
		if(selfCreature->m_pTeam)
		{
			if(selfCreature->m_pTeam->mData.Size() > 0)
			{
				for(int ii=selfCreature->m_pTeam->mData.Begin();
					ii != selfCreature->m_pTeam->mData.End();
					ii++)
					ulv += (int)selfCreature->m_pTeam->mData[ii].mLevel;
				ulv /= selfCreature->m_pTeam->mData.Size();
			}
			else ulv = selfCreature->m_core.Lev;
		}
		else ulv = selfCreature->m_core.Lev;
	}

	if(lTeamEnter)
	{
		int iMemberCount = selfCreature->m_pTeam->mData.Size();
		for(int ii=0; ii<iMemberCount; ii++)
		{
			CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
			if(pUser)
			{
				const char* szMsg = g_region->m_fbMgr->RequestEnter(pUser, fbId, dif, ulv,
					lExitPos[0] == 0 ? pUser->m_dummy->m_pos[0] : lExitPos[0],
					lExitPos[1] == 0 ? pUser->m_dummy->m_pos[1] : lExitPos[1]);
				if(szMsg)
					pUser->m_dummy->SendSystemMessage(szMsg);
			}
		}
	}
	else
	{
		const char* szMsg = g_region->m_fbMgr->RequestEnter(selfCreature->m_userInfo, fbId, dif, ulv,
			lExitPos[0] == 0 ? selfCreature->m_pos[0] : lExitPos[0],
			lExitPos[1] == 0 ? selfCreature->m_pos[1] : lExitPos[1]);
		if(szMsg)
			selfCreature->SendSystemMessage(szMsg);
	}

	return CScriptEngine::ReturnNone();
}


PyObject* CPythonRegionCreature::Leave_fb(region_object_python* self, PyObject* args)
{

	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);


	//g_region->m_fbMgr->RequestLeave(selfCreature->m_userInfo);

	return Py_BuildValue("i", g_region->m_fbMgr->RequestLeave(selfCreature->m_userInfo));
}

PyObject* CPythonRegionCreature::AddLevel(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	long lAddLvCount = PyInt_AsLong(args);

	if(lAddLvCount > 0)
	{
		selfCreature->m_core.Lev += lAddLvCount;
		selfCreature->Levelup(lAddLvCount);
	}
	else	PyErr_Format(PyExc_TypeError, "[AddLevel] Add Level %d is <= 0", lAddLvCount);


	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddExp(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	long lAddExpCount = PyInt_AsLong(args);

	if(lAddExpCount != 0)
	{
		selfCreature->AddExp(lAddExpCount, 0);
	}
	else	PyErr_Format(PyExc_TypeError, "[AddExp] Add Exp is == 0");


	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddCulExp(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	long lAddExpCount = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(lAddExpCount != 0)
	{
		selfCreature->AddCulExp(lAddExpCount);
	}

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddSoul(region_object_python* self, PyObject* args)
{
	long lAddSoul = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(lAddSoul != 0)
	{
		selfCreature->m_core.AddSoul(lAddSoul);
		selfCreature->AttChangeMessageToClient(lAddSoul > 0 ? "MSG_GET_Soul" : "MSG_LOST_Soul", lAddSoul);
	}

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddCredit(region_object_python* self, PyObject* args)
{
	//
	// get creature
	//
	_CHECK_CREATURE_TYPE(0);
	long lAddCredit = PyInt_AsLong(args);

	if(lAddCredit != 0)
	{
		selfCreature->AddCredit(lAddCredit);
	}

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddContribute(region_object_python* self, PyObject* args)
{
	long lAddContribute = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(lAddContribute != 0)
	{
		selfCreature->m_core.AddContribute(lAddContribute);
		selfCreature->AttChangeMessageToClient(lAddContribute > 0 ? "MSG_ADD_CONTRIBUTE" : "MSG_DEL_CONTRIBUTE", lAddContribute);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddPoint(region_object_python* self, PyObject* args)
{
	long lAddPoint = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(lAddPoint != 0)
	{
		selfCreature->m_core.AddPoint(lAddPoint);
		selfCreature->AttChangeMessageToClient(lAddPoint > 0 ? "MSG_JIFEN_GET" : "MSG_JIFEN_LOSE", lAddPoint);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddRecord(region_object_python* self, PyObject* args)
{
	long lAddRecord = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(lAddRecord != 0)
	{
		selfCreature->m_core.AddRecord(lAddRecord);
		selfCreature->AttChangeMessageToClient(lAddRecord > 0 ? "MSG_ADD_RECORD" : "MSG_DEL_RECORD", lAddRecord);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddUnionRecord(region_object_python* self, PyObject* args)
{
	long lAddUnionRecord = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(selfCreature->IsUser() && lAddUnionRecord != 0)
	{
		//selfCreature->m_core.AddUnionRecord(lAddUnionRecord);
		selfCreature->GetPlayerLogic()->AddUnionRecord(lAddUnionRecord);
		selfCreature->AttChangeMessageToClient(lAddUnionRecord > 0 ? "MSG_ADD_UNIONRECORD" : "MSG_DEL_UNIONRECORD", lAddUnionRecord);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddJobRecord(region_object_python* self, PyObject* args)
{
	long lAddJobRecord = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(selfCreature->IsUser() && lAddJobRecord != 0)
	{
		selfCreature->m_core.AddJobRecord(lAddJobRecord);
		//selfCreature->GetPlayerLogic()->AddJobRecord(lAddJobRecord);
		selfCreature->AttChangeMessageToClient(lAddJobRecord > 0 ? "MSG_ADD_JOBRECORD" : "MSG_DEL_JOBRECORD", lAddJobRecord);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::ShowDesignateDialog(region_object_python* self, PyObject* args, PyObject* kw)
{
	static const char *sStartStr = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\"/></head><body>";
	static const char *sEndStr = "</body></html>";
	static const char *sStartFag = "<p>";
	static const char *sEndFag = "</p>";
	static const char *sFlogStr = "@@";
	//static char sText[1024 * 10] = {0};
	S_MEMDEF(sText, 1024 * 10)
	S_MEMPROTECTOR(sText, 1024 * 10, bMP)

	char *sDialogText = 0;
	long lNPC_ID = 0;
	long lShowTime = 0;
	static char const* keywords[] = {"text", "id", "time", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "s|ll:ShowDesignateDialog", (char**)keywords, &sDialogText, &lNPC_ID, &lShowTime))
		return PyErr_Format(PyExc_TypeError, "[ShowDesignateDialog] invalid params");
	if(!sDialogText || sDialogText[0] == 0)
		return PyErr_Format(PyExc_TypeError, "[ShowDesignateDialog] invalid params");

	rt2_strcpy(sText, sDialogText);
	std::string sCreateString(sStartStr);
	char *pNewSubString = strtok(sText, sFlogStr);
	if(pNewSubString)
	{
		do
		{
			sCreateString += sStartFag;
			sCreateString += pNewSubString;
			sCreateString += sEndFag;
			pNewSubString = strtok(NULL, sFlogStr);
		}while(pNewSubString);
	}
	else
	{
		sCreateString += sStartFag;
		sCreateString += sText;
		sCreateString += sEndFag;
	}
	sCreateString += sEndStr;

	_CHECK_CREATURE_TYPE(0);
	g_region->ShowHtmlDialog(selfCreature->m_scene->FindCreature(lNPC_ID),
							selfCreature, sCreateString.c_str(), 0, 0, 0, 0, 0, lShowTime);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::ShowFunctionDialog(region_object_python* self, PyObject* args)
{
	static const char *sStartStr = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\"/></head><body>";
	static const char *sEndStr = "</body></html>";
	static const char *sStartFag = "<p>";
	static const char *sEndFag = "</p>";
	static const char *sFlogStr = "@@";
	//static char sText[1024 * 10] = {0};
	S_MEMDEF(sText, 1024 * 10)
	S_MEMPROTECTOR(sText, 1024 * 10, bMP)

	//
	// get param
	//
	char *sDialogText = PyString_AsString(args);
	if(!sDialogText || sDialogText[0] == 0)
		return PyErr_Format(PyExc_TypeError, "[ShowFunctionDialog] invalid params");

	rt2_strcpy(sText, sDialogText);
	std::string sCreateString(sStartStr);
	char *pNewSubString = strtok(sText, sFlogStr);
	if(pNewSubString)
	{
		do
		{
			sCreateString += sStartFag;
			sCreateString += pNewSubString;
			sCreateString += sEndFag;
			pNewSubString = strtok(NULL, sFlogStr);
		}while(pNewSubString);
	}
	else
	{
		sCreateString += sStartFag;
		sCreateString += sText;
		sCreateString += sEndFag;
	}
	sCreateString += sEndStr;

	_CHECK_CREATURE_TYPE(0);
	//g_region->ShowHtmlDialog(static_cast<CRegionCreature*>(pNPC->m_pObject),
	//	selfCreature, sCreateString.c_str(), sName,
	//	szValue, NULL, iTaskID);
	g_region->ShowMessageBox(selfCreature, sCreateString.c_str(), "", 2);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::DisposeTaskInterface(region_object_python* self, PyObject* args)
{
	long lTaskID = 0;
	long lNPC_ID = 0;
	if(!PyArg_ParseTuple(args, "ll", &lTaskID, &lNPC_ID))
		return PyErr_Format(PyExc_TypeError, "[DisposeTaskInterface] invalid params");
	if(lTaskID <= 0 || lNPC_ID <= 0)
		return CScriptEngine::ReturnNone();

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return CScriptEngine::ReturnNone();

	CRegionCreature *pObject = selfCreature->m_scene->FindCreature(lNPC_ID);
	if(!pObject)
		return CScriptEngine::ReturnNone();
	CRegionNpcTask *pNPC = dynamic_cast<CRegionNpcTask*>(pObject);
	if(!pNPC)
		return CScriptEngine::ReturnNone();
	char pTaskInfo[16] = {0};
	rt2_snprintf(pTaskInfo, 16, "_TA_%d", lTaskID);
	pNPC->DisposeTaskInterface(selfCreature, pTaskInfo);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::RegionMessageBox(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	char *sDialogMsg = 0;
	char *sComText = 0;
	long iButton = 0;
	if(!PyArg_ParseTuple(args, "ssl", &sDialogMsg, &sComText, &iButton))
		return PyErr_Format(PyExc_TypeError, "[RegionMessageBox] invalid params");

	_CHECK_CREATURE_TYPE(0);
	g_region->ShowMessageBox(selfCreature, sDialogMsg, sComText, iButton);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::CanAcceptTask(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	_CHECK_CREATURE_TYPE(0);
	long iTaskID = PyInt_AsLong(args);

	long lFinishRet = 0;
	if(iTaskID > 0 && selfCreature->IsUser())
	{
		lFinishRet = selfCreature->CanAcceptTask(iTaskID, 0) ? 1 : 0;
	}

	if(lFinishRet == 0)
		PyErr_Format(PyExc_TypeError, "[AcceptTask] Can`t accept task %d", iTaskID);
	return Py_BuildValue("i", lFinishRet);
}

PyObject* CPythonRegionCreature::AcceptTask(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	_CHECK_CREATURE_TYPE(0);
	long iTaskID = PyInt_AsLong(args);

	long lFinishRet = 0;
	if(iTaskID > 0 && selfCreature->IsUser())
	{
		lFinishRet = selfCreature->AcceptTask(iTaskID, 0) ? 1 : 0;
	}

	if(lFinishRet == 0)
		PyErr_Format(PyExc_TypeError, "[AcceptTask] Can`t accept task %d", iTaskID);
	return Py_BuildValue("i", lFinishRet);
}

PyObject* CPythonRegionCreature::FinishTask(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long iTaskID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	//if(!iTaskID = PyInt_AsLong(args))
	//	return PyErr_Format(PyExc_TypeError, "[FinishTask] invalid params");

	long lFinishRet = 0;
	if(iTaskID > 0 && selfCreature->IsUser())
	{
		STaskInfo *pTInfo = selfCreature->m_task.FindTask(iTaskID);
		if(pTInfo && pTInfo->Desc && selfCreature->IsDoingTask(pTInfo->Desc, 0))
		{
			if(pTInfo->Desc->finishType == TASK_FINISH_TYPE_NORMAL)
			{
				lFinishRet = selfCreature->FinishTask(pTInfo->Desc, 0, 0) ? 1 : 0;
			}
		}
	}

	if(lFinishRet == 0)
		PyErr_Format(PyExc_TypeError, "[FinishTask] Can`t finish task %d", iTaskID);
	return Py_BuildValue("i", lFinishRet);
}

PyObject* CPythonRegionCreature::SetToCanFinishTaskByID(region_object_python* self, PyObject* args)
{
	long iTaskID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);

	if(iTaskID > 0 && selfCreature->IsUser())
	{
		STaskInfo *pTInfo = selfCreature->m_task.FindTask(iTaskID);
		if(pTInfo && pTInfo->Desc && selfCreature->IsDoingTask(pTInfo->Desc, 0))
		{
			if(pTInfo->Desc->finishType == TASK_FINISH_TYPE_NORMAL)
			{
				pTInfo->FSetCanFinish();
				selfCreature->UpdateToClient(NET_CRE_UPDATE_TASK);
			}
		}
	}

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::GetTaskStateByID(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long iTaskID = PyInt_AsLong(args);

	_CHECK_CREATURE_TYPE(0);
	long lTaskState = 0;
	if(selfCreature->m_task.IsDoneTask(iTaskID))
		lTaskState = 2;
	else if(selfCreature->m_task.FindTask(iTaskID))
		lTaskState = 1;
	else
		lTaskState = 0;
	//if(iTaskID > 0 && selfCreature->IsUser())
	//{
	//	STaskInfo *pTInfo = selfCreature->m_task.FindTask(iTaskID);
	//	if(!pTInfo && g_TableTask.FindTask(iTaskID))
	//	{
	//		selfCreature->m_task.AddTask(iTaskID);
	//		pTInfo = selfCreature->m_task.FindTask(iTaskID);
	//	}
	//	if(pTInfo)
	//	{
	//		if(pTInfo->FIsCanecl())
	//			lTaskState = 0;	//未接受此任务
	//		else if(pTInfo->FIsDoing())
	//			lTaskState = 1;	//此任务正在进行中
	//		else if(pTInfo->FIsDone())
	//			lTaskState = 2;	//此任务已经完成并提交
	//		else lTaskState = 0;
	//	}
	//}

	return Py_BuildValue("i", lTaskState);
}

PyObject* CPythonRegionCreature::CancelTask(region_object_python* self, PyObject* args)
{
	long lTaskID = PyInt_AsLong(args);
	if(lTaskID <= 0)
		return CScriptEngine::ReturnNone();

	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->IsUser())
	{
		STaskInfo *pTInfo = selfCreature->m_task.FindTask(lTaskID);
		if(pTInfo)
			selfCreature->CancelTask(lTaskID, false);
	}

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SetTaskStateByID(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long lTaskID = 0;
	long lState = 0;
	if(!PyArg_ParseTuple(args, "ll", &lTaskID, &lState))
		return PyErr_Format(PyExc_TypeError, "[SetTaskStateByID] invalid params");
	if(lTaskID <= 0 || lState < 0 || lState > 2)
		return CScriptEngine::ReturnNone();

	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->IsUser())
	{
		STaskInfo *pTInfo = selfCreature->m_task.FindTask(lTaskID);
		if(!pTInfo && g_TableTask.FindTask(lTaskID))
		{
			selfCreature->m_task.AddTask(lTaskID);
			pTInfo = selfCreature->m_task.FindTask(lTaskID);
		}
		if(pTInfo)
		{
			switch(lState)
			{
			case 0:
				//if(pTInfo->FIsDone())
				if(selfCreature->m_task.IsDoneTask(lTaskID))
					pTInfo->FSetCanecl();
				if(pTInfo->FIsDoing())
					selfCreature->CancelTask(lTaskID, false);
				break;
			case 1:
				if(pTInfo->FIsCanecl())
					selfCreature->AcceptTask(lTaskID, 0);
				//if(pTInfo->FIsDone())
				if(selfCreature->m_task.IsDoneTask(lTaskID))
				{
					pTInfo->FSetCanecl();
					selfCreature->AcceptTask(lTaskID, 0);
				}
				break;
			case 2:
				//FSetDone和FSetDone已经无效
				//if(pTInfo->FIsCanecl())
				//	pTInfo->FSetDone();
				//if(pTInfo->FIsDoing())
				//{
				//	selfCreature->CancelTask(lTaskID, false);
				//	pTInfo->FSetDone();
				//}
				break;
			}
		}
	}

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::GetTeamAllMember(region_object_python* self, PyObject* args)
{
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pTeam || selfCreature->m_pTeam->mData.Empty())
		return Py_BuildValue("(i)", 0);
	int iMemberCount = selfCreature->m_pTeam->mData.Size();
	if(iMemberCount <= 0)
		return Py_BuildValue("(i)", 0);

	CScriptEngine* theEngine = CScriptEngine::GetSingleton();
	PyObject* pRet = theEngine->BuildCallArgs(iMemberCount);
	for(int ii=0; ii<iMemberCount; ii++)
	{
		CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
		if(pUser && pUser->m_dummy)
			theEngine->BuildArg(pRet, ii, pUser->m_dummy);
		else
		{
			ERR1("Script Function : GetTeamAllMember, Find team user error. return 0");
			return Py_BuildValue("(i)", 0);
		}
	}

	return pRet;
}

PyObject* CPythonRegionCreature::TeamMoveTo(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long lX = 0;
	long lY = 0;
	if(!PyArg_ParseTuple(args, "ll", &lX, &lY))
		return PyErr_Format(PyExc_TypeError, "[TeamMoveTo] invalid params");

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pTeam || selfCreature->m_pTeam->mData.Empty())
		return CScriptEngine::ReturnNone();
	int iMemberCount = selfCreature->m_pTeam->mData.Size();
	if(iMemberCount < 0)
		iMemberCount = 0;

	//
	// get creature
	//
	for(int ii=0; ii<iMemberCount; ii++)
	{
		if(selfCreature->m_pTeam->mData[ii].mDBID == selfCreature->m_userInfo->m_userId)
			continue;
		CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
		if(!pUser)
		{
			ERR1("Script Function : TeamMoveTo, Find team index user %d error. return, since User no move.", ii);
			return CScriptEngine::ReturnNone();
		}
		g_region->m_gws.MoveUserToPosition(pUser->m_userId, 0, lX, lY);
	}
	g_region->m_gws.MoveUserToPosition(selfCreature->m_userInfo->m_userId, 0, lX, lY);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::TeamAtArea(region_object_python* self, PyObject* args)
{
	long lMinX = 0, lMinY = 0;
	long lMaxX = 0, lMaxY = 0;
	if(!PyArg_ParseTuple(args, "llll", &lMinX, &lMinY, &lMaxX, &lMaxY))
		return PyErr_Format(PyExc_TypeError, "[TeamAtArea] invalid params");

	//以下进行参数转换lMin始终小于lMax，方便后面判断区域
	if(lMinX == lMaxX || lMinY == lMaxY)
		return Py_BuildValue("i", 0);
	if(lMinX < lMaxX)
	{
		if(lMinY > lMaxY)
		{
			long lTempY = lMaxY;
			lMaxY = lMinY;
			lMinY = lTempY;
		}
	}
	else
	{
		long lTempX = lMaxX;
		lMaxX = lMinX;
		lMinX = lTempX;

		if(lMinY > lMaxY)
		{
			long lTempY = lMaxY;
			lMaxY = lMinY;
			lMinY = lTempY;
		}
	}

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pTeam || selfCreature->m_pTeam->mData.Empty())
		return Py_BuildValue("i", 0);
	int iMemberCount = selfCreature->m_pTeam->mData.Size();
	if(iMemberCount <= 0)
		return Py_BuildValue("i", 0);

	bool bAtArea = true;
	for(int ii=0; ii<iMemberCount; ii++)
	{
		CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
		if(!pUser || !pUser->m_dummy)
		{
			ERR1("Script Function : TeamAtArea, Find team user %d error. return", ii);
			return Py_BuildValue("i", 0);
		}
		int lX = 0, lY = 0;
		if(selfCreature->m_scene->m_pTerrain->GetTerrainTile(pUser->m_dummy->m_pos[0], pUser->m_dummy->m_pos[1], lX, lY))
		{
			if( (lX > lMinX) && (lX < lMaxX) && (lY > lMinY) && (lY < lMaxY) )
				continue;
			else
			{
				bAtArea = false;
				break;
			}
		}
		else
		{
			bAtArea = false;
			break;
		}
	}

	return Py_BuildValue("i", bAtArea ? 1 : 0);
}

PyObject* CPythonRegionCreature::TeamAtAreaByName(region_object_python* self, PyObject* args)
{
	char *sName = PyString_AsString(args);
	if(!sName || sName[0] == 0)
		return Py_BuildValue("i", 0);

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->m_pTeam || selfCreature->m_pTeam->mData.Empty())
		return Py_BuildValue("i", 0);
	int iMemberCount = selfCreature->m_pTeam->mData.Size();
	if(iMemberCount <= 0)
		return Py_BuildValue("i", 0);

	bool bAtArea = true;
	for(int ii=0; ii<iMemberCount; ii++)
	{
		CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
		if(!pUser || !pUser->m_dummy)
		{
			ERR1("Script Function : TeamAtArea, Find team user %d error. return", ii);
			return Py_BuildValue("i", 0);
		}
		//if(!selfCreature->m_scene->m_pTerrain->PointInArea(pUser->m_dummy->m_pos[0], pUser->m_dummy->m_pos[1], sName))
		if(!pUser->m_dummy->AtAreaByName(sName))
		{
			bAtArea = false;
			break;
		}
	}

	return Py_BuildValue("i", bAtArea ? 1 : 0);
}

PyObject* CPythonRegionCreature::GetCurrentSelect(region_object_python* self, PyObject* args)
{
	_CHECK_CREATURE_TYPE(0);
	CRegionCreature *pUser = 0;//selfCreature->m_sel;
	return Py_BuildValue("O", pUser ? CScriptEngine::CreatePythonObject(pUser) : 0);
}

PyObject* CPythonRegionCreature::GetAllSkillID(region_object_python* self, PyObject* args)
{
	_CHECK_CREATURE_TYPE(0);
	std::vector<int> vSkillID;
	int iSkillCount = selfCreature->m_Skill.GetAllActiveSkillID(vSkillID);
	if(iSkillCount <= 0)
		return Py_BuildValue("(i)", 0);

	CScriptEngine* theEngine = CScriptEngine::GetSingleton();
	PyObject* pRet = theEngine->BuildCallArgs(iSkillCount+1);
	theEngine->BuildArg(pRet, 0, iSkillCount);
	for(int ii=0; ii<iSkillCount; ii++)
		theEngine->BuildArg(pRet, ii+1, vSkillID[ii]);

	return pRet;
}

PyObject* CPythonRegionCreature::HaveSkillID(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long lSkillID = PyInt_AsLong(args);

	_CHECK_CREATURE_TYPE(0);
	long lHaveSkillID = selfCreature->m_Skill.FindAllSkill(lSkillID) ? 1 : 0;
	return Py_BuildValue("i", lHaveSkillID);
}

PyObject* CPythonRegionCreature::HaveSkillEffect(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long wID = 0, lType = 0;
	if(!PyArg_ParseTuple(args, "ll", &wID, &lType))
		return PyErr_Format(PyExc_TypeError, "[HaveSkillEffect] invalid params");

	_CHECK_CREATURE_TYPE(0);
	bool bHave = false;
	if(lType == 0)
		bHave = selfCreature->HaveEffectByID(wID);
	else
		bHave = selfCreature->HaveEffectByClass(wID);
	return Py_BuildValue("i", bHave ? 1 : 0);
}

PyObject* CPythonRegionCreature::UseSkill(region_object_python* self, PyObject* args)
{
	long lSkillID = 0;
	region_object_python *pTarget = 0;
	long lPosX = 0, lPosY = 0;
	float fPosX = 0.0f, fPosY = 0.0f;
	if(!PyArg_ParseTuple(args, "lO", &lSkillID, &pTarget))
		return PyErr_Format(PyExc_TypeError, "[UseSkill] invalid params");
	if(lSkillID <= 0 || !pTarget)
		return Py_BuildValue("i", 0);

	_CHECK_CREATURE_TYPE(0);
	CRegionCreature *pSelTar = selfCreature->GetCurrentTarget();

	SAttackParam skillParam;
	skillParam.Seed = rand() % 60000;
	skillParam.Skill = lSkillID;
	skillParam.TargetType = 0;

	if(pSelTar && selfCreature->m_scene != pSelTar->m_scene)
		return Py_BuildValue("i", 0);
	CRegionCreature *pTarObject = dynamic_cast<CRegionCreature*>(pTarget->m_pObject);
	if(pTarObject && selfCreature->m_scene != pTarObject->m_scene)
		return Py_BuildValue("i", 0);

	SSkill* pSkill = Skill()->FindSkill(skillParam.Skill);
	if(!pSkill)
		return Py_BuildValue("i", 0);
	if(pSkill->dwRTarget == 0)
	{
		skillParam.TargetType = SKILL_TARGET_TYPE_NONE;
		skillParam.EnemyId = selfCreature->GetObjectId();
		skillParam.Pos[0] = selfCreature->m_pos[0];
		skillParam.Pos[1] = selfCreature->m_pos[1];
		skillParam.Pos[2] = selfCreature->m_pos[2];
	}
	else if(pSkill->dwRTarget&SKILL_TARGET_POSITION)
	{
		skillParam.TargetType = SKILL_TARGET_TYPE_POSITION;
		skillParam.EnemyId = 0;
		selfCreature->m_scene->m_pTerrain->GetTerrainPos(lPosX, lPosY, fPosX, fPosY);
		skillParam.Pos[0] = fPosX;
		skillParam.Pos[1] = fPosY;
		skillParam.Pos[3] = selfCreature->m_pos[2];
	}
	else if(pSkill->bABad)
	{
		skillParam.TargetType = SKILL_TARGET_TYPE_ACTOR;
		skillParam.EnemyId = pTarget->m_pObject ?
								pTarget->m_pObject->m_oId :
								pSelTar ? pSelTar->GetObjectId() : 0;
		if(skillParam.EnemyId == 0)
			return Py_BuildValue("i", 0);
		if(pTarget->m_pObject)
		{
			CRegionCreature *pObject = dynamic_cast<CRegionCreature*>(pTarget->m_pObject);
			if(!pObject)
				return Py_BuildValue("i", 0);
			skillParam.Pos[0] = pObject->m_pos[0];
			skillParam.Pos[1] = pObject->m_pos[1];
			skillParam.Pos[2] = pObject->m_pos[2];
		}
	}
	else
	{
		skillParam.TargetType = pTarget->m_pObject ? SKILL_TARGET_TYPE_ACTOR : SKILL_TARGET_TYPE_NONE;
		skillParam.EnemyId = pTarget->m_pObject ? pTarget->m_pObject->m_oId : selfCreature->GetObjectId();
		skillParam.Pos[0] = selfCreature->m_pos[0];
		skillParam.Pos[1] = selfCreature->m_pos[1];
		skillParam.Pos[2] = selfCreature->m_pos[2];
	}

	return Py_BuildValue("i", selfCreature->UseSkill(skillParam) ? 1 : 0);
}

PyObject* CPythonRegionCreature::AddEffect(region_object_python* self, PyObject* args)
{
	//
	// get param
	//
	long lEffectID = PyInt_AsLong(args);

	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_Skill.AddEffect(lEffectID, 0, true) ? 1 : 0);
}

PyObject* CPythonRegionCreature::IsBattle(region_object_python* self, PyObject* args)
{
	_CHECK_CREATURE_TYPE(0);
	return Py_BuildValue("i", selfCreature->m_lIsCombat ? 1 : 0);
}

PyObject* CPythonRegionCreature::DebaseEquipWear(region_object_python* self, PyObject* args)
{
	long lWear = 0;//PyInt_AsLong(args);
	CItemContainerBase::EEquipParts eEquipPos = CItemContainerBase::WEAPON_1;
	if(!PyArg_ParseTuple(args, "ll", &lWear, &eEquipPos))
		return PyErr_Format(PyExc_TypeError, "[DebaseEquipWear] invalid params");

	if(lWear <= 0 || eEquipPos < -1 || eEquipPos >= CItemContainerBase::MAX_EQUIP_PARTS)
		return PyErr_Format(PyExc_TypeError, "[DebaseEquipWear] invalid params");
	_CHECK_CREATURE_TYPE(0);
	if(eEquipPos >= CItemContainerBase::WEAPON_1)
	{
		if(selfCreature->m_pItemContainer->LoseEquip(eEquipPos, lWear))
			selfCreature->RefreshEquipWear();
	}
	else
	{
		bool bRefresh = false;
		for(unsigned int ePos = CItemContainerBase::WEAPON_1;
			ePos < CItemContainerBase::MAX_EQUIP_PARTS;
			++ePos)
		{
			if(selfCreature->m_pItemContainer->LoseEquip((CItemContainerBase::EEquipParts)ePos, lWear))
				bRefresh = true;
		}
		if(bRefresh)
			selfCreature->RefreshEquipWear();
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SetGuard(region_object_python* self, PyObject* args)
{
	long lUserID = PyInt_AsLong(args);

	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->m_ai && selfCreature->m_ai->GetType() == CRegionAI::TYPE_TOUR)
	{
		CRegionCreature *pUser = selfCreature->m_scene->FindCreature(lUserID);
		if(pUser)
			selfCreature->m_ai->SetGuard(pUser);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::CancelGuard(region_object_python* self, PyObject* args)
{
	long lUserID = PyInt_AsLong(args);

	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->m_ai && selfCreature->m_ai->GetType() == CRegionAI::TYPE_TOUR)
	{
		CRegionCreature *pUser = selfCreature->m_scene->FindCreature(lUserID);
		if(pUser)
			selfCreature->m_ai->CancelGuard(lUserID);
	}
	else if(selfCreature->IsUser() && selfCreature->m_iGuardID > 0)
	{
		CRegionCreature *pNPC = selfCreature->m_scene->FindCreature(selfCreature->m_iGuardID);
		if(pNPC && pNPC->m_ai && pNPC->m_ai->GetType() == CRegionAI::TYPE_TOUR)
			pNPC->m_ai->CancelGuard(selfCreature->GetObjectId());
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::Remove(region_object_python* self, PyObject* args)
{
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser() && !selfCreature->IsUserPet() && selfCreature->m_npcInfo)
		selfCreature->InvokeEventCommand("delself");
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::Die(region_object_python* self, PyObject* args)
{
	long lKillerID = PyInt_AsLong(args);
	if(lKillerID <= 0)
		return CScriptEngine::ReturnNone();

	_CHECK_CREATURE_TYPE(0);
	CRegionCreature *pKiller = selfCreature->m_scene->FindCreature(lKillerID);
	if(!pKiller)
		return CScriptEngine::ReturnNone();
	if( selfCreature->m_type != OBJECT_TYPE_CREATURE &&
		selfCreature->m_type != OBJECT_TYPE_NPC_SERVICE &&
		selfCreature->m_type != OBJECT_TYPE_NPC_COMBATIVE )
		return CScriptEngine::ReturnNone();
	if(!selfCreature->IsUser() && !selfCreature->IsUserPet() && selfCreature->m_npcInfo)
	{
		DWORD dwParam[4] = {-1};
		dwParam[0] = lKillerID;
		selfCreature->SetTimer(CRE_TIMER_DIE_SELF, 1000, dwParam);
		//selfCreature->MeDie(pKiller);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::DeleteEffect(region_object_python* self, PyObject* args)
{
	long lEffectID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	if(lEffectID > 0 && Skill()->FindEffect(lEffectID) &&
		(selfCreature->IsUser() || selfCreature->IsUserPet() || selfCreature->IsMonster()))
	{
		selfCreature->m_Skill.DeleteEffect(lEffectID, true);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SwitchModel(region_object_python* self, PyObject* args)
{
	long lModelID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	if( selfCreature->m_type != OBJECT_TYPE_CREATURE &&
		selfCreature->m_type != OBJECT_TYPE_NPC_SERVICE &&
		selfCreature->m_type != OBJECT_TYPE_NPC_COMBATIVE )
		return CScriptEngine::ReturnNone();
	if(!selfCreature->IsUser() && !selfCreature->IsUserPet() && lModelID > 0 &&
		g_TableCreModel.FindCreModel(lModelID) != NULL)
		selfCreature->SetModel(lModelID);

	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::AddLifeSkill(region_object_python* self, PyObject* args)
{
	long lSkillID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	if(lSkillID <= 0 || !selfCreature->IsUser())
		return Py_BuildValue("i", 0);

	bool bSuss = selfCreature->m_Skill.AddSkill(lSkillID, 0, true);
	return Py_BuildValue("i", bSuss ? 1 : 0);
}
PyObject* CPythonRegionCreature::DelLifeSkill(region_object_python* self, PyObject* args)
{
	long lSkillID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	if(lSkillID <= 0 || !selfCreature->IsUser())
		return Py_BuildValue("i", 0);
	selfCreature->DeleteLifeSkill(lSkillID);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SetNPCColorForUser(region_object_python* self, PyObject* args)
{
	long lColor[3] = {-1};
	if(!PyArg_ParseTuple(args, "lll", &lColor[0], &lColor[1], &lColor[2]))
		return PyErr_Format(PyExc_TypeError, "[SetNPCColorForUser] invalid params");

	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->IsUser() || selfCreature->IsUserPet())
		return CScriptEngine::ReturnNone();

	for(int ii=0; ii<3; ii++)
	{
		if(lColor[ii] > 255)
			lColor[ii] = 255;
		if(lColor[ii] < 0)
			lColor[ii] = 0;
	}
	DWORD dwColor = 0xff000000;
	dwColor |= lColor[0] << 16;
	dwColor |= lColor[1] << 8;
	dwColor |= lColor[2];

	selfCreature->SetColorToNPCName(dwColor);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SetNPCAnimation(region_object_python* self, PyObject* args)
{
	char *sAnimName = 0;
	long lDurTime = 1000;
	if(!PyArg_ParseTuple(args, "sl", &sAnimName, &lDurTime))
		return PyErr_Format(PyExc_TypeError, "[SetNPCAnimation] invalid params");
	if(!sAnimName)
		return PyErr_Format(PyExc_TypeError, "[SetNPCAnimation] invalid params");

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser() && selfCreature->m_ai)
		selfCreature->m_ai->SetToAnimationState(&std::string(sAnimName), (short)lDurTime);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SetWarder(region_object_python* self, PyObject* args)
{
	long lPlayerID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->IsUser() || selfCreature->IsUserPet())
		return CScriptEngine::ReturnNone();
	CRegionCreature *pUser = selfCreature->m_scene->FindCreature(lPlayerID);
	if(pUser && pUser->IsUser())
		selfCreature->SetWarder(pUser);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::RemoveWardNPC(region_object_python* self, PyObject* args)
{
	_CHECK_CREATURE_TYPE(0);
	selfCreature->RemoveWardNPC();
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SendProblem(region_object_python* self, PyObject* args)
{
	long lProblemID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	selfCreature->SendProblemToClient(lProblemID);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SendMail(region_object_python* self, PyObject* args, PyObject* kw)
{
	char *sTitle = 0;
	char *sText = 0;
	long lItemID = 0;
	long lColor = ItemColor_White;
	long lLevel = 0;
	long lMoney = 0;
	static char const* keywords[] = {"title", "text", "money", "id", "color", "lv", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "ss|llll:SendMail", (char**)keywords, &sTitle, &sText, &lMoney, &lItemID, &lColor, &lLevel))
		return PyErr_Format(PyExc_TypeError, "[SendMail] invalid params");
	if(lItemID < 0 || lColor < ItemColor_White || lColor > ItemColor_All || lLevel < 0 || lMoney < 0)
		return PyErr_Format(PyExc_TypeError, "[SendMail] invalid params");

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return CScriptEngine::ReturnNone();
	if(lItemID > 0)
	{
		CItemManager *pItemManager = selfCreature->m_pItemContainer->GetItemManager();
		SItemID Item = g_region->m_pItemManager->CreateItem(lItemID, (EItemColor)lColor, lLevel, IB_NotBind);
		std::string strItem = Item.SaveToString(pItemManager);
		g_region->m_gws.SendMail(selfCreature->m_userInfo->m_userId, 0, "【系统】", sTitle, sText, strItem, lMoney);
	}
	else
		g_region->m_gws.SendMail(selfCreature->m_userInfo->m_userId, 0, "【系统】", sTitle, sText, "", lMoney);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::CanJoinActivity(region_object_python* self, PyObject* args)
{
	long lActID = 0;
	long lTeam = 0;
	if(!PyArg_ParseTuple(args, "ll", &lActID, &lTeam))
		return PyErr_Format(PyExc_TypeError, "[CanJoinActivity] invalid params");
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return CScriptEngine::ReturnNone();

	if(lTeam)
	{
		int iMemberCount = selfCreature->m_pTeam->mData.Size();
		if(iMemberCount > 0)
		{
			for(int ii=0; ii<iMemberCount; ii++)
			{
				CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
				if(pUser)
					CRegionActManager::GetSingle()->CanExecute(lActID, pUser);
			}
		}
		else
			CRegionActManager::GetSingle()->CanExecute(lActID, selfCreature->m_userInfo);
	}
	else
		CRegionActManager::GetSingle()->CanExecute(lActID, selfCreature->m_userInfo);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::JoinActivity(region_object_python* self, PyObject* args, PyObject* kw)
{
	long lActID = -1;//PyInt_AsLong(args);
	PyObject *pParam = 0;
	long lTeam = 0;
	static char const* keywords[] = {"id", "param", "team", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "l|Ol:JoinActivity", (char**)keywords, &lActID, &pParam, &lTeam))
		return PyErr_Format(PyExc_TypeError, "[JoinActivity] invalid params");
	long lValue[5] = {0};
	if(pParam)
	{
		long lListSize = PyList_Size(pParam);
		for(int ii=0; ii<lListSize && ii<5; ii++)
			lValue[ii] = PyInt_AsLong(PyList_GetItem(pParam, ii));
	}

	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return Py_BuildValue("i", 0);
	if(lTeam)
	{
		int iMemberCount = selfCreature->m_pTeam->mData.Size();
		if(iMemberCount > 0)
		{
			for(int ii=0; ii<iMemberCount; ii++)
			{
				CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
				if(pUser)
					CRegionActManager::GetSingle()->Execute(lActID, pUser, lValue);
			}
		}
		else
			CRegionActManager::GetSingle()->Execute(lActID, selfCreature->m_userInfo, lValue);
	}
	else
		CRegionActManager::GetSingle()->Execute(lActID, selfCreature->m_userInfo, lValue);
	return Py_BuildValue("i", 1);
}

PyObject* CPythonRegionCreature::OverActivity(region_object_python* self, PyObject* args)
{
	long lActID = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	bool bSuccessful = false;
	if(selfCreature->IsUser())
		bSuccessful = CRegionActManager::GetSingle()->Complate(lActID, selfCreature->m_userInfo);
	return Py_BuildValue("i", bSuccessful ? 1 : 0);
}

PyObject* CPythonRegionCreature::StartSchedule(region_object_python* self, PyObject* args)
{
	long lSchedule = 0;
	char *pBarName = 0;
	char *pRetScript = 0;
	if(!PyArg_ParseTuple(args, "lss", &lSchedule, &pBarName, &pRetScript))
		return PyErr_Format(PyExc_TypeError, "[StartSchedule] invalid params");
	if(lSchedule <= 0 || !pRetScript || pRetScript[0] == 0)
		return PyErr_Format(PyExc_TypeError, "[StartSchedule] invalid params");

	_CHECK_CREATURE_TYPE(0);
	if(selfCreature->IsUser())
		g_region->StartSchedule(selfCreature, lSchedule, pRetScript);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::RandomFunction(region_object_python* self, PyObject* args)
{
	PyObject *pListStr = 0;
	PyObject *pListVal = 0;
	if(!PyArg_ParseTuple(args, "OO", &pListStr, &pListVal))
		return PyErr_Format(PyExc_TypeError, "[RandomFunction] invalid params");
	if(!pListStr || !pListVal)
		return PyErr_Format(PyExc_TypeError, "[RandomFunction] invalid params");

	long lListSize = PyList_Size(pListStr);
	lListSize = PyList_Size(pListVal) < lListSize ? PyList_Size(pListVal) : lListSize;
	if(lListSize > 4)
		lListSize = 4;
	int iRandRes = rand() % lListSize;
	char *pValue[4] = {0};

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_random_function);
	g_sendCmd->WriteLong(lListSize);
	g_sendCmd->WriteLong(iRandRes);
	for(int ii=0; ii<lListSize; ii++)
	{
		g_sendCmd->WriteString( PyString_AsString(PyList_GetItem(pListStr, ii)) );
		pValue[ii] = PyString_AsString(PyList_GetItem(pListVal, ii));
	}
	g_sendCmd->WriteString(pValue[iRandRes]);
	_CHECK_CREATURE_TYPE(0);
	selfCreature->RecvCmd(g_sendCmd);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::SystemMsgByTeam(region_object_python* self, PyObject* args)
{
	char *sInfo = PyString_AsString(args);
	if(!sInfo || sInfo[0] == 0)
		return PyErr_Format(PyExc_TypeError, "[SystemMsgByTeam] invalid params");
	_CHECK_CREATURE_TYPE(0);
	if(!selfCreature->IsUser())
		return CScriptEngine::ReturnNone();

	int iMemberCount = selfCreature->m_pTeam->mData.Size();
	if(iMemberCount > 0)
	{
		for(int ii=0; ii<iMemberCount; ii++)
		{
			CRegionUser *pUser = g_region->FindUser(selfCreature->m_pTeam->mData[ii].mDBID);
			if(pUser && pUser->m_dummy)
				pUser->m_dummy->SendSystemMessage(sInfo);
		}
	}
	else
		selfCreature->SendSystemMessage(sInfo);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionCreature::Test1(region_object_python* self, PyObject* args)
{
	long lTimer = PyInt_AsLong(args);
	_CHECK_CREATURE_TYPE(0);
	g_region->BroadcastShowTimerByScene(selfCreature->m_scene, "XXX", lTimer);
	return CScriptEngine::ReturnNone();
}
PyObject* CPythonRegionCreature::Test2(region_object_python* self, PyObject* args)
{
	PyObject *pListStr = 0;
	PyObject *pListVal = 0;
	if(!PyArg_ParseTuple(args, "OO", &pListStr, &pListVal))
		return PyErr_Format(PyExc_TypeError, "[Test2] invalid params");
	if(!pListStr || !pListVal)
		return PyErr_Format(PyExc_TypeError, "[Test2] invalid params");

	_CHECK_CREATURE_TYPE(0);
	long lListSize = PyList_Size(pListStr);
	lListSize = PyList_Size(pListVal) < lListSize ? PyList_Size(pListVal) : lListSize;
	char **pString = RT_NEW char*[lListSize];
	long *pValue = RT_NEW long[lListSize];
	for(int ii=0; ii<lListSize; ii++)
	{
		pString[ii] = PyString_AsString(PyList_GetItem(pListStr, ii));
		pValue[ii] = PyLong_AsLong(PyList_GetItem(pListVal, ii));
	}
	g_region->BroadcastSceneDataByScene(selfCreature->m_scene, lListSize, (const char**)pString, pValue);
	DEL_ARRAY(pValue);
	DEL_ARRAY(pString);
	return CScriptEngine::ReturnNone();
}

//
// init region object type
//
bool CPythonRegionCreature::Initialize(PyObject* module)
{
	//
	// init region creature type
	//
	m_pythonType.tp_base													= CPythonRegionObject::GetPythonType();
	if(PyType_Ready(&m_pythonType) < 0)
		return false;

	//
	// add channel const
	//
	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_NORMAL", CHAT_CHANNEL_NORMAL) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_TEAM", CHAT_CHANNEL_TEAM) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_PARTY", CHAT_CHANNEL_PARTY) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_COUNTRY", CHAT_CHANNEL_COUNTRY) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_PUBLIC", CHAT_CHANNEL_PUBLIC) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_PRIVATE", CHAT_CHANNEL_PRIVATE) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_SYSTEM", CHAT_CHANNEL_SYSTEM) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_GM", CHAT_CHANNEL_GM) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_WORLD", CHAT_CHANNEL_WORLD) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "CHAT_CHANNEL_MAX", CHAT_CHANNEL_MAX) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "FRIEND", SConnection::FRIEND) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "ENEMY", SConnection::ENEMY) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "STUDENT", SConnection::STUDENT) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "PRENTICE", SConnection::PRENTICE) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "TEACHER", SConnection::TEACHER) == -1)
		return false;

	if(PyModule_AddIntConstant(module, "MASTER", SConnection::MASTER) == -1)
		return false;

	return true;
}

//
// create new
//
PyObject* CPythonRegionCreature::CreatePythonObject(CRegionCreature* theCreature)
{
	region_object_python* ret												= PyObject_NEW(region_object_python, &m_pythonType);
	if(!ret)
		return 0;

	ret->m_pObject															= theCreature;
	return reinterpret_cast<PyObject*>(ret);
}

//
// method for region creature
//
PyMethodDef CPythonRegionCreature::m_pMethods[] = 
{
	{"Talk",						(PyCFunction)CPythonRegionCreature::Talk,				METH_VARARGS | METH_KEYWORDS,	""},
	{"MoveTo",						(PyCFunction)CPythonRegionCreature::MoveTo,				METH_VARARGS,					""},
	{"GetItemCountByType",			(PyCFunction)CPythonRegionCreature::GetItemCountByType,	METH_VARARGS | METH_KEYWORDS,	""},
	{"DeleteItemByType",			(PyCFunction)CPythonRegionCreature::DeleteItemByType,	METH_VARARGS | METH_KEYWORDS,	""},
	{"DeleteItemById",				(PyCFunction)CPythonRegionCreature::DeleteItemById,		METH_O,							""},
	{"CheckEquipment",				(PyCFunction)CPythonRegionCreature::CheckEquipment,		METH_VARARGS | METH_KEYWORDS,	""},
	{"AddItem",						(PyCFunction)CPythonRegionCreature::AddItem,			METH_VARARGS | METH_KEYWORDS,	""},
	{"UseItemById",					(PyCFunction)CPythonRegionCreature::UseItemById,		METH_O,							""},
	{"UseItemByType",				(PyCFunction)CPythonRegionCreature::UseItemByType,		METH_VARARGS | METH_KEYWORDS,	""},
	{"BagFreeSpace",				(PyCFunction)CPythonRegionCreature::BagFreeSpace,		METH_O,							""},
	{"BagItems",					(PyCFunction)CPythonRegionCreature::BagItems,			METH_O,							""},
	{"Equipment",					(PyCFunction)CPythonRegionCreature::Equipment,			METH_O,							""},
	{"AreaCheck",					(PyCFunction)CPythonRegionCreature::AreaCheck,			METH_VARARGS,					""},
	{"AreaCheckByName",				(PyCFunction)CPythonRegionCreature::AreaCheckByName,	METH_O,							""},
	{"Distance",					(PyCFunction)CPythonRegionCreature::Distance,			METH_VARARGS,					""},
	{"Connections",					(PyCFunction)CPythonRegionCreature::Connections,		METH_O,							""},
	{"TeamConnection",				(PyCFunction)CPythonRegionCreature::TeamConnection,		METH_O,							""},
	{"EnterLineReq",				(PyCFunction)CPythonRegionCreature::EnterLineReq,		METH_VARARGS,					""},
	{"LeaveLineReq",				(PyCFunction)CPythonRegionCreature::LeaveLineReq,		METH_VARARGS,					""},
	{"Enter_fb",					(PyCFunction)CPythonRegionCreature::Enter_fb,			METH_VARARGS,					""},
	{"Leave_fb",					(PyCFunction)CPythonRegionCreature::Leave_fb,			METH_VARARGS,					""},
	{"AddLevel",					(PyCFunction)CPythonRegionCreature::AddLevel,			METH_O,							""},
	{"AddExp",						(PyCFunction)CPythonRegionCreature::AddExp,				METH_O,							""},
	{"AddCulExp",					(PyCFunction)CPythonRegionCreature::AddCulExp,			METH_O,							""},
	{"AddSoul",						(PyCFunction)CPythonRegionCreature::AddSoul,			METH_O,							""},
	{"AddCredit",					(PyCFunction)CPythonRegionCreature::AddCredit,			METH_O,							""},
	{"AddContribute",				(PyCFunction)CPythonRegionCreature::AddContribute,		METH_O,							""},
	{"AddPoint",					(PyCFunction)CPythonRegionCreature::AddPoint,			METH_O,							""},
	{"AddRecord",					(PyCFunction)CPythonRegionCreature::AddRecord,			METH_O,							""},
	{"AddUnionRecord",				(PyCFunction)CPythonRegionCreature::AddUnionRecord,		METH_O,							""},
	{"AddJobRecord",				(PyCFunction)CPythonRegionCreature::AddJobRecord,		METH_O,							""},
	{"AddPK",						(PyCFunction)CPythonRegionCreature::AddPK,				METH_O,							""},
	{"GetFuncState",				(PyCFunction)CPythonRegionCreature::GetFuncState,		METH_O,							""},
	{"SetFuncState",				(PyCFunction)CPythonRegionCreature::SetFuncState,		METH_VARARGS,					""},
	{"ShowDesignateDialog",			(PyCFunction)CPythonRegionCreature::ShowDesignateDialog,METH_VARARGS | METH_KEYWORDS,	""},
	{"ShowFunctionDialog",			(PyCFunction)CPythonRegionCreature::ShowFunctionDialog,	METH_O,							""},
	{"DisposeTaskInterface",		(PyCFunction)CPythonRegionCreature::DisposeTaskInterface,	METH_VARARGS,				""},
	{"RegionMessageBox",			(PyCFunction)CPythonRegionCreature::RegionMessageBox,	METH_VARARGS,					""},
	{"CanAcceptTask",				(PyCFunction)CPythonRegionCreature::CanAcceptTask,		METH_O,							""},
	{"AcceptTask",					(PyCFunction)CPythonRegionCreature::AcceptTask,			METH_O,							""},
	{"FinishTask",					(PyCFunction)CPythonRegionCreature::FinishTask,			METH_O,							""},
	{"SetToCanFinishTaskByID",		(PyCFunction)CPythonRegionCreature::SetToCanFinishTaskByID,	METH_O,						""},
	{"CancelTask",					(PyCFunction)CPythonRegionCreature::CancelTask,			METH_O,							""},
	{"GetTaskStateByID",			(PyCFunction)CPythonRegionCreature::GetTaskStateByID,	METH_O,							""},
	//{"SetTaskStateByID",			(PyCFunction)CPythonRegionCreature::SetTaskStateByID,	METH_VARARGS,					""},
	{"GetTeamAllMember",			(PyCFunction)CPythonRegionCreature::GetTeamAllMember,	METH_NOARGS,					""},
	{"TeamMoveTo",					(PyCFunction)CPythonRegionCreature::TeamMoveTo,			METH_VARARGS,					""},
	{"TeamAtArea",					(PyCFunction)CPythonRegionCreature::TeamAtArea,			METH_VARARGS,					""},
	{"TeamAtAreaByName",			(PyCFunction)CPythonRegionCreature::TeamAtAreaByName,	METH_O,							""},
	{"GetCurrentSelect",			(PyCFunction)CPythonRegionCreature::GetCurrentSelect,	METH_NOARGS,					""},
	{"GetAllSkillID",				(PyCFunction)CPythonRegionCreature::GetAllSkillID,		METH_NOARGS,					""},
	{"HaveSkillID",					(PyCFunction)CPythonRegionCreature::HaveSkillID,		METH_O,							""},
	{"HaveSkillEffect",				(PyCFunction)CPythonRegionCreature::HaveSkillEffect,	METH_VARARGS,					""},
	{"UseSkill",					(PyCFunction)CPythonRegionCreature::UseSkill,			METH_VARARGS,					""},
	{"AddEffect",					(PyCFunction)CPythonRegionCreature::AddEffect,			METH_O,							""},
	{"IsBattle",					(PyCFunction)CPythonRegionCreature::IsBattle,			METH_NOARGS,					""},
	{"DebaseEquipWear",				(PyCFunction)CPythonRegionCreature::DebaseEquipWear,	METH_VARARGS,					""},
	{"SetGuard",					(PyCFunction)CPythonRegionCreature::SetGuard,			METH_O,							""},
	{"CancelGuard",					(PyCFunction)CPythonRegionCreature::CancelGuard,		METH_O,							""},
	{"Remove",						(PyCFunction)CPythonRegionCreature::Remove,				METH_NOARGS,					""},
	{"Die",							(PyCFunction)CPythonRegionCreature::Die,				METH_O,							""},
	{"DeleteEffect",				(PyCFunction)CPythonRegionCreature::DeleteEffect,		METH_O,							""},
	{"SwitchModel",					(PyCFunction)CPythonRegionCreature::SwitchModel,		METH_O,							""},
	{"AddMoney",					(PyCFunction)CPythonRegionCreature::AddMoney,			METH_O,							""},
	{"AddLifeSkill",				(PyCFunction)CPythonRegionCreature::AddLifeSkill,		METH_O,							""},
	{"DelLifeSkill",				(PyCFunction)CPythonRegionCreature::DelLifeSkill,		METH_O,							""},
	{"SetNPCColorForUser",			(PyCFunction)CPythonRegionCreature::SetNPCColorForUser,	METH_VARARGS,					""},
	{"SetNPCAnimation",				(PyCFunction)CPythonRegionCreature::SetNPCAnimation,	METH_VARARGS,					""},
	{"SetWarder",					(PyCFunction)CPythonRegionCreature::SetWarder,			METH_O,							""},
	{"RemoveWardNPC",				(PyCFunction)CPythonRegionCreature::RemoveWardNPC,		METH_NOARGS,					""},
	{"SendProblem",					(PyCFunction)CPythonRegionCreature::SendProblem,		METH_O,							""},
	{"SendMail",					(PyCFunction)CPythonRegionCreature::SendMail,			METH_VARARGS | METH_KEYWORDS,	""},
	{"CanJoinActivity",				(PyCFunction)CPythonRegionCreature::CanJoinActivity,	METH_VARARGS,					""},
	{"JoinActivity",				(PyCFunction)CPythonRegionCreature::JoinActivity,		METH_VARARGS | METH_KEYWORDS,	""},
	{"OverActivity",				(PyCFunction)CPythonRegionCreature::OverActivity,		METH_O,							""},
	{"StartSchedule",				(PyCFunction)CPythonRegionCreature::StartSchedule,		METH_VARARGS,					""},
	{"RandomFunction",				(PyCFunction)CPythonRegionCreature::RandomFunction,		METH_VARARGS,					""},
	{"SystemMsgByTeam",				(PyCFunction)CPythonRegionCreature::SystemMsgByTeam,	METH_O,							""},
	{"TeamUnionMember",				(PyCFunction)CPythonRegionCreature::TeamUnionMember,	METH_NOARGS,					""},
	{"test1",						(PyCFunction)CPythonRegionCreature::Test1,				METH_O,							""},
	{"test2",						(PyCFunction)CPythonRegionCreature::Test2,				METH_VARARGS,					""},
	{0, 0},
};

//
// attribute
//
PyGetSetDef CPythonRegionCreature::m_pAttributes[] = 
{
	{(char*)"name",					(getter)CPythonRegionCreature::GetName,					0},//名字
	{(char*)"id",					(getter)CPythonRegionCreature::GetID,					0},//ID
	{(char*)"level",				(getter)CPythonRegionCreature::GetLevel,				0},//等级
	{(char*)"exp",					(getter)CPythonRegionCreature::GetExp,					0},//经验
	{(char*)"cul_lv",				(getter)CPythonRegionCreature::GetCulLV,				0},//修为等级
	{(char*)"cul",					(getter)CPythonRegionCreature::GetCul,					0},//修为
	{(char*)"soul",					(getter)CPythonRegionCreature::GetSoul,					(setter)CPythonRegionCreature::SetSoul},//灵力
	{(char*)"credit",				(getter)CPythonRegionCreature::GetCredit,				(setter)CPythonRegionCreature::SetCredit},//仙誉
	{(char*)"contribute",			(getter)CPythonRegionCreature::GetContribute,			(setter)CPythonRegionCreature::SetContribute},//俗名
	{(char*)"point",				(getter)CPythonRegionCreature::GetPoint,				(setter)CPythonRegionCreature::SetPoint},//积分
	{(char*)"pk",					(getter)CPythonRegionCreature::GetPK,					(setter)CPythonRegionCreature::SetPK},//PK
	{(char*)"record",				(getter)CPythonRegionCreature::GetRecord,				(setter)CPythonRegionCreature::SetRecord},//军功
	{(char*)"union_record",			(getter)CPythonRegionCreature::GetUnionRecord,			(setter)CPythonRegionCreature::SetUnionRecord},//帮会贡献
	{(char*)"job_record",			(getter)CPythonRegionCreature::GetJobRecord,			(setter)CPythonRegionCreature::SetJobRecord},//门派贡献
	{(char*)"metier",				(getter)CPythonRegionCreature::GetMetier,				0},//职业
	{(char*)"born_tag",				(getter)CPythonRegionCreature::GetBornTag,				0},//生肖

	{(char*)"hp",					(getter)CPythonRegionCreature::GetHp,					(setter)CPythonRegionCreature::SetHp},
	{(char*)"max_hp",				(getter)CPythonRegionCreature::GetMaxHp,				0},
	{(char*)"mp",					(getter)CPythonRegionCreature::GetMp,					(setter)CPythonRegionCreature::SetMp},
	{(char*)"max_mp",				(getter)CPythonRegionCreature::GetMaxMp,				0},
	{(char*)"attack",				(getter)CPythonRegionCreature::GetAttack,				(setter)CPythonRegionCreature::SetAttack},
	{(char*)"damage",				(getter)CPythonRegionCreature::GetDamage,				(setter)CPythonRegionCreature::SetDamage},
	{(char*)"dodge",				(getter)CPythonRegionCreature::GetDodge,				(setter)CPythonRegionCreature::SetDodge},
	{(char*)"armor",				(getter)CPythonRegionCreature::GetArmor,				(setter)CPythonRegionCreature::SetArmor},
	{(char*)"sex",					(getter)CPythonRegionCreature::GetSex,					0},//性别
	{(char*)"pos",					(getter)CPythonRegionCreature::GetPos,					0},//世界位置
	{(char*)"tile_pos",				(getter)CPythonRegionCreature::GetTilePos,				0},//格子位置
	{(char*)"dir",					(getter)CPythonRegionCreature::GetDir,					0},//当前方向

	{(char*)"is_captain",			(getter)CPythonRegionCreature::GetIsCaptain,			0},//是否是队长
	{(char*)"is_team_member",		(getter)CPythonRegionCreature::GetIsTeamMember,			0},//是否队员
	{(char*)"team_member_count",	(getter)CPythonRegionCreature::GetTeamMemberCount,		0},//当前队伍人数
	{(char*)"team_level",			(getter)CPythonRegionCreature::GetTeamLevel,			0},//最低最高等级

	{(char*)"scene",				(getter)CPythonRegionCreature::GetScene,				0},//场景对象
	{(char*)"is_player",			(getter)CPythonRegionCreature::IsPlayer,				0},//是否是用户
	{(char*)"is_tour",				(getter)CPythonRegionCreature::IsTourNPC,				0},//是否是用户
	{(char*)"money",				(getter)CPythonRegionCreature::GetMoney,				(setter)CPythonRegionCreature::SetMoney},//用户money

	{(char*)"masterID",				(getter)CPythonRegionCreature::GetMasterID,				0},//所属用户ID，没有则为0
	{(char*)"task_count",			(getter)CPythonRegionCreature::GetTaskCount,			0},//是否是用户
	{(char*)"has_task_id",			(getter)CPythonRegionCreature::HasTaskID,				0},//获得全部任务ID

	{(char*)"union",				(getter)CPythonRegionCreature::GetUnionMember,			0},//获得帮派ID，没有则为0
	{0, 0},
};

//
// region creature type
//
PyTypeObject CPythonRegionCreature::m_pythonType = 
{
	PyObject_HEAD_INIT(NULL)
	0,																		/*ob_size*/
	SCRIPT_PYTHON_MODULE_NAME ".RegionCreature",							/*tp_name*/
	sizeof(region_object_python),											/*tp_basicsize*/
	0,																		/*tp_itemsize*/
	0,																		/*tp_dealloc*/
	0,																		/*tp_print*/
	0,																		/*tp_getattr*/
	0,																		/*tp_setattr*/
	0,																		/*tp_compare*/
	0,																		/*tp_repr*/
	0,																		/*tp_as_number*/
	0,																		/*tp_as_sequence*/
	0,																		/*tp_as_mapping*/
	0,																		/*tp_hash*/
	0,																		/*tp_call*/
	0,																		/*tp_str*/
	PyObject_GenericGetAttr,												/*tp_getattro*/
	PyObject_GenericSetAttr,												/*tp_setattro*/
	0,																		/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,														/*tp_flags*/
	0,																		/*tp_doc*/
	0,																		/*tp_traverse*/
	0,																		/*tp_clear*/
	0,																		/*tp_richcompare*/
	0,																		/*tp_weaklistoffset*/
	0,																		/*tp_iter*/
	0,																		/*tp_iternext*/
	CPythonRegionCreature::m_pMethods,										/*tp_methods*/
	0,																		/*tp_members*/
	CPythonRegionCreature::m_pAttributes,									/*tp_getset*/
	0,																		/*tp_base*/
	0,																		/*tp_dict*/
	0,																		/*tp_descr_get*/
	0,																		/*tp_descr_set*/
	0,																		/*tp_dictoffset*/
	0,																		/*tp_init*/
	0,																		/*tp_alloc*/
	0,																		/*tp_new*/
	0,																		/*tp_free*/
	0,																		/*tp_is_gc*/
};

