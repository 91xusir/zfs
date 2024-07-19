/*
 * =====================================================================================
 *
 *       Filename:  game_object.h
 *
 *    Description:  Game object for ZFS server
 *
 *        Version:  1.0
 *        Created:  11/14/2008 11:35:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Louis Huang (louis), louis.huang@yqidea.com
 *        Company:  YQIdea
 *
 * =====================================================================================
 */

#ifndef __CG_GAME_OBJECT_H__
#define __CG_GAME_OBJECT_H__

struct STimer
{
	bool   valid;
	bool   active;
	DWORD  id;
	DWORD  interval;
	long   delay;
};


class CGameObject : public RtObject
{
public:
    CGameObject();
    virtual ~CGameObject();

	// timer
	virtual void OnTimer(DWORD id);
	void SetTimer(DWORD id,DWORD interval);
	void RemoveTimer(DWORD id);
	bool ActiveTimer(DWORD id,bool active);
	bool StopTimer(DWORD id);
	void ClearTimer();
	int  UpdateTimer(DWORD now);
	STimer *FindTimer(DWORD id);
	list<STimer*> m_timerList;

	// timer system support
	static void UpdateTimerObject(DWORD delta);
	void AddTimerObject(CGameObject *object);
	void RemoveTimerObject(CGameObject *object);

	// object flag
	DWORD GetObjectId() { return m_oId; } 
	void  SetObjectId(ULONG id) { m_oId = id; }

	DWORD  m_oId;
	char   m_flag;
	static DWORD m_nextObjId;
	static DWORD m_objIdStepValue;

	static DWORD m_timerCalled;
};

#endif
