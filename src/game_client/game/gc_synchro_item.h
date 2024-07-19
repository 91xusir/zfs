
#ifndef _GAMECLIENT_SYNCHRO_ITEM_H_
#define _GAMECLIENT_SYNCHRO_ITEM_H_

class GcRenderItem : public RtsRenderAgent
{
    RT_DECLARE_DYNAMIC(GcRenderItem, RtsRenderAgent, 0, "gc")
public:
	DWORD               dwOID;          // Ψһ ID
	DWORD               dwTID;          // ���ߵ����� ID
	BYTE				color;			// ���ߵ���ɫ
	BYTE				cBind;			// ��

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
        DWORD               dwOID;          // Ψһ ID
        DWORD               dwTID;          // ���ߵ����� ID
		BYTE				color;			// ���ߵ���ɫ
		BYTE				cBind;			// ��
        float               fDownloadTime;  // �������ݵı�־
        bool                bDisplay;       // �Ƿ���ʾ
        CRT_ActorInstance*  pGraph;         // ͼ��
		float				zPosGround;		// �䵽����󣬵����zֵ
		float				zPosFall;		// ��������е�zֵ
		float				xPosFall;		// ��������е�xֵ
		float				yPosFall;		// ��������е�yֵ			//ac.ma  for auto_pick_item()
		float				degree;			// ��������еĽǶ�
		float				timeFall;		// ��������е�ʣ��ʱ��
        GcRenderItem        render;
		BYTE				dropStyle;		// ���䷽ʽ(EItemDropStyle)
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

    void OnBeginDownloadSnapshot(); // ��ʼ����ˢ��λ�õ���Ϣ
    void OnEndDownloadSnapshot();   // ��������ˢ��λ�õ���Ϣ������е����û�б�ˢ�¾ͻᱻDisable
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
    float                           m_fDownloadTime;    // �������ݵ�ʱ��

};

#endif // _GAMECLIENT_SYNCHRO_ITEM_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
