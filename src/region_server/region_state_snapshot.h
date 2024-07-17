#ifndef _INC_REGION_STATE_SNAPSHOT_H_
#define _INC_REGION_STATE_SNAPSHOT_H_

class CRegionStateSnapshot
{
public:
	void BuildSpawnMap(const char* filename);

	void BuildTerrainMap(const char* filename, int mask = 0xff);

private:
	void SaveBitmap(const char* filename, const unsigned char* buf, int width, int height);
};

extern CRegionStateSnapshot g_stateSnapshot;

#endif // _INC_REGION_STATE_SNAPSHOT_H_
