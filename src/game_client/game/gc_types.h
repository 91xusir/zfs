

#ifndef GC_TYPES_H
#define GC_TYPES_H

// 人物链表的Node
struct GcsActorListNode
{
    GcSynchroActor* pNext;
    GcSynchroActor* pPrev;
    GcsActorListNode() { pNext=pPrev=0; }
};

// 人物链表的Head
struct GcsActorListHead
{
    GcSynchroActor* pFirst;
    GcSynchroActor* pLast;
    GcsActorListHead() { pFirst=pLast=0;}
};

class GcUserInput
{
public:
    GcUserInput() {}
    virtual ~GcUserInput() {}
    virtual void OnRun(float fSecond)=0;
    virtual void OnBeginRender()=0;
    virtual void OnRender(float fSecond)=0;
    virtual void OnEndRender()=0;
    virtual void OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY)=0;
	virtual void OnMouseWheel(int iButton, long vDelta)=0;
 	virtual void OnMouseUp(int iButton, int x, int y)=0;
    virtual void OnMouseLDown(int iButton, int x, int y)=0;
    virtual void OnMouseLDClick(int iButton,int x, int y)=0;
    virtual void OnMouseRDown(int iButton,int x,int y)=0;
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY)=0;
    virtual void OnKeyDown(int iButton,int iKey)=0;
    virtual void OnKeyUp(int iButton,int iKey)=0;
	virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY)=0;
};

struct GcSetting
{
    struct Graph
    {
        short sTexture;
        short sEffect;
        short sShadow;

        bool Same(Graph& graph)
        {
            return (sTexture==graph.sTexture
                && sEffect==graph.sEffect
                && sShadow==graph.sShadow
                );
        }
    };
	struct Light
	{
		bool  bEnvref;
		bool  bObjref;
		bool  bPosteffect;
		bool  bObjeffect;
	}light;
	bool  bFsaa;

    Graph graphLow;
	Graph graphMiddle1;
	Graph graphMiddle2;
    Graph graphHigh;
    Graph graph;

    struct Audio
    {
        int     iMusicVal;
        int     iSoundVal;
        bool    bUse3D;
    }audio;

    struct Game
    {
        short   cMaxDisplay;
        bool    bLockCamera;
        char    cHUD;
		short   sUIInputProject;
		short   sUIShowProject;
		
		bool    bRefuse;		//拒绝邀请
    }game;

};

#endif//GC_TYPES_H


/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
