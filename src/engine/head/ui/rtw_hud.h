//********************************************************************
//	created:	2010.05.28 16:00
//	filename: 	rtw_hud.cpp
//	author:		ldr123
//	purpose:	hud
//  other:		局部重构添加注释
//********************************************************************

#ifndef _RTUI_HUD_H
#define _RTUI_HUD_H

namespace ui 
{
	class RtwHUD : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwHUD, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;

		typedef RtwRefPtr<RtwHUD> Ptr;
	public:
		RtwHUD();
		virtual ~RtwHUD();
	public:
		struct EditBox3D
		{
			std::string	text;	//头顶文字
			SRect3D		rect;	//显示矩形
			int			width, height;
			RtwPixel	color;

			EditBox3D():color(0xffFFFFFF), text(""), width(0), height(0)
			{
				rect.z = 0;
			}

			~EditBox3D(){}

			void SetText(const string&  t)
			{
				text = t;
			}

			void SetPos(RtgVertex3 pos)
			{
				rect.s.left		= pos.x - (width>>1);
				rect.s.top		= pos.y - 24;
				rect.z			= pos.z;
				rect.s.right	= pos.x + (width>>1);
				rect.s.bottom	= rect.s.top + height;
			}

			void SetSize(int x,int y)
			{
				width	= x;
				height	= y;        
			}

			void Render(RtgDevice& inDevice)
			{
				inDevice.SetRenderState(RTGRS_LIGHTING,FALSE);
				inDevice.SetFogEnable(FALSE);
				DWORD s_dwFSAA = inDevice.GetRenderState(RTGRS_FULLSCRENE_ANTIALIASING);
				inDevice.SetRenderState( RTGRS_FULLSCRENE_ANTIALIASING,    FALSE );
				g_workspace.getFontManager()->DrawString3D(text,rect,color,true,SUiTextAlignment(alignCenter,alignCenter));
				g_workspace.getFontManager()->Render();
				inDevice.SetRenderState( RTGRS_FULLSCRENE_ANTIALIASING, s_dwFSAA);
				inDevice.SetRenderState(RTGRS_LIGHTING,TRUE);
				inDevice.SetFogEnable(TRUE);
			}

			void Render();
		};
	public:
		enum EShowItem
		{
			siSpeak		= 0x01,			// 说话
			siName		= 0x02,			// 名字
			siGuild		= 0x04,			// 帮派
			siNick		= 0x08,			// 帮派昵称(只有帮派显示才会显示)
			esiFaction	= 0x10,			// 门派
			esiHPBar	= 0x20,			// HP条		
			siMsg		= 0x40,			// 信息
			esiStall	= 0x80,			// 摆摊店名
			esiLogo		= 0x100,		// 帮派LOGO
			esiDynInfo  = 0x200,		// 伤害值
			siPetAbility  = 0x400,		// 宠物锻体等级
			siPetFuseLev  = 0x800,		// 宠物炼化等级
			siPetMaster	  = 0x1000,		// 宠物主人
		};
	public:
		bool	Init();
		void	SetPosition(RtgVertex3 pos);
		void	Show();
		void	Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
		void	OnHudFlagChanged(long newFlag);
		void    Show(bool flag) {m_bshow = flag;}
		void	ShowNameSeparate(bool showName){m_bShowName = showName;}
		void	Hide() {m_bshow = false;}
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);
	public:
		void	SetSpeakText(const string& text);
		void	SetSpeakTime();
		void	SetSpeakColor(DWORD color);
		void	SetGuildText(const string& text); //change from SetPartyText by tooth.shi
		void	SetPartyColor(DWORD color);
		void	SetPartyLogo(RtwImage* image);
		void	SetNameText(const string& text);
		void	SetNameColor(DWORD color);
		void	SetMessageText(const string& text);
		void	SetMessageColor(DWORD color);
		void    SetBloodValue(float value);
		void    SetStallText(const string& text);
		void    SetTitleText(const string& text);
		void	SetTitleColor(DWORD color);
		void	SetTitleLogo(RtwImage* image);
		void    ShowName(bool flag)     { if (flag&&ename) m_cShowItem|=siName;    else m_cShowItem&=(~siName);   }
		void    ShowSpeak(bool flag)    { if (flag) m_pSpeakUI->Show();     else m_pSpeakUI->Hide(); }
		void    ShowParty(bool flag)    { if (flag) m_cShowItem|=siGuild;   else m_cShowItem&=(~siGuild);   }
		void    ShowTitle(bool flag)    { if (flag) m_cShowItem|=siNick;    else m_cShowItem&=(~siNick);  }
		void    ShowFaction(bool flag)  { if (flag) m_cShowItem|=esiFaction; else m_cShowItem&=(~esiFaction); }
		void    ShowHPBar(bool flag);    /*{ if (flag) m_cShowItem|=esiHPBar;   else m_cShowItem&=(~esiHPBar);   }*/
		void    ShowMsg(bool flag)      { if (flag) m_cShowItem|=siMsg;     else m_cShowItem&=(~siMsg);	  }
		void    ShowStall(bool flag);    /*{ if (flag) m_cShowItem|=esiStall;   else m_cShowItem&=(~esiStall);   }*/
		void    ShowPetAbility(bool flag)      { if (flag) m_cShowItem|=siPetAbility;     else m_cShowItem&=(~siPetAbility);} 
		void    ShowPetFuseLev(bool flag)      { if (flag) m_cShowItem|=siPetFuseLev;     else m_cShowItem&=(~siPetFuseLev);}
		void    ShowPetMaster(bool flag)      { if (flag) m_cShowItem|=siPetMaster;     else m_cShowItem&=(~siPetMaster);}
		void    ShowLogo(bool flag){if (flag) m_partylogo->Show();     else m_partylogo->Hide();}
		void    SetShowFlag(BYTE flag)  { m_cShowItem = flag; }
		long    GetShowFlag()           { return m_cShowItem; }
		void    ApplyFlag()             { m_cShowItem = s_cShowItem; }

		bool    IsShowSpeak()           { return (m_cShowItem & siSpeak	& s_cShowItem)!=0;	}
		bool    IsShowName()            { return (m_cShowItem & siName	& s_cShowItem)!=0;	}
		bool    IsShowParty()           { return (m_cShowItem & siGuild	& s_cShowItem)!=0;	}
		bool    IsShowTitle()           { return (m_cShowItem & siNick  & s_cShowItem)!=0;	}
		bool    IsShowFaction()         { return (m_cShowItem & esiFaction	& s_cShowItem)!=0;	}
		bool    IsShowHPBar()           { return (m_cShowItem & esiHPBar	& s_cShowItem)!=0;	}
		bool    IsShowMsg()             { return (m_cShowItem & siMsg		& s_cShowItem)!=0;	}
		bool    IsShowStall()           { return (m_cShowItem & esiStall    & s_cShowItem)!=0;  }
		bool    IsShowLogo()            { return (m_cShowItem & esiLogo     & s_cShowItem)!=0;  }
		bool    IsShowPetAbility()            { return (m_cShowItem & siPetAbility     & s_cShowItem)!=0;  }
		bool    IsShowPetFuseLev()            { return (m_cShowItem & siPetFuseLev     & s_cShowItem)!=0;  }
		bool    IsShowPetMaster()            { return (m_cShowItem & siPetMaster     & s_cShowItem)!=0;  }
		RtwTextBox* GetSpeakHud()		{ return m_pSpeakUI; }
		void    SetNameShow(bool m_name)           { ename=m_name; }
		void SetGuildLogo(RtwImage* image)	   {	SetPartyLogo(image);}
		void SetAgnameText(const string& text) {	SetTitleText(text);	}

		void SetPetFlag(bool flag,const string& szPetAbility ,DWORD dwColorAbility,
			const string& szPetFuseLev,DWORD dwColorFuseLev ,const string& szPetMaster,DWORD dwColorMaster )
		{ 
			ShowPetAbility(flag);
			ShowPetFuseLev(flag);
			ShowPetMaster(flag);
			if (flag)
			{
				if (szPetAbility.empty())
					ShowPetAbility(false);
				else
					m_EditBoxPetAbility.SetText(szPetAbility);

				if (szPetFuseLev.empty())
					ShowPetFuseLev(false);
				else
					m_EditBoxPetFuseLev.SetText(szPetFuseLev);

				if (szPetMaster.empty())
					ShowPetMaster(false);
				else
					m_EditBoxPetMaster.SetText(szPetMaster);

				m_EditBoxPetAbility.color = dwColorAbility;
				m_EditBoxPetFuseLev.color = dwColorFuseLev;
				m_EditBoxPetMaster.color = dwColorMaster;
			}
		}
	private:
		bool            m_pstallshow;
		bool            m_bshow;
		bool			m_bShowName;
		int				m_speakDuration;
		int				m_speakTimeStamp;	
		float			m_bloodValue;	
		RtwProgressBar*	m_pBloodBar;
		RtwTextBox*		m_pSpeakUI;
		RtwTextBox*		m_pNameUI;
		RtwTextBox*		m_pPartyUI;
		EditBox3D       m_name;
		EditBox3D       m_title;
		EditBox3D       m_pPartyUi;
		EditBox3D       m_EditBoxPetAbility;
		EditBox3D       m_EditBoxPetFuseLev;
		EditBox3D       m_EditBoxPetMaster;
		RtwButton*      m_partylogo;
		RtwButton*      m_pStall;
		DWORD			m_speakColor;
		string          m_sname;
		string          m_szStall;
		RtgVertex3      m_pos;
	public:
		static bool            ename;
		static bool            espeak;
		long            m_cShowItem;
	public:
		static long		s_cShowItem;
	};
} 
#endif 