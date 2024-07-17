#ifndef _INC_UI_FORM_ITEMSINFO_H_
#define _INC_UI_FORM_ITEMSINFO_H_

class GcActor;

class UIFormItemsInfo
{
public:
	UIFormItemsInfo();
	~UIFormItemsInfo();

	void Show();
	void Hide();
	bool IsShowing();										// 是否已经显示
	void Refresh();
	void SetPos(int x, int y) { m_pFrmThis->Move(SPoint(x, y)); }

	bool MouseRClick(int x, int y);
	bool OnCommand(GcActor* pTargetActor);
	void OnPopupMenu(RtwPopupMenu* pPopupMenu, GcActor* pTargetActor, int iData);

	bool Is_Widget_Parts(const string& id, CItemContainerBase::EEquipParts& outPart);

	DWORD m_PlayerID;
	string m_PlayerName;
	CItemContainerBase m_ItemsContainter;

	int m_nMaxDistance;

private:
	RtwWidget *m_pFrmThis;

	RtwWidget *m_pPlayerName;
	RtwButton *m_btnParts[CItemContainerBase::MAX_EQUIP_PARTS];

private:
	void OnPreDraw();

	void OnGetHintText_Parts(RtwWidget* sender, void*);


	RtwImage::Ptr m_ptrHoverImage;
	char m_iconUnquip[CItemContainerBase::MAX_EQUIP_PARTS][PATH_STRING_LEN];
};

#endif // _INC_UI_FORM_ITEMSINFO_H_
