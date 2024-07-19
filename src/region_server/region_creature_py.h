//********************************************************************
//	created:	24:12:2009   13:21
//	filename: 	region_creature_py.h
//	author:		tiamo
//	purpose:	python interface for region creature
//********************************************************************

#pragma once

//
// creature
//
class CPythonRegionCreature
{
public:
	//
	// create region object
	//
	static PyObject* CreatePythonObject(CRegionCreature* theServer);

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
	// get name
	//
	static PyObject* GetName(region_object_python* self, void* unused);

	//
	// get id
	//
	static PyObject* GetID(region_object_python* self, void* unused);

	//
	// get level
	//
	static PyObject* GetLevel(region_object_python* self, void* unused);
	static int SetLevel(region_object_python* self, PyObject* lv);

	//
	// get exp
	//
	static PyObject* GetExp(region_object_python* self, void* unused);
	static int SetExp(region_object_python* self, PyObject* exp);

	//
	// get cul
	//
	static PyObject* GetCulLV(region_object_python* self, void* unused);
	static PyObject* GetCul(region_object_python* self, void* unused);
	static int SetCul(region_object_python* self, PyObject* cul);

	//
	// get soul
	//
	static PyObject* GetSoul(region_object_python* self, void* unused);
	static int SetSoul(region_object_python* self, PyObject* soul);

	//
	// get credit
	//
	static PyObject* GetCredit(region_object_python* self, void* unused);
	static int SetCredit(region_object_python* self, PyObject* credit);

	//
	// get contribute
	//
	static PyObject* GetContribute(region_object_python* self, void* unused);
	static int SetContribute(region_object_python* self, PyObject* contribute);

	//
	// get point
	//
	static PyObject* GetPoint(region_object_python* self, void* unused);
	static int SetPoint(region_object_python* self, PyObject* point);

	//
	// get pk
	//
	static PyObject* GetPK(region_object_python* self, void* unused);
	static int SetPK(region_object_python* self, PyObject* pk);
	static PyObject* AddPK(region_object_python* self, PyObject* pk);

	//
	// get record
	//
	static PyObject* GetRecord(region_object_python* self, void* unused);
	static int SetRecord(region_object_python* self, PyObject* record);

	//
	// get union record
	//
	static PyObject* GetUnionRecord(region_object_python* self, void* unused);
	static int SetUnionRecord(region_object_python* self, PyObject* unionrecord);

	//
	// get job record
	//
	static PyObject* GetJobRecord(region_object_python* self, void* unused);
	static int SetJobRecord(region_object_python* self, PyObject* jobrecord);

	//
	// get metier
	//
	static PyObject* GetMetier(region_object_python* self, void* unused);

	//
	// get born tag
	//
	static PyObject* GetBornTag(region_object_python* self, void* unused);

	//
	// get hp
	//
	static PyObject* GetHp(region_object_python* self, void* unused);

	//
	// get max hp
	//
	static PyObject* GetMaxHp(region_object_python* self, void* unused);

	//
	// set hp
	//
	static int SetHp(region_object_python* self, PyObject* hp);

	//
	// get mp
	//
	static PyObject* GetMp(region_object_python* self, void* unused);

	//
	// get max mp
	//
	static PyObject* GetMaxMp(region_object_python* self, void* unused);

	//
	// set mp
	//
	static int SetMp(region_object_python* self, PyObject* mp);

	//’Ω∂∑ Ù–‘
	static PyObject* GetAttack(region_object_python* self, void* unused);
	static int SetAttack(region_object_python* self, PyObject* attack);
	static PyObject* GetDamage(region_object_python* self, void* unused);
	static int SetDamage(region_object_python* self, PyObject* damage);
	static PyObject* GetDodge(region_object_python* self, void* unused);
	static int SetDodge(region_object_python* self, PyObject* dodge);
	static PyObject* GetArmor(region_object_python* self, void* unused);
	static int SetArmor(region_object_python* self, PyObject* armor);

	//
	// get sex
	//
	static PyObject* GetSex(region_object_python* self, void* unused);

	//
	// get pos
	//
	static PyObject* GetPos(region_object_python* self, void* unused);

	//
	// get tile pos
	//
	static PyObject* GetTilePos(region_object_python* self, void* unused);

	//
	// get dir
	//
	static PyObject* GetDir(region_object_python* self, void* unused);

	//
	// is captain
	//
	static PyObject* GetIsCaptain(region_object_python* self, void* unused);

	//
	// is team member
	//
	static PyObject* GetIsTeamMember(region_object_python* self, void* unused);

	//
	// team member count
	//
	static PyObject* GetTeamMemberCount(region_object_python* self, void* unused);

	//
	// team max min level
	//
	static PyObject* GetTeamLevel(region_object_python* self, void* unused);

	//
	// get scene
	//
	static PyObject* GetScene(region_object_python* self, void* unused);

	//
	// get task count
	//
	static PyObject* GetTaskCount(region_object_python* self, void* unused);

	//
	// get master id
	//
	static PyObject* GetMasterID(region_object_python* self, void* unused);

	//
	// get task id s
	//
	static PyObject* HasTaskID(region_object_python* self, void* unused);

	//
	// is player
	//
	static PyObject* IsPlayer(region_object_python* self, void* unused);

	//
	// is tour npc
	//
	static PyObject* IsTourNPC(region_object_python* self, void* unused);

	//
	// money
	//
	static PyObject* GetMoney(region_object_python* self, void* unused);
	static int SetMoney(region_object_python* self, PyObject* money);
	static PyObject* AddMoney(region_object_python* self, PyObject* money);

	//
	// union
	//
	static PyObject* GetUnionMember(region_object_python* self, void* unused);
	static PyObject* TeamUnionMember(region_object_python* self, void* unused);

	//
	// FunctionState
	//
	static PyObject* GetFuncState(region_object_python* self, PyObject* args);
	static PyObject* SetFuncState(region_object_python* self, PyObject* args);

	//
	// talk
	//
	static PyObject* Talk(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// moveto(x,y)
	//
	static PyObject* MoveTo(region_object_python* self, PyObject* args);
	
	//
	// get count
	//
	static PyObject* GetItemCountByType(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// delete item by type
	//
	static PyObject* DeleteItemByType(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// delete item by id
	//
	static PyObject* DeleteItemById(region_object_python* self, PyObject* args);

	//
	// check equipment
	//
	static PyObject* CheckEquipment(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// additem(type, level, color)
	//
	static PyObject* AddItem(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// use item by id
	//
	static PyObject* UseItemById(region_object_python* self, PyObject* args);

	//
	// use item by type
	//
	static PyObject* UseItemByType(region_object_python* self, PyObject* args, PyObject* kw);

	//
	// get free space
	//
	static PyObject* BagFreeSpace(region_object_python* self, PyObject* args);

	//
	// get bag items
	//
	static PyObject* BagItems(region_object_python* self, PyObject* args);

	//
	// get equipment
	//
	static PyObject* Equipment(region_object_python* self, PyObject* args);

	//
	// area check
	//
	static PyObject* AreaCheck(region_object_python* self, PyObject* args);
	static PyObject* AreaCheckByName(region_object_python* self, PyObject* args);

	//
	// distance
	//
	static PyObject* Distance(region_object_python* self, PyObject* args);

	//
	// connections count
	//
	static PyObject* Connections(region_object_python* self, PyObject* args);

	//
	// team connection
	//
	static PyObject* TeamConnection(region_object_python* self, PyObject* args);

	//
	// enterlinereq
	//
	static PyObject* EnterLineReq(region_object_python* self, PyObject* args);
	//
	// enterlinereq
	//
	static PyObject* LeaveLineReq(region_object_python* self, PyObject* args);


	static PyObject* Enter_fb(region_object_python* self, PyObject* args);


	static PyObject* Leave_fb(region_object_python* self, PyObject* args);


	static PyObject* AddLevel(region_object_python* self, PyObject* args);
	static PyObject* AddExp(region_object_python* self, PyObject* args);
	static PyObject* AddCulExp(region_object_python* self, PyObject* args);
	static PyObject* AddSoul(region_object_python* self, PyObject* args);
	static PyObject* AddCredit(region_object_python* self, PyObject* args);
	static PyObject* AddContribute(region_object_python* self, PyObject* args);
	static PyObject* AddPoint(region_object_python* self, PyObject* args);
	static PyObject* AddRecord(region_object_python* self, PyObject* args);
	static PyObject* AddUnionRecord(region_object_python* self, PyObject* args);
	static PyObject* AddJobRecord(region_object_python* self, PyObject* args);

		
	static PyObject* ShowDesignateDialog(region_object_python* self, PyObject* args, PyObject* kw);
	static PyObject* ShowFunctionDialog(region_object_python* self, PyObject* args);
	static PyObject* DisposeTaskInterface(region_object_python* self, PyObject* args);
	static PyObject* RegionMessageBox(region_object_python* self, PyObject* args);


	static PyObject* CanAcceptTask(region_object_python* self, PyObject* args);
	static PyObject* AcceptTask(region_object_python* self, PyObject* args);
	static PyObject* FinishTask(region_object_python* self, PyObject* args);
	static PyObject* SetToCanFinishTaskByID(region_object_python* self, PyObject* args);
	static PyObject* CancelTask(region_object_python* self, PyObject* args);
	static PyObject* GetTaskStateByID(region_object_python* self, PyObject* args);
	static PyObject* SetTaskStateByID(region_object_python* self, PyObject* args);


	static PyObject* GetTeamAllMember(region_object_python* self, PyObject* args);
	static PyObject* TeamMoveTo(region_object_python* self, PyObject* args);
	static PyObject* TeamAtArea(region_object_python* self, PyObject* args);
	static PyObject* TeamAtAreaByName(region_object_python* self, PyObject* args);

	static PyObject* GetCurrentSelect(region_object_python* self, PyObject* args);
	static PyObject* GetAllSkillID(region_object_python* self, PyObject* args);
	static PyObject* HaveSkillID(region_object_python* self, PyObject* args);
	static PyObject* HaveSkillEffect(region_object_python* self, PyObject* args);
	static PyObject* UseSkill(region_object_python* self, PyObject* args);
	static PyObject* AddEffect(region_object_python* self, PyObject* args);

	static PyObject* IsBattle(region_object_python* self, PyObject* args);

	static PyObject* DebaseEquipWear(region_object_python* self, PyObject* args);

	static PyObject* SetGuard(region_object_python* self, PyObject* args);
	static PyObject* CancelGuard(region_object_python* self, PyObject* args);
	static PyObject* Remove(region_object_python* self, PyObject* args);
	static PyObject* Die(region_object_python* self, PyObject* args);
	static PyObject* DeleteEffect(region_object_python* self, PyObject* args);
	static PyObject* SwitchModel(region_object_python* self, PyObject* args);

	static PyObject* AddLifeSkill(region_object_python* self, PyObject* args);
	static PyObject* DelLifeSkill(region_object_python* self, PyObject* args);

	static PyObject* SetNPCColorForUser(region_object_python* self, PyObject* args);
	static PyObject* SetNPCAnimation(region_object_python* self, PyObject* args);

	static PyObject* SetWarder(region_object_python* self, PyObject* args);
	static PyObject* RemoveWardNPC(region_object_python* self, PyObject* args);

	static PyObject* SendProblem(region_object_python* self, PyObject* args);
	static PyObject* SendMail(region_object_python* self, PyObject* args, PyObject* kw);

	static PyObject* CanJoinActivity(region_object_python* self, PyObject* args);
	static PyObject* JoinActivity(region_object_python* self, PyObject* args, PyObject* kw);
	static PyObject* OverActivity(region_object_python* self, PyObject* args);
	static PyObject* StartSchedule(region_object_python* self, PyObject* args);
	static PyObject* RandomFunction(region_object_python* self, PyObject* args);
	static PyObject* SystemMsgByTeam(region_object_python* self, PyObject* args);

	static PyObject* Test1(region_object_python* self, PyObject* args);
	static PyObject* Test2(region_object_python* self, PyObject* args);

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

