
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
		esiSpeak   = 0x01,   // 说话
		esiName    = 0x02,   // 名字
		esiParty   = 0x04,   // 帮派
		esiTitle   = 0x08,   // 帮派昵称(只有帮派显示才会显示)
		esiFaction = 0x10,   // 门派
		esiHPBar   = 0x20,   // HP条		
		//esiDynInfo = 0x40,	 // 伤害值
		esiMsg     = 0x40,   // 信息
		esiStall   = 0x80,  // 摆摊店名
	};
	

public:
	CHud();
	virtual ~CHud();

	bool	Init();
	void	SetPosition(RtgVertex3 pos);
	//void    SetWorldPos(RtgVertex3 pos);
	void	Show();
	void	Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
	void	OnHudFlagChanged(BYTE newFlag);
	void    Show(bool flag) {m_bshow = flag;}
	void	Hide() {m_bshow = false;}
public:
	void	SetSpeakText(const string& text);
	void	SetSpeakTime();
	void	SetSpeakColor(DWORD color);
	void	SetPartyText(const string& text);
	void	SetPartyColor(DWORD color);
	void	SetPartyLogo(IUiImage* image);
	void	SetNameText(const string& text);
	void	SetNameColor(DWORD color);
	void	SetMessageText(const string& text);
	void	SetMessageColor(DWORD color);
	void    SetBloodValue(float value);
	void    SetStallText(const string& text);
	void    SetTitleText(const string& text);
	void	SetTitleColor(DWORD color);
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
	//IUiImage*		m_partyLogo;	
	bool            m_bshow;
	int				m_speakDuration;
	int				m_speakTimeStamp;	
	float			m_bloodValue;	
	//IUiImage*		m_bkimage;
	CUiProgressBar*	m_pBloodBar;
	CUiEditBox*		m_pSpeakUI;
	CUiEditBox*		m_pNameUI;
	CUiEditBox*		m_pPartyUI;
	EditBox3D       m_name;
	EditBox3D       m_title;
	EditBox3D       m_pPartyUi;
	CUiButton*      m_pStall;
	DWORD			m_speakColor;
	string          m_sname;
	string          m_szStall;
	RtgVertex3      m_pos;
public:
	static bool            ename;
	static bool            espeak;

	BYTE            m_cShowItem;

public:
	static BYTE			s_cShowItem;
	
};

#endif //__HUD_H__