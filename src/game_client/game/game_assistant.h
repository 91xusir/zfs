


#ifndef GAME_ASSISTANT_H
#define GAME_ASSISTANT_H
#include <string>
#include "gc_synchro_actor.h"




//同目标的距离的平方
inline float DistanceSquare(float vX1,float vY1,float vZ1,float vX2,float vY2,float vZ2)				
{

	//距离的平方
	return (((vX2-vX1)*(vX2-vX1))
		+((vY2-vY1)*(vY2-vY1))
		+((vZ2-vZ1)*(vZ2-vZ1)));
}




inline std::string& operator+(std::string& vrString,int vInt)
{
	
	int Temp1=vInt;
	int Temp2=0;
	int i=0;

	if(Temp1<0)
	{
		vrString+='-';
		Temp1=-Temp1;
	}

	for(;Temp1!=0;)
	{
		Temp2=Temp2*10+(Temp1%10);
		Temp1=Temp1/10;
		i++;
	}

	if(Temp2==0)
	{
		i=1;
	}

	for(;i!=0;i--)
	{
		vrString+=(char)((Temp2%10)+'0');
		Temp2=Temp2/10;
	}

	return vrString;
};

inline GcActor* FindActor(DWORD vID, bool bDead=false)//所有人
{
    guard;
	GcActor* p;
    if (GetWorld()==NULL) return NULL;
	p=GetWorld()->m_ActorManager.FindActive(vID, bDead);
	if(p==NULL)
	{
        GcWorld* pWorld = GetWorld();
        if (pWorld)
        {
            if(pWorld->m_pPlayer)
            {
                if (pWorld->m_pPlayer->ID()==vID)
                {
                    return pWorld->m_pPlayer;
                }
            }
            if(pWorld->m_pPet)
            {
                if (pWorld->m_pPet->ID()==vID)
                {
                    return pWorld->m_pPet;
                }
            }
        }
		return NULL;
	}else
	{
		return p;
	}
    unguard;
}

inline GcActor* FindAllActor(DWORD vID)
{
    guard;
    GcActor* p;
    if (GetWorld()==NULL) return NULL;
    p=GetWorld()->m_ActorManager.FindAll(vID);
    if(p==NULL)
    {
        GcWorld* pWorld = GetWorld();
        if (pWorld)
        {
            if(pWorld->m_pPlayer)
            {
                if (pWorld->m_pPlayer->ID()==vID)
                {
                    return pWorld->m_pPlayer;
                }
            }
            if(pWorld->m_pPet)
            {
                if (pWorld->m_pPet->ID()==vID)
                {
                    return pWorld->m_pPet;
                }
            }
        }
        return NULL;
    }else
    {
        return p;
    }
    unguard;
}

inline void PlayEffectOnce(const char *szFileName, RtgVertex3& vPosition)
{
    guard;
    if (GetWorld()==NULL) return;
    GcActor* p = GetWorld()->m_pPlayer;
    if (p)
    {
        if (p->GetRideState()!=GcActor::ERS_NONE)
        {
            p = GetWorld()->m_pPet;
        }

        if (p)
        {
            RtgVertex3 vPlayerPos(p->mBaseActor.GetMatrix()->Position());
            RtgVertex3 vOffsetPos = vPosition - vPlayerPos;
            p->mBaseActor.PlayEffect(szFileName, false, NULL, vOffsetPos, false);
        }
    }
    unguard;
}

const int MAX_DEGREE_COUNT=360;

inline float QuickSin(int vDegree)//度数//整数
{
	static bool Init=false;
	static float Table[MAX_DEGREE_COUNT];

	if(!Init)
	{
		Init=true;
		//初始化Sin表
		for(int i=0;i<MAX_DEGREE_COUNT;i++)
		{
			Table[i]=sin((float) i*2*PI/MAX_DEGREE_COUNT);
		}
	}
	int Degree=vDegree%MAX_DEGREE_COUNT;
	return Table[Degree];
}

inline float QuickCos(int vDegree)
{
	static bool Init=false;
	static float Table[MAX_DEGREE_COUNT];

	if(!Init)
	{
		Init=true;
		//初始化表
		for(int i=0;i<MAX_DEGREE_COUNT;i++)
		{
			Table[i]=cos((float) i*2*PI/MAX_DEGREE_COUNT);
		}
	}
	int Degree=vDegree%MAX_DEGREE_COUNT;
	return Table[Degree];
}




#endif//GAME_ASSISTANT_H
