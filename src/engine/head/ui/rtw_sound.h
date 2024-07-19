//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_SOUND_H
#define _RTUI_SOUND_H

#include "audio/rt_audio.h"



namespace ui {


	class IUiSound
	{
	public:
		IUiSound() { }
		virtual ~IUiSound() { }

	public:
		virtual void PlaySound(const std::string& strFilename) = 0;
	};

	class CUiSoundImpl_Rt : public IUiSound
	{
	public:
		CUiSoundImpl_Rt() { }
		virtual ~CUiSoundImpl_Rt() { }

	public:
		virtual void PlaySound(const std::string& strFilename)
		{
			RtgVertex3 vPos;
			g_pSoundMgr->PlayOnce(strFilename.c_str(), false, 0, vPos);
		}
	}; 

} // namespace ui





#endif // _RTUI_SOUND_H
