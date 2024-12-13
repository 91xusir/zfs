
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
	// 初始化HUD
	bool	Init();
	// 设置HUD在屏幕上的位置
	void	SetPosition(RtgVertex3 pos);
	//void    SetWorldPos(RtgVertex3 pos);
	// 显示HUD
	void	Show();
	// 渲染HUD
	void	Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
	// HUD显示标志改变时的回调
	void	OnHudFlagChanged(BYTE newFlag);
	// 设置HUD显示状态
	void    Show(bool flag) {m_bshow = flag;}
	// 隐藏HUD
	void	Hide() {m_bshow = false;}
public:
	// 设置说话文本内容
	void	SetSpeakText(const string& text);
	// 设置说话时间戳
	void	SetSpeakTime();
	// 设置说话文本颜色
	void	SetSpeakColor(DWORD color);
	// 设置公会/队伍文本内容
	void	SetPartyText(const string& text);
	// 设置公会/队伍文本颜色
	void	SetPartyColor(DWORD color);
	// 设置公会/队伍标志图片
	void	SetPartyLogo(IUiImage* image);
	// 设置名字文本内容
	void	SetNameText(const string& text);
	// 设置名字文本颜色
	void	SetNameColor(DWORD color);
	// 设置消息文本内容
	void	SetMessageText(const string& text);
	// 设置消息文本颜色
	void	SetMessageColor(DWORD color);
	// 设置血条值(0.0-1.0)
	void    SetBloodValue(float value);
	// 设置摆摊信息文本
	void    SetStallText(const string& text);
	// 设置称号文本内容
	void    SetTitleText(const string& text);
	// 设置称号文本颜色
	void	SetTitleColor(DWORD color);
	// 设置称号标志图片
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
	//IUiImage*		m_partyLogo;      // 公会/队伍标志图片
	bool            m_bshow;           // 是否显示HUD
	int				m_speakDuration;   // 说话持续时间
	int				m_speakTimeStamp;  // 说话时间戳
	float			m_bloodValue;      // 血条值
	//IUiImage*		m_bkimage;        // 背景图片
	CUiProgressBar*	m_pBloodBar;      // 血条进度条控件
	CUiEditBox*		m_pSpeakUI;       // 说话文本框控件
	CUiEditBox*		m_pNameUI;        // 名字文本框控件
	CUiEditBox*		m_pPartyUI;       // 公会/队伍文本框控件
	EditBox3D       m_name;           // 3D名字文本框
	EditBox3D       m_title;          // 3D称号文本框
	EditBox3D       m_pPartyUi;       // 3D公会/队伍文本框
	CUiButton*      m_pStall;         // 摆摊按钮控件
	DWORD			m_speakColor;     // 说话文字颜色
	string          m_sname;          // 名字字符串
	string          m_szStall;        // 摆摊信息字符串
	RtgVertex3      m_pos;            // HUD位置坐标
public:
	static bool            ename;      // 是否显示名字的静态标志
	static bool            espeak;     // 是否显示说话的静态标志

	BYTE            m_cShowItem;       // 当前HUD显示项目的标志位

public:
	static BYTE			s_cShowItem;  // 全局HUD显示项目的静态标志位

};

#endif //__HUD_H__
