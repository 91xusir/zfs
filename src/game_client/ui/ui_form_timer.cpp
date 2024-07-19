//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#include "gc_include.h"
#include "net_command_castlewar.h"
#include "game_string_assistant.h"
#include "ui_form_timer.h"

UIFormTimer::UIFormTimer()
{
	guard;

	m_pLabelTimer = RTW_WIDGET_T(RtwLabel, "lbbattle");

	m_total_time = 0;

	Hide();

	unguard;
}

void UIFormTimer::Show(string feod_name, DWORD total_time)
{
	guard;

	DWORD hour	 = 0;
	DWORD minute = 0;
	DWORD second = 0;
	string dim	 = ":";		
	string txt	 = "";
	
	int cnt = 0;
	for(cnt = 0; cnt < FEOD_COUNT; cnt++)
	{
		if(feod_name == FeodNameInDB[cnt])
		{
			txt = CastleWarAreaName[cnt];

			break;
		}
	}

	if(cnt >= FEOD_COUNT)
	{
		return;
	}

	txt += CWAR_START_TIME;

	if(total_time > 300)
	{
		total_time = 300;
	}

	minute = total_time / 60;

	second = total_time % 60;

	txt += std::string(rtFormatNumber(minute));
	txt += dim;
	txt += std::string(rtFormatNumber(second));

	m_pLabelTimer->SetText(txt);

	m_pLabelTimer->SetTextColor(0xA0FFFF00);

	m_pLabelTimer->Show();

	unguard;
}

void UIFormTimer::Hide()
{
	m_pLabelTimer->Hide();
}