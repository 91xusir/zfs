
#ifndef _GAMECLIENT_SYNCHRO_ITEM_H_
#define _GAMECLIENT_SYNCHRO_ITEM_H_

class GcRenderItem : public RtsRenderAgent
{
    RT_DECLARE_DYNAMIC(GcRenderItem, RtsRenderAgent, 0, "gc")
public:
	DWORD               dwOID;          // 唯一 ID
	DWORD               dwTID;          // 道具的类型 ID
	BYTE				color;			// 道具的颜色
	BYTE				cBind;			// 绑定

    GcRenderItem(){}
    virtual ~GcRenderItem(){}
    void SetGraph(CRT_ActorInstance* pGraph)    { m_pGraph = pGraph;}
    virtual CRT_ActorInstance* GetActor()       { return m_pGraph; }
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
private:
    CRT_ActorInstance* m_pGraph;
};

class GcSynchroItem
{
public:
    struct SGcsItem
    {
        DWORD               dwOID;          // 唯一 ID
        DWORD               dwTID;          // 道具的类型 ID
		BYTE				color;			// 道具的颜色
		BYTE				cBind;			// 绑定
        float               fDownloadTime;  // 下载数据的标志
        bool                bDisplay;       // 是否显示
        CRT_ActorInstance*  pGraph;         // 图形
		float				zPosGround;		// 落到地面后，地面的z值
		float				zPosFall;		// 下落过程中的z值
		float				xPosFall;		// 下落过程中的x值
		float				yPosFall;		// 下落过程中的y值			//ac.ma  for auto_pick_item()
		float				degree;			// 下落过程中的角度
		float				timeFall;		// 下落过程中的剩余时间
        GcRenderItem        render;
		BYTE				dropStyle;		// 下落方式(EItemDropStyle)
        SGcsItem()
        {
            dwOID = dwTID = 0;
            fDownloadTime = 0;
            bDisplay = false;
            pGraph = NULL;
        }
    };

public:
    GcSynchroItem();
    virtual ~GcSynchroItem();

    bool Init();
    bool Clear();

    void OnBeginDownloadSnapshot(); // 开始网络刷新位置等信息
    void OnEndDownloadSnapshot();   // 结束网络刷新位置等信息，如果有的物件没有被刷新就会被Disable
    void OnDownloadActorSnapshot(DWORD dwOID, DWORD dwTID, BYTE itemColor, BYTE itemBind, float fX, float fY, float fZ, BYTE cDir, BYTE dropStyle);

    void OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene);
    void Run(float fSecond);

	SGcsItem* Find(DWORD dwOID);

	SGcsItem* NewItem(DWORD dwOID, DWORD dwTID, BYTE itemColor, BYTE itemBind, float fX, float fY, float fZ, BYTE cDir, BYTE dropStyle);
	void DeleteItem(SGcsItem* pActor);

    bool HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, bool bGetNearest, DWORD &outOID, DWORD &outType, DWORD &outColor, DWORD &outBind);

	float _distant(GcSynchroItem::SGcsItem* pItem);
	bool FindMapItems();

protected:
    EXT_SPACE::unordered_map<DWORD, SGcsItem*> m_mapItems;
    float                           m_fDownloadTime;    // 下载数据的时间

};

#endif // _GAMECLIENT_SYNCHRO_ITEM_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
