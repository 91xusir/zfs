
#ifndef AUDIO_SOURCE
#   ifdef _DEBUG
#       ifdef _MT
#           ifdef _DLL
#               pragma comment(lib, "rtaudio_lib_MDD.lib")
#           else
#               pragma comment(lib, "rtaudio_lib_MTD.lib")
#           endif
#       else
#               pragma comment(lib, "rtlib_core_MLD.lib")
#       endif
#   else
#       ifdef _MT
#           ifdef _DLL
#               pragma comment(lib, "rtaudio_lib_MD.lib")
#           else
#               pragma comment(lib, "rtaudio_lib_MT.lib")
#           endif
#       else
#               pragma comment(lib, "rtlib_core_ML.lib")
#       endif
#   endif
#endif

#   pragma comment(lib, "d3dx9.lib")
#   pragma comment(lib, "d3d9.lib")
#   pragma comment(lib, "d3dxof.lib")
#   pragma comment(lib, "dxguid.lib")
#   pragma comment(lib, "ddraw.lib")
#   pragma comment(lib, "dinput8.lib")
#   pragma comment(lib, "dsound.lib")
#   pragma comment(lib, "dxerr9.lib")

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
