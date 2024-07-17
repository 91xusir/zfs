

#ifndef GAME_SCRIPT_INTERFACE_H 
#define GAME_SCRIPT_INTERFACE_H

//------------------------------------------Cµ÷ÓÃ½Å±¾------------------------------------------
void GameReceiveMessage(BYTE vChannel,char vName[],char vMsg[]);








#endif//GAME_SCRIPT_INTERFACE_H





/*

RT_PY_BEGIN_METHOD_MAP(rtc)
	RT_PY_METHOD_ENTRY_2(XXX_Login, GcWorld, RT_VT_STRING, RT_VT_STRING),
	RT_PY_METHOD_ENTRY_0(XXX_CreateCharacter, GcWorld, CreateCharacter),
RT_PY_END_METHOD_MAP(rtc)

RT_PY_BEGIN_ATTRIBUTE_MAP(GcWorld)
	RT_PY_GET_PROPERTY("Scene", GcWorld, GetScene, RT_VT_OBJECT),
RT_PY_END_ATTRIBUTE_MAP(GcWorld)

RT_PY_TYPE_DECL(GcWorld, RtPython::BaseObjectType());

class CCC : public RtObject
{
};

class GcWorld : public RtPyObject<CCC>
{
	RT_DECLARE_DYNAMIC(GcWorld, RtObject, 0, 0);
};



RT_PY_BEGIN_GET_OBJECT(rtc, GcWorld)
	object = GetWorld();
RT_PY_END_GET_OBJECT(rtc, GcWorld)

ABC = GcWorld()
ABC.XXX_Login()
ABC.Scene.

*/