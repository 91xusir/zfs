
#ifndef _RT_ENGINE_AUDIO_H_
#define _RT_ENGINE_AUDIO_H_

/*----------------------------------------------------------------------------
-   API definition.
----------------------------------------------------------------------------*/

#ifdef RT_AUDIO_DLL  // ÊÇ·ñÉú³ÉDLL
#   ifdef RT_GRAPH_EXPORTS
#       define AUDIO_API DLL_EXPORT
#   else
#       define AUDIO_API DLL_IMPORT
#   endif
#else
#   define AUDIO_API
#endif

namespace rt_audio {

/*----------------------------------------------------------------------------
-   Classes.
----------------------------------------------------------------------------*/
class RtaSoundNode;
class RtaSoundManager;
class RtaSound;
class RtaBackMusic;

} // namespace rt_audio

/*----------------------------------------------------------------------------
-   Include.
----------------------------------------------------------------------------*/

#include "rta_sound.h"
#include "rta_sound_mgr.h"
#include "rta_back_music.h"

/*----------------------------------------------------------------------------
-   Global function.
----------------------------------------------------------------------------*/
namespace rt_audio {

AUDIO_API bool rtSoundInit(const char* szPackage, HWND hWnd);
AUDIO_API bool rtMusicInit();
AUDIO_API void rtAudioPreExit();
AUDIO_API void rtAudioExit();

/*----------------------------------------------------------------------------
-   Global variables.
----------------------------------------------------------------------------*/

AUDIO_API extern RtaSoundManager* g_pSoundMgr;
AUDIO_API extern RtaBackMusic* g_pBackMusic;

} // namespace rt_audio
using namespace rt_audio;

#endif _RT_ENGINE_AUDIO_H_

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
