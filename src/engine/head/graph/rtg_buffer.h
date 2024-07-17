
namespace rt_graph {

struct _RtgBufferItem;
typedef _RtgBufferItem RtgBufferItem;

class RtgBuffer
{
public:
	RtgBuffer();
	~RtgBuffer();
	RTGMemoryType GetMemoryType(void) { return m_eMType; }
	RTGBufferType GetBufferType(void) { return m_eVType; }

	int  SetVertexFormat(int vsize, int normal, int color, int numtex, int *texcoords = NULL);
	void SetVertexFormat(RTGVertexFormat fmt);
	void SetVertexFormat(RtgVertexSpec fmt);

	void SetResourceType(RTGMemoryType mtype, RTGBufferType vtype);
	void *AllocateElements(RtgDevice* pDevice, int esize);
	void *AllocateBuffer(RtgDevice* pDevice, int size);
	int  ReleaseBuffer(RtgDevice* pDevice);
	void *LockBuffer(RtgDevice* pDevice);
	int  UnLockBuffer(RtgDevice* pDevice);
	RtgBufferItem *GetBuffer(void) { return m_pBuffer; }

	int SetVertex(int index, RtgVertex3 &vertex);
	int SetVertex(int index, float x, float y, float z);
	int GetVertex(int index, RtgVertex3 &vertex);
	RtgVertex3 *GetVertex(int index);

	int SetNormal(int index, RtgVertex3 &normal);
	int SetNormal(int index, float x, float y, float z);
	int GetNormal(int index, RtgVertex3 &normal);
	RtgVertex3 *GetNormal(int index);

	int SetColor(int index, int color);
	int SetColor(int index, float r, float g, float b, float a=1.0f);
	void *GetColor(int index);

	int SetTexCoord(int index, float *coord);
	int SetTexCoord(int index, float x, float y, int channel = -1);
	int SetTexCoord(int index, float x, float y, float z, int channel = -1);
	int GetTexCoord(int index, float &s, float &t, int channel = 0);
	void *GetTexCoord(int index);

	int SetIndex(int index, int value);
	int GetIndex(int index, int &value);

	RtgVertexSpec GetVertexFormat(void) { return m_VertexSpec; }

protected:
    RtgDevice*      m_pDevice;
	RTGMemoryType   m_eMType;
	RTGBufferType   m_eVType;
	RtgBufferItem*  m_pBuffer;
	unsigned char   *m_pLockedBuffer;
	RtgVertexSpec   m_VertexSpec;
	int             m_iNumElements;
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
