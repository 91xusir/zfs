/**
* @file rtc2_engine_sys.h.
*/

namespace rt2_core {

class RtEngineSystem : public RtObject
{
public:
	RT_DECLARE_DYNAMIC(RtEngineSystem, RtObject, NULL, "")
	RtEngineSystem() {}
	virtual ~RtEngineSystem() {}

	virtual bool Init(RtIni &cfg) = 0;
	virtual void Tick(DWORD delta) = 0;
	virtual void Exit() = 0;
	virtual bool ExecCmd(const char *cmd, const char *param) = 0;
	virtual const char* GetName() = 0;
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
