
namespace rt_graph {

class RtgDisplayModeEnum
{
public:
    struct SBitCount
    {
        DWORD dwBitCount;
        std::list<DWORD>    listRefreshRate;
    };
    struct SScreenSize
    {
        DWORD dwWidth;
        DWORD dwHeight;
        std::list<SBitCount>    listBitCount;
    };
    struct SDevice
    {
        GUID  guid;
        //char  strDescription[256];
        //char  strDriverName[64];
		CM_MEMDEF(strDescription, 256)
		CM_MEMDEF(strDriverName, 64)
        std::list<SScreenSize>  listScreenSize;

		SDevice()
		{
			CM_MEMPROTECTOR(strDescription, 256)
			CM_MEMPROTECTOR(strDriverName, 64)
		}
		SDevice(const SDevice &object)
		{
			memcpy(this, &object, sizeof(SDevice));
			CM_MEMPROTECTOR(strDescription, 256)
			CM_MEMPROTECTOR(strDriverName, 64)
		}
		~SDevice()
		{
			CM_MEMUNPROTECTOR(strDescription)
			CM_MEMUNPROTECTOR(strDriverName)
		}
    };

public:
    bool EnumAll();

protected:
    bool EnumDevices();
    bool EnumModes( SDevice* pDevice );

public:
    std::list<SDevice>  m_listDevices;

};


} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
