
#include "core/rt2_core.h"
#include "audio/rta_back_music.h"

#define USE_SDL_AUDIO   1

#if USE_SDL_AUDIO
#   include "sdl.h"
#   include "sdl_mixer.h"
#   ifndef _DEBUG
#       ifdef _MT
#           ifdef _DLL
//              multithread DLL
#               pragma comment(lib, "sdl_MD.lib")
#               pragma comment(lib, "sdlmixer_MD.lib")
#               pragma comment(lib, "ogg_static_MD.lib")
#               pragma comment(lib, "vorbis_static_MD.lib")
#               pragma comment(lib, "vorbisfile_static_MD.lib")
#           else
//              multithread
#               pragma comment(lib, "sdl_MT.lib")
#               pragma comment(lib, "sdlmixer_MT.lib")
#               pragma comment(lib, "ogg_static_MT.lib")
#               pragma comment(lib, "vorbis_static_MT.lib")
#               pragma comment(lib, "vorbisfile_static_MT.lib")
#           endif
#       endif
#   else
#       ifdef _MT
#           ifdef _DLL
//              Debug multithread DLL
#               pragma comment(lib, "sdl_MDD.lib")
#               pragma comment(lib, "sdlmixer_MDD.lib")
#               pragma comment(lib, "ogg_static_MDD.lib")
#               pragma comment(lib, "vorbis_static_MDD.lib")
#               pragma comment(lib, "vorbisfile_static_MDD.lib")
#           else
//              Debug multithread
#               pragma comment(lib, "sdl_MTD.lib")
#               pragma comment(lib, "sdlmixer_MTD.lib")
#               pragma comment(lib, "ogg_static_MTD.lib")
#               pragma comment(lib, "vorbis_static_MTD.lib")
#               pragma comment(lib, "vorbisfile_static_MTD.lib")
#           endif
#       endif
#   endif
#endif


#if USE_SDL_AUDIO
static int rta_core_seek(SDL_RWops *context, int offset, int whence)
{
    if (context && context->hidden.unknown.data1!=NULL)
    {
        RtArchive::SeekMode eMode;
        switch (whence)
        {
        case SEEK_SET:  eMode = RtArchive::SeekBegin;   break;
        case SEEK_END:  eMode = RtArchive::SeekEnd;     break;
        case SEEK_CUR:  eMode = RtArchive::SeekCurrent; break;
        }
        ((RtArchive*)context->hidden.unknown.data1)->Seek(offset, eMode);
        return ((RtArchive*)context->hidden.unknown.data1)->Tell();
    }
    return 0;
}
static int rta_core_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
    if (context && context->hidden.unknown.data1!=NULL)
    {
        int nRead = ((RtArchive*)context->hidden.unknown.data1)->Serialize((void*)ptr, size*maxnum)/size;
        if (nRead==0)
        {
            // TODO: Error
        }
        return (nRead);
    }
    return 0;
}
static int rta_core_write(SDL_RWops *context, const void *ptr, int size, int num)
{
    if (context && context->hidden.unknown.data1!=NULL)
    {
        int nWrote = ((RtArchive*)context->hidden.unknown.data1)->Serialize((void*)ptr, size*num)/size;
        if (nWrote==0)
        {
            // D    WRYUIPIUVTGT6 G6TODO: Error
        }
        return (nWrote);
    }
    return 0;
}
static int rta_core_close(SDL_RWops *context)
{
    if ( context )
    {
        if (context->hidden.unknown.data1!=NULL)
        {
            RtArchive* pArchive = (RtArchive*)context->hidden.unknown.data1;
            DEL_ONE(pArchive);
        }
        SDL_FreeRW(context);
    }
    return(0);
}
SDL_RWops *rta_RWFromCore(const char* szFilename)
{
    SDL_RWops *rwops = SDL_AllocRW();
    if ( rwops != NULL ) {
        rwops->seek = rta_core_seek;
        rwops->read = rta_core_read;
        rwops->write = rta_core_write;
        rwops->close = rta_core_close;
        rwops->hidden.unknown.data1 = RtCoreFile().CreateFileReader(szFilename, 0);
        if (rwops->hidden.unknown.data1==NULL)
        {
            RtCoreLog().Error("RtaBackMusic: 找不到音乐文件 [%s]\n", szFilename);
        }
    }
    return(rwops);
}
#endif

bool RtaBackMusic::m_bfinished = false;

RtaBackMusic::RtaBackMusic()
{
    m_bLoop     = false;
    m_bQuiet    = false;
    m_bPlaying  = false;
    m_bInit	    = false;
    m_pMusic    = NULL;
	
}

RtaBackMusic::~RtaBackMusic()
{
#if USE_SDL_AUDIO
    Cleanup();
#endif
}

void RtaBackMusic::Cleanup()
{
#if USE_SDL_AUDIO
    Stop();
    if (m_pMusic)
    {
        Mix_FreeMusic((Mix_Music*)m_pMusic);
        m_bPlaying = false;
        m_pMusic = NULL;
    }
    if (m_bInit)
    {
        Mix_CloseAudio();
        m_bInit = false;
    }
    SDL_Quit();
#endif
}

bool RtaBackMusic::Initialize(int iMaxVolume)
{
#if USE_SDL_AUDIO
    Uint16 iAudioFormat = AUDIO_S16;
    int iAudioRate      = 44100;
    int iAudioChannels  = 2;
    int iAudioBuffers   = 4096;
    if (iMaxVolume>MIX_MAX_VOLUME) iMaxVolume = MIX_MAX_VOLUME;

    SDL_Init(SDL_INIT_AUDIO|SDL_INIT_NOPARACHUTE); // SDL_INIT_NOPARACHUTE
    if (Mix_OpenAudio(iAudioRate, iAudioFormat, iAudioChannels, iAudioBuffers) < 0) 
    {
        RtCoreLog().Error("音乐错误: 不能初始化音乐.\n");
        m_bInit = false;
        return false;
    }else
    {
        Mix_QuerySpec(&iAudioRate, &iAudioFormat, &iAudioChannels);
        Mix_SetMusicCMD(getenv("MUSIC_CMD"));
    }
    Mix_VolumeMusic(iMaxVolume);
    m_bInit = true;
#endif
    return true;
}

// 设置目录，所有的音乐文件都要在这个目录里面，(eg. "audio/")
void RtaBackMusic::SetMusicPath(const char* szPathname)
{
    m_strPathname = szPathname;
}

bool RtaBackMusic::Play(const char* szFileName, bool bLoop)
{
    if (!m_bInit) return false;
    if (m_bQuiet) return true;
    if (szFileName==0 || szFileName[0]==0) return true;
#if USE_SDL_AUDIO
    if (m_pMusic)
    {
        Mix_FreeMusic((Mix_Music*)m_pMusic);
        m_bPlaying = false;
        m_pMusic = NULL;
    }
    m_bLoop = bLoop;
    m_strFilename = m_strPathname * szFileName;
    //m_pMusic = Mix_LoadMUS(m_strFilename);
    m_pMusic = Mix_LoadMUS_RW(rta_RWFromCore(m_strFilename));
    if (m_pMusic==NULL)
    {
        RtCoreLog().Error("音乐错误: 不能载入音乐 [%s].\n", m_strFilename);
        return false;
    }
    if (m_bLoop)
    {
        Mix_FadeInMusic((Mix_Music*)m_pMusic, -1, 2000);
    }else
    {
        Mix_FadeInMusic((Mix_Music*)m_pMusic, 0, 2000);
    }
	Mix_HookMusicFinished(&RtaBackMusic::Finish);
    m_bPlaying = true;
#endif
    return true;
}

bool RtaBackMusic::Stop()
{
    if (!m_bInit) return false;
#if USE_SDL_AUDIO
    if (m_bPlaying)
    {
        Mix_FadeOutMusic(2000);
    }
    m_bPlaying = false;
#endif
    return true;
}

void RtaBackMusic::SetVolume(float fVolume)
{
    if (!m_bInit) return;
#if USE_SDL_AUDIO
    if (fVolume<0.f) fVolume = 0.f;
    else if (fVolume>1.f) fVolume = 1.f;
    Mix_VolumeMusic(fVolume*MIX_MAX_VOLUME);
#endif
}

float RtaBackMusic::GetVolume()
{
    if (!m_bInit) return 0.f;
#if USE_SDL_AUDIO
    return (float)Mix_VolumeMusic(-1)/(float)MIX_MAX_VOLUME;
#endif
}

void RtaBackMusic::Quiet(bool bQuiet)
{
    if (!m_bInit) return;
    if (m_bQuiet==bQuiet) return;
#if USE_SDL_AUDIO
    m_bQuiet = bQuiet;
    if(m_bQuiet)
    {
        Stop();
    }else
    {
        Play(m_strFilename, m_bLoop);
    }
#endif
}
void RtaBackMusic::Finish()
{
    m_bfinished = !m_bfinished;
}

bool RtaBackMusic::IsFinished()
{
	return m_bfinished;
}

bool RtaBackMusic::IsPlaying()
{
	return !!Mix_PlayingMusic();
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
