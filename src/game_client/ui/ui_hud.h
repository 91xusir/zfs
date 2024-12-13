
#ifndef __HUD_H__
#define __HUD_H__

class CGameObject;
class CGameSystemImp;

struct EditBox3D
{
	std::string text;
	SRect3D   rect;
	int width,height;
	SColor   color;
    EditBox3D()
	{
	}
	~EditBox3D()
	{
	}
	void SetText(const string  t)
	{
		text = t;
	}
	void SetPos(RtgVertex3 pos)
	{
		rect.s.left = pos.x - width/2;
		rect.s.top = pos.y - 24;
		rect.z = pos.z;
		rect.s.right = pos.x + width/2;
		rect.s.bottom = rect.s.top + height;
	}
	void SetSize(int x,int y)
	{
		width = x;
		height = y;
	}
	void Render(RtgDevice& inDevice)
	{
		inDevice.SetRenderState(RTGRS_LIGHTING,FALSE);
		inDevice.SetFogEnable(FALSE);
		DWORD s_dwFSAA = inDevice.GetRenderState(RTGRS_FULLSCRENE_ANTIALIASING);
		inDevice.SetRenderState( RTGRS_FULLSCRENE_ANTIALIASING,    FALSE );
		g_ui.getFontManager()->DrawString3D(text,rect,color,true,SUiTextAlignment(alignCenter,alignCenter));
		g_ui.getFontManager()->Render();
		inDevice.SetRenderState( RTGRS_FULLSCRENE_ANTIALIASING, s_dwFSAA);
		inDevice.SetRenderState(RTGRS_LIGHTING,TRUE);
		inDevice.SetFogEnable(TRUE);
	}

};

class CHud
{
public:
	enum EShowItem
	{
		esiSpeak   = 0x01,   // ˵��
		esiName    = 0x02,   // ����
		esiParty   = 0x04,   // ����
		esiTitle   = 0x08,   // �����ǳ�(ֻ�а�����ʾ�Ż���ʾ)
		esiFaction = 0x10,   // ����
		esiHPBar   = 0x20,   // HP��
		//esiDynInfo = 0x40,	 // �˺�ֵ
		esiMsg     = 0x40,   // ��Ϣ
		esiStall   = 0x80,  // ��̯����
	};


public:
	CHud();
	virtual ~CHud();
	// ��ʼ��HUD
	bool	Init();
	// ����HUD����Ļ�ϵ�λ��
	void	SetPosition(RtgVertex3 pos);
	//void    SetWorldPos(RtgVertex3 pos);
	// ��ʾHUD
	void	Show();
	// ��ȾHUD
	void	Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
	// HUD��ʾ��־�ı�ʱ�Ļص�
	void	OnHudFlagChanged(BYTE newFlag);
	// ����HUD��ʾ״̬
	void    Show(bool flag) {m_bshow = flag;}
	// ����HUD
	void	Hide() {m_bshow = false;}
public:
	// ����˵���ı�����
	void	SetSpeakText(const string& text);
	// ����˵��ʱ���
	void	SetSpeakTime();
	// ����˵���ı���ɫ
	void	SetSpeakColor(DWORD color);
	// ���ù���/�����ı�����
	void	SetPartyText(const string& text);
	// ���ù���/�����ı���ɫ
	void	SetPartyColor(DWORD color);
	// ���ù���/�����־ͼƬ
	void	SetPartyLogo(IUiImage* image);
	// ���������ı�����
	void	SetNameText(const string& text);
	// ���������ı���ɫ
	void	SetNameColor(DWORD color);
	// ������Ϣ�ı�����
	void	SetMessageText(const string& text);
	// ������Ϣ�ı���ɫ
	void	SetMessageColor(DWORD color);
	// ����Ѫ��ֵ(0.0-1.0)
	void    SetBloodValue(float value);
	// ���ð�̯��Ϣ�ı�
	void    SetStallText(const string& text);
	// ���óƺ��ı�����
	void    SetTitleText(const string& text);
	// ���óƺ��ı���ɫ
	void	SetTitleColor(DWORD color);
	// ���óƺű�־ͼƬ
	void	SetTitleLogo(IUiImage* image);

	void    ShowSpeak(bool flag)    { if (flag&&espeak) m_cShowItem|=esiSpeak;   else m_cShowItem&=(~esiSpeak);  }
	void    ShowName(bool flag)     { if (flag&&ename) m_cShowItem|=esiName;    else m_cShowItem&=(~esiName);   }
	void    ShowParty(bool flag)    { if (flag) m_cShowItem|=esiParty;   else m_cShowItem&=(~esiParty);   }
	void    ShowTitle(bool flag)    { if (flag) m_cShowItem|=esiTitle;    else m_cShowItem&=(~esiTitle);  }
	void    ShowFaction(bool flag)  { if (flag) m_cShowItem|=esiFaction; else m_cShowItem&=(~esiFaction); }
	void    ShowHPBar(bool flag)    { if (flag) m_cShowItem|=esiHPBar;   else m_cShowItem&=(~esiHPBar);   }
	void    ShowMsg(bool flag)      { if (flag) m_cShowItem|=esiMsg;     else m_cShowItem&=(~esiMsg);	  }
	void    ShowStall(bool flag)    { if (flag) m_cShowItem|=esiStall;   else m_cShowItem&=(~esiStall);   }

	void    SetShowFlag(BYTE flag)  { m_cShowItem = flag; }
	BYTE    GetShowFlag()           { return m_cShowItem; }
	void    ApplyFlag()             { m_cShowItem = s_cShowItem; }

	bool    IsShowSpeak()           { return (m_cShowItem & esiSpeak	& s_cShowItem)!=0;	}
	bool    IsShowName()            { return (m_cShowItem & esiName		& s_cShowItem)!=0;	}
	bool    IsShowParty()           { return (m_cShowItem & esiParty	& s_cShowItem)!=0;	}
	bool    IsShowTitle()           { return (m_cShowItem & esiTitle    & s_cShowItem)!=0;	}
	bool    IsShowFaction()         { return (m_cShowItem & esiFaction	& s_cShowItem)!=0;	}
	bool    IsShowHPBar()           { return (m_cShowItem & esiHPBar	& s_cShowItem)!=0;	}
	bool    IsShowMsg()             { return (m_cShowItem & esiMsg		& s_cShowItem)!=0;	}
	bool    IsShowStall()           { return (m_cShowItem & esiStall    & s_cShowItem)!=0;  }

	CUiEditBox* GetSpeakHud()		{ return m_pSpeakUI; }
	void    SetNameShow(bool m_name)           { ename=m_name; }

private:
	//IUiImage*		m_partyLogo;      // ����/�����־ͼƬ
	bool            m_bshow;           // �Ƿ���ʾHUD
	int				m_speakDuration;   // ˵������ʱ��
	int				m_speakTimeStamp;  // ˵��ʱ���
	float			m_bloodValue;      // Ѫ��ֵ
	//IUiImage*		m_bkimage;        // ����ͼƬ
	CUiProgressBar*	m_pBloodBar;      // Ѫ���������ؼ�
	CUiEditBox*		m_pSpeakUI;       // ˵���ı���ؼ�
	CUiEditBox*		m_pNameUI;        // �����ı���ؼ�
	CUiEditBox*		m_pPartyUI;       // ����/�����ı���ؼ�
	EditBox3D       m_name;           // 3D�����ı���
	EditBox3D       m_title;          // 3D�ƺ��ı���
	EditBox3D       m_pPartyUi;       // 3D����/�����ı���
	CUiButton*      m_pStall;         // ��̯��ť�ؼ�
	DWORD			m_speakColor;     // ˵��������ɫ
	string          m_sname;          // �����ַ���
	string          m_szStall;        // ��̯��Ϣ�ַ���
	RtgVertex3      m_pos;            // HUDλ������
public:
	static bool            ename;      // �Ƿ���ʾ���ֵľ�̬��־
	static bool            espeak;     // �Ƿ���ʾ˵���ľ�̬��־

	BYTE            m_cShowItem;       // ��ǰHUD��ʾ��Ŀ�ı�־λ

public:
	static BYTE			s_cShowItem;  // ȫ��HUD��ʾ��Ŀ�ľ�̬��־λ

};

#endif //__HUD_H__
