
namespace rt_graph {

/*----------------------------------------------------------------------------
-   class RtgConfig.
----------------------------------------------------------------------------*/

class GRAPH_API RtgConfig
{
public:

    RtgConfig(const char* szConfigFile);
    ~RtgConfig();

public:

    DWORD		nDynamicVertexStreamCount;
    DWORD*  	nDynamicVertexStreamSize;	

    DWORD		nStaticVertexStreamCount;
    DWORD*	    nStaticVertexStreamSize;	

    DWORD		nDynamicIndexStreamCount;
    DWORD*	    nDynamicIndexStreamSize;

    DWORD		nStaticIndexStreamCount;
    DWORD*	    nStaticIndexStreamSize;

    DWORD		nDynamicVertexEntryCount;
    DWORD		nStaticVertexEntryCount;

    DWORD		nDynamicIndexEntryCount;
    DWORD		nStaticIndexEntryCount;

    DWORD		nTextureEntryCount;
    DWORD		nTextureBufferSize;
};


} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
