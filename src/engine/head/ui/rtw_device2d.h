//////////////////////////////////////////////////////////////////////////
// 
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_RENDERDEVICE_H
#define _RTUI_RENDERDEVICE_H

#include "core/rt2_core.h"
#include "graph/rt_graph.h"

class CRT_ActorInstance;
namespace ui {

	//rtui_renderdevice.h
	class IUiRenderDevice
	{
	public:
		IUiRenderDevice() { }
		virtual ~IUiRenderDevice() { }

	public:
		//virtual bool Init() = 0;
		virtual void OnInvalidateDevice() = 0;
		virtual void OnRestoreDevice() = 0;

		virtual Real getZOrder() = 0;
		virtual RtwRect getViewportRect() = 0;

	public:
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;
		virtual void DrawImage(RtwImage* pImage, const RtwRect& rc, const SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear), const RtwRect* pClipRect = NULL) = 0;
		virtual void DrawBorderImage(RtwImage* pImage, const RtwRect& rc, const SMargin& margin, const SUiTextAlignment align = SUiTextAlignment(alignNear,alignNear),const RtwRect* pClipRect = NULL) = 0;
		virtual void DrawComplexImage(RtwComplexImage* image, const RtwRect& rect) = 0;
		virtual void DrawRectangle(const RtwRect& rect, const RtwPixel& color) = 0;
		virtual void DrawRectangleBorder(const RtwRect& rect, const RtwPixel& color,const RtwRect* pClipRect = NULL) = 0;
		virtual void DrawLine(const SPoint& startPoint, const SPoint& endPoint, const RtwPixel& color) = 0;
		virtual void DrawTriangle(const SPoint& p1, const SPoint& p2, const SPoint& p3, const RtwPixel& color) = 0;
		virtual void SetTextureRenderTarget( int nTextureSurface , bool bClearUp , long nBackColor )  {}
		virtual void RestorRenderTarget( ) {}
		virtual RtgDevice* GetDevice() { return NULL; }
		virtual void BeginRenderViewPort(RtgCameraD3DView *pCam, DWORD dwX,DWORD dwY,DWORD dwWeight,DWORD dwHeight,float fMin,float fMax, bool bRefreshCam = true){}
		virtual void EndRenderViewPort(bool bRefreshCam = true){}
		virtual void SetMatrixFromCamera(){}
		virtual void RenderActorInstance(CRT_ActorInstance *pActor,RTGRenderMask eMask){}
	};
	//end rtui_renderdevice.h

	//rtui_render_image.h
	class CUiImageImpl_Rt : public RtwImage
	{
		RT_DECLARE_DYNCREATE(CUiImageImpl_Rt, RtwImage, 0, "ui")

			friend class CUiImageFactoryImpl_Rt;

	protected:
		CUiImageImpl_Rt(RtgDevice* pRtgDevice);
		virtual ~CUiImageImpl_Rt();

	public:
		// base class inherit
		virtual void OnBeforeDestroy();

		virtual bool LoadFromFile(const std::string& filename);
		virtual bool LoadFromMemory(const void* data, int length);
		virtual bool LoadFromMemory(int width, int height, int length, const void* data);
		virtual bool LoadFromRtArchive(RtArchive *pArc,int length,bool bflip = false);
		virtual void SetTransparency(Real trans);
		virtual void SetBlend(bool bBlend);
		virtual void SetSourceRect(RtwRect& vrRect)//add by fox
		{
			m_Source = vrRect;
		}
		RtgTextItem* GetTexItem(){return m_Image.Shaders[0].GetTexture();}
		RtgTextItem* GetTexture(){return m_Image.Shaders[0].GetTexture();}

		inline int GetWidth()
		{
			return m_Image.Shaders[0].GetTexture()->width;
		}

		inline int GetHeight()
		{
			return m_Image.Shaders[0].GetTexture()->height;
		}
	public:
		RtgShader       m_Image;
		RtgDevice*      m_pRtgDevice;
	};
	//end rtui_render_image.h

	//rtui_render_imagefactory.h
	class CUiImageFactoryImpl_Rt : public IUiImageFactory
	{
	public:
		CUiImageFactoryImpl_Rt(RtgDevice* pRtgDevice);
		~CUiImageFactoryImpl_Rt();

	public:
		virtual RtwImage* createImage(const std::string& filename);
		virtual RtwImage* RealCreateImage();

	private:
		RtgDevice*      m_pRtgDevice;
	};
	//end rtui_render_imagefactory.h

	//rtui_render_renderdevice.h
	class CUiRenderDeviceImpl_Rt : public IUiRenderDevice
	{
	public:
		CUiRenderDeviceImpl_Rt();
		~CUiRenderDeviceImpl_Rt();

	public:
		struct SImageVertex
		{
			RtgVertex4  coord;
			DWORD       diffuse;
			RtgVertex2  texcoord;

			SImageVertex(Real x, Real y, Real z, DWORD color, Real tx, Real ty)
				:coord(x, y, z, 1)
				,diffuse(color)
				,texcoord(tx, ty)
			{
			}
		};
		struct SShapeVertex
		{
			RtgVertex4  coord;
			DWORD       diffuse;

			SShapeVertex(Real x, Real y, Real z, DWORD color)
				:coord(x, y, z, 1)
				,diffuse(color)
			{
			}
		};

	public:
		virtual bool Init(RtgDevice* pRtgDevice);
		virtual void OnInvalidateDevice();
		virtual void OnRestoreDevice();

		virtual Real getZOrder() { return m_ZOrder; }
		virtual RtwRect getViewportRect();

	public:
		virtual void BeginRender();
		virtual void EndRender();
		virtual void DrawImage(RtwImage* pImage, const RtwRect& rc, const SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear), const RtwRect* pClipRect = NULL);
		virtual void DrawBorderImage(RtwImage* pImage, const RtwRect& rc, const SMargin& margin, const SUiTextAlignment align = SUiTextAlignment(alignNear,alignNear),const RtwRect* pClipRect = NULL);
		virtual void DrawComplexImage(RtwComplexImage* image, const RtwRect& rect);
		virtual void DrawRectangle(const RtwRect& rect, const RtwPixel& color);
		virtual void DrawRectangleBorder(const RtwRect& rect, const RtwPixel& color,const RtwRect* pClipRect = NULL);
		virtual void DrawLine(const SPoint& startPoint, const SPoint& endPoint, const RtwPixel& color);
		virtual void DrawTriangle(const SPoint& p1, const SPoint& p2, const SPoint& p3, const RtwPixel& color);
		virtual void SetTextureRenderTarget(RtgTextItem* texItem ,bool bClearUp , long nBackColor );
		virtual void RestorRenderTarget( ) ;
		virtual RtgDevice* GetDevice() { return m_pRtgDevice; }
		virtual void BeginRenderViewPort(RtgCameraD3DView *pCam, DWORD dwX,DWORD dwY,DWORD dwWeight,DWORD dwHeight,float fMin,float fMax, bool bRefreshCam = true);
		virtual void EndRenderViewPort(bool bRefreshCam = true);
		virtual void SetMatrixFromCamera();
		virtual void RenderActorInstance(CRT_ActorInstance *pActor,RTGRenderMask eMask);
	private:
		Real            m_ZOrder;
		bool            m_bDrawing;
		DWORD           m_IB;
		RtgDevice*      m_pRtgDevice;
		RtgShader       m_DefaultShader;
		RtgCamera*		m_pLastCam;
	};

	//end rtui_render_renderdevice.h

	//rtui_render_font.h
	class CUiFontImpl_Rt : public RtwFont
	{
	private:
		static const int TEXTURE_WIDTH = 256;
		static const int TEXTURE_HEIGHT = 256;
		static const int TEXTURE_COUNT = 4;

		struct FontVertex
		{
			RtgVertex4	coord;
			DWORD		diffuse;
			RtgVertex2	texcoord;

			FontVertex(float x, float y, float z, DWORD color, float tx, float ty)
				: coord(x, y, z, 1)
				, diffuse(color)
				, texcoord(tx, ty)
			{
			}
		};

		struct FontVertex1
		{
			RtgVertex3	coord;
			DWORD		diffuse;
			RtgVertex2	texcoord;

			FontVertex1(float x, float y, float z, DWORD color, float tx, float ty)
				: coord(x, y, z )
				, diffuse(color)
				, texcoord(tx, ty)
			{
			}
		};

		struct CharIndex
		{
			BYTE    bSet;           // 用于记录字符是否被缓冲
			BYTE	texIdx;         // 贴图Index
			BYTE	row;            // 贴图中字符的行
			BYTE	col;            // 贴图中字符的列
		};

		struct CharQueuer
		{
			WORD    x;
			WORD    y;
			Real    z;
			WORD    width;
			WORD    height;
			Real    tx1;
			Real    ty1;
			Real    tx2;
			Real    ty2;
			DWORD	color;
			Real    scale;
		};

		struct StringTexture
		{
			bool	dirty;
			BYTE*	buffer;
			RtgShader* shader;
			std::vector<CharQueuer> pending;
		};

	public:
		CUiFontImpl_Rt(const std::string& FontName, int FontSize );//add by fox
		virtual ~CUiFontImpl_Rt();

	public:
//		virtual bool Create(RtgDevice* pDevice, const std::string& FontName, int FontSize);
		bool Create(RtgDevice* pDevice);//add by fox
		virtual void Close();
		virtual void OnInvalidateDevice();
		virtual void OnRestoreDevice();
		virtual void DrawString3D(const std::string& str, const SRect3D& rect, const RtwPixel& textColor, bool bWordWrap = true, SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear), bool bUnderLine = false, const RtwRect* pClipRect = NULL, int RenderSize = 0);
		virtual void DrawString(const std::string& str, const RtwRect& rect, const RtwPixel& textColor, bool bWordWrap = true, SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear), bool bUnderLine = false, const RtwRect* pClipRect = NULL, int RenderSize = 0);

		virtual void Flush() { DrawBufferToDevice(); } 
		virtual int GetStringWidth(const std::string& str, int length = -1, int RenderSize = 0);
		virtual bool PickCharacter(const SPoint& point, int& outIndex, bool& outDoubleByte, bool& outNear, const std::string& str, const RtwRect& rect, int RenderSize = 0);
		virtual bool PickCharacter(int width, int& outIndex, bool& outDoubleByte, bool& outNear, const std::string& str, int RenderSize = 0);


	private:
		void Reset();
		void _ResetTextureBuffer();

		void FillTexture(WORD character);
		void DrawCharacterToBuffer(WORD character, const RtwPixel& textColor, int xTarget, int yTarget, int width, int height, int xImage = 0, int yImage = 0, Real scale = 1.0);
		void DrawStringToBuffer(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, const RtwPixel& textColor, bool bWordWrap = true, int RenderSize = 0);
		void CalcStringPosition(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, bool bWordWrap, std::vector<WORD>& outArrCharacters, std::vector<short>& outArrPosX, std::vector<short>& outArrPosY, std::vector<short>& outArrWidth, std::vector<short>& outArrHeight, std::vector<short>& outArrImageX, std::vector<short>& outArrImageY, int RenderSize = 0);
		void CalcCharacterPosition(const WORD character, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, short& outPosX, short& outPosY, short& outWidth, short& outHeight, short& outImageX, short& outImageY, int RenderSize = 0);
		void DrawBufferToDevice();

		void DrawCharacterToBuffer3D(WORD character, const RtwPixel& textColor, int xTarget, int yTarget, float zTarget, int width, int height, int xImage = 0, int yImage = 0, Real scale = 1.0);
		void DrawStringToBuffer3D(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, float PosZ, const RtwPixel& textColor, bool bWordWrap  = true , int RenderSize  = 0 );
		void CalcStringPosition3D(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, float PosZ, bool bWordWrap, std::vector<WORD>& outArrCharacters, std::vector<short>& outArrPosX, std::vector<short>& outArrPosY,std::vector<float>& outArrPosZ, std::vector<short>& outArrWidth, std::vector<short>& outArrHeight, std::vector<short>& outArrImageX, std::vector<short>& outArrImageY, int RenderSize = 0);
		void CalcCharacterPosition3D(const WORD character, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, float PosZ, short& outPosX, short& outPosY, float& outPosZ, short& outWidth, short& outHeight, short& outImageX, short& outImageY, int RenderSize = 0);
		void DrawBufferToDevice3D();
		CharIndex	        m_indices[256 * 256];               // 字符映射到纹理的位置
		StringTexture       m_textures[TEXTURE_COUNT];          // 字符贴图

		int                 m_CurrBufferIndex;                  // 当前纹理缓冲的字符数
		int                 m_MaxCharBufferCount;               // 最多支持的纹理缓冲字符数

		int                 m_FontWidth;
		HDC                 m_hDC;
		HBITMAP             m_hBitmap;
		HFONT               m_hFont;
		BYTE*               m_BitmapBuffer;                     // 字符位图使用的临时缓冲区

		RtgVertex3          m_Pos;
		RtgDevice*          m_pRtgDevice;
	};

	class CUiFontManagerImpl_Rt : public IUiFontManager
	{
	public:
		CUiFontManagerImpl_Rt(RtgDevice* pRtgDevice);
		virtual ~CUiFontManagerImpl_Rt();

	public:
		virtual void Close();
		virtual RtwFont* OnCreateFont(const std::string& FontName,const int FontSize , int nFontStyle);
		virtual void Render();

		RtgDevice*          m_pRtgDevice;
	};

	//end rtui_render_font.h
}


#endif // _RTUI_RENDERDEVICE_H
