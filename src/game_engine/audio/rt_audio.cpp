
#include "core/rt2_core.h"
#include "graph/rt_graph.h"
#include "audio/rt_audio.h"
#include "dx8\dsutil.h"
#include "rta_sound_dx8.h"

namespace rt_audio {

RtaSoundManager* g_pSoundMgr = NULL;
RtaBackMusic* g_pBackMusic = NULL;

bool rtSoundInit(const char* szPackage, HWND hWnd)
{
    g_pSoundMgr = RT_NEW RtaSoundManager;
    g_pSoundMgr->Initialize(hWnd);
    
	return true;
   
}
bool rtMusicInit()
{
	g_pBackMusic = RT_NEW RtaBackMusic;
	g_pBackMusic->Initialize();

	return true;

}

void rtAudioPreExit()
{
    if (g_pSoundMgr)
    {
        g_pSoundMgr->Cleanup();
    }
    if (g_pBackMusic)
    {
        g_pBackMusic->Cleanup();
    }
}

void rtAudioExit()
{
    DEL_ONE(g_pBackMusic);
    DEL_ONE(g_pSoundMgr);
}

} // namespace rt_audio

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
