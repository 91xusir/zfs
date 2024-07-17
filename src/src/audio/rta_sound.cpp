
#include "graph/rt_graph.h"
#include "audio/rt_audio.h"

RtaSound::RtaSound()
{
    m_iUseAudioCnt      = 0;
    m_dwUseAudioMem     = 0;
    SetupSound(50, 1024*1024);
    //SetupSound(50, 4*1024*1024);
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
