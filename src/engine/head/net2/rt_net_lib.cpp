#   ifdef _DEBUG
#       ifdef _MT
#           ifdef _DLL
#               pragma comment(lib, "rtnet_lib_MDD.lib")
#           else
#               pragma comment(lib, "rtnet_lib_MTD.lib")
#           endif
#       else
#               pragma comment(lib, "rtnet_lib_MLD.lib")
#       endif
#   else
#       ifdef _MT
#           ifdef _DLL
#               pragma comment(lib, "rtnet_lib_MD.lib")
#           else
#               pragma comment(lib, "rtnet_lib_MT.lib")
#           endif
#       else
#               pragma comment(lib, "rtnet_lib_ML.lib")
#       endif
#   endif
