#ifndef _RTUI_IMAGE_H
#define _RTUI_IMAGE_H
#include <assert.h>

#define IMAGETICK(img, time) if (img){(img)->Tick(time);}
#define DROP_TRUI_IMAGE(ptr) if (ptr){/*(ptr)->DropImageList();*//* DROP_RTUI_OBJECT((ptr)->border);*/ DROP_RTUI_OBJECT(ptr);}
#define GRAB_TRUI_IMAGE(ptr) if ((ptr)){(ptr)->GrabImageList(); (ptr)->grab();}

template<typename pixel_t>
struct _PiexlAccess
{
	_PiexlAccess(void* _data, int _width, int _height, int _pitch)
		: pdata(_data),
		width(_width),
		height(_height),
		pitch(_pitch)
	{
	}

	inline void set(int x, int y, pixel_t val)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		((pixel_t*)(((char*)pdata) + y * pitch))[x] = val;
	}

	inline pixel_t get(int x, int y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			assert(false);
		return ((pixel_t*)(((char*)pdata) + y * pitch))[x];
	}

	inline bool get(int x, int y, pixel_t* p)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return false;
		(*p) = ((pixel_t*)(((char*)pdata) + y * pitch))[x];
		return true;
	}

	inline DWORD get(int sx, int sy, int dx, int dy)
	{
		RTASSERT(sizeof(DWORD) == sizeof(pixel_t));
		DWORD a = 0;
		DWORD r = 0;
		DWORD g = 0;
		DWORD b = 0;
		DWORD cNum = 0;
		for (int x = sx; x < dx; ++x)
			for (int y = sy; y < dy; ++y)
			{
				DWORD c = 0;
				if (get(x, y, &c))
				{
					a += ((c >> 24) & 0xff);
					r += ((c >> 16) & 0xff);
					g += ((c >>  8) & 0xff);
					b += ((c >>  0) & 0xff);
					++cNum;
				}
			}
			return (DWORD)(((a / cNum) << 24) | ((r / cNum) << 16) | ((g / cNum) << 8) | (b / cNum));
	}

	void cubeBlurAlpha(int sizCube = 1)
	{
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				DWORD c = get(x, y);
				DWORD a = 0;
				DWORD cNum = 0;
				for (int i = x - sizCube; i <= x + sizCube; ++i)
					for (int j = y - sizCube; j <= y + sizCube; ++j)
					{
						DWORD t;
						if (get(i, j, &t))
						{
							a += ((t >> 24) & 0xff);
							++cNum;
						}
					}
					set(x, y, (c & 0x00ffffff) | ((a / cNum) << 24));
			}
	}

	const int width;
	const int height;
	const int pitch;
	void* pdata;
};

namespace ui
{
	class IUiImageFactory;
	enum EImageFillMode
	{
		ImageFillMode_Stretch,
		ImageFileMode_Repeat,
	};

	enum EImageBlendMode // @@@@@ 尚未使用
	{
		ImageBlendMode_Disable,             // opaque texture
		ImageBlendMode_Blend,               // normal blending
		ImageBlendMode_AlphaKey,            // 1-bit alpha
		ImageBlendMode_Add,                 // additive blend
		ImageBlendMode_Mod                 // modulating blend
	};

	class RtwImage : public RtwRefObject
	{
		RT_DECLARE_DYNCREATE(RtwImage, RtwRefObject, 0, "ui")

			friend class IUiImageFactory;
		typedef RtwRefPtr<RtwImage> Ptr;
	public:
		RtwImage();
		virtual ~RtwImage() { OnBeforeDestroy();}
	public:
		// base class inherit
		virtual void OnBeforeDestroy() { RtwRefObject::OnBeforeDestroy(); }

		virtual bool LoadFromFile(const std::string& filename) = 0;
		virtual bool LoadFromMemory(const void* data, int length) = 0;
		virtual bool LoadFromMemory(int width, int height, int length, const void* data) = 0;
		virtual bool LoadFromRtArchive(RtArchive *pArc,int iLength,bool bflip = false) = 0;
		virtual void SetBlend(bool bBlend) = 0;
		virtual void SetSourceRect(RtwRect& vrRect) = 0;//add by fox

		Real getTransparency() const { return m_Transparency; }
		SSize getSize() const { return SSize(m_Source.getWidth(), m_Source.getHeight()); }
		RtwRect& getSourceRect(){ return m_Source; };//add by fox
		void	SetTransparency(float transparen);			//图片本身透明度
		void	SetCtrlTransparent(float transparent);		//要显示的透明度（作为淡入淡出用）
		virtual void Tick(DWORD dwMilliseconds) { }
		virtual RtwImage* getRenderImage() { return this; }
		RtwImage*	GetImage()	{	return this;	}
		void	SetColor(RtwPixel color)	{}
		void DropImageList(){}
		void GrabImageList(){}
	public:
		RtwImage*		border;
		SMargin			borderSize;		
		std::string     m_FileName;
		std::string     m_Name;
		RtwPixel          m_Color;
		Real            m_Transparency;
		Real			m_BackTransparency;
		bool            m_bBlend;
		EImageFillMode  m_FillMode;
		int				eBlendDstFunc;
		int				eBlendSrcFunc;
		bool            m_bHighLight;
	protected:
		RtwRect         m_Source;

	};

	struct SSequenceImage
	{
		int         m_RenderTime;

		SSequenceImage():m_pImage(NULL) {}
		~SSequenceImage()
		{
			DROP_TRUI_IMAGE(m_pImage);
		}
		SSequenceImage(const SSequenceImage& Other)
		{
			m_pImage = NULL;

			m_RenderTime = Other.m_RenderTime;
			DROP_TRUI_IMAGE(m_pImage);
			m_pImage = Other.m_pImage;
			if (m_pImage)
			{
				GRAB_TRUI_IMAGE(m_pImage);
			}
		}
		const SSequenceImage& operator=(const SSequenceImage& Other)
		{
			m_pImage = NULL;

			m_RenderTime = Other.m_RenderTime;
			DROP_TRUI_IMAGE(m_pImage);
			m_pImage = Other.m_pImage;
			if (m_pImage)
			{
				GRAB_TRUI_IMAGE(m_pImage);
			}

			return *this;
		}
		bool CloneFrom(const SSequenceImage* pOther)
		{
			m_pImage = NULL;

			m_RenderTime = pOther->m_RenderTime;
			DROP_TRUI_IMAGE(m_pImage);
			m_pImage = pOther->m_pImage;
			if (m_pImage)
			{
				GRAB_TRUI_IMAGE(m_pImage);
			}

			return true;
		}

		void SetImage(RtwImage* pImage, DWORD delay=50)
		{
			DROP_TRUI_IMAGE(m_pImage);
			m_pImage = pImage;
			if (m_pImage)
			{
				GRAB_TRUI_IMAGE(m_pImage);
			}
			m_RenderTime = delay;
		}
		RtwImage* getImage() { return m_pImage; }
		void ReleaseImage()
		{
			DROP_TRUI_IMAGE(m_pImage);
		}

		void GrabImage()
		{
			if (m_pImage)
			{
				GRAB_TRUI_IMAGE(m_pImage);
			}
		}

	private:
		RtwImage*   m_pImage;
	};

	class RtwComplexImage : public RtwImage
	{
		RT_DECLARE_DYNCREATE(RtwComplexImage, RtwImage, 0, "ui");
		friend class IUiImageFactory;
	public:
		RtwComplexImage();
		virtual ~RtwComplexImage();
		virtual void OnBeforeDestroy();
		void Reset();
		virtual void Tick(DWORD dwMilliseconds);
		virtual RtwImage* getRenderImage() { return getCurrentImage() ? getCurrentImage() : NULL; }
		virtual bool LoadFromFile(const std::string& filename) { return false; }
		virtual bool LoadFromMemory(const void* data, int length) { return false; }
		virtual bool LoadFromMemory(int width, int height, int length, const void* data) {return false;}
		virtual bool LoadFromRtArchive(RtArchive *pArc,int iLength,bool bflip = false){return false;}
		virtual void SetTransparency(Real trans);
		virtual void SetCtrlTransparent(Real trans);
		virtual void SetBlend(bool bBlend);
		virtual void SetSourceRect(RtwRect& vrRect);// {for (int i=0; i<getImageSize(); ++i) getImage(i)->SetSourceRect(vrRect);}
		bool AddImage(RtwImage* pImage, int RenderTime = 0);
		RtwImage* getCurrentImage();
		int getImageCount() { return (int)m_Images.size(); }
		RtwImage* getImage(int index);
		SSize getImageSize() { return getCurrentImage() ? getCurrentImage()->getSize() : SSize(0, 0); }
		void DropImageList()
		{
			for (int i=0; i<getImageCount(); ++i)
			{
				m_Images[i].ReleaseImage();
			}

			while (getImageCount() > 0)
			{
				m_Images.erase(m_Images.begin());
			}
			m_Images.clear();
			m_Images.swap(m_Images);
		}

		void GrabImageList()
		{
			m_Play_CurrentImage = 0;
			m_nDirtyTime = 0;
			for (int i=0; i<getImageCount(); ++i)
			{
				m_Images[i].GrabImage();
			}
		}

		void Pause(){m_bPause = true;}
		void Run(){m_bPause = false;}
		void SetCount(int nCount=0){m_nCount = nCount;}
	private:
		int		m_Play_CurrentImage;// 正在播放的图片
		int		m_nDirtyTime;
		int     m_nCount;		//播放次数
		std::map<int, RtwEventDispatcher>::iterator m_iterCountEvent;//
		bool	m_bPause;//暂停播放
	public:
		std::vector<SSequenceImage> m_Images;        // 图片序列
		std::map<int, RtwEventDispatcher> m_CountEvent;
	};

	class CGifSequence : public RtwComplexImage
	{
		RT_DECLARE_DYNCREATE(CGifSequence, RtwComplexImage, 0, "ui");
		friend class IUiImageFactory;
	public:
		CGifSequence();
		~CGifSequence();
		virtual bool LoadFromFile(const std::string& filename);
		virtual bool LoadFromGif(const std::string &filename);
		virtual void OnBeforeDestroy();
	private:
		int FrameWidth, FrameHeight;	// Dimensions of ImageSet in pixels.
	};

	class CUiImageRegister
	{
	public:
		CUiImageRegister(){};
		virtual ~CUiImageRegister();
	public:
		void registerImage(RtwImage* pImage);					//创建一个物件//并登记它//用来核查它的生存期
		void unregisterImage(RtwImage* pObject);

	protected:
		EXT_SPACE::unordered_map<RtwImage*, DWORD> m_LiveImages;		//用来记录已经生成的物件
	};

	class IUiImageFactory
	{
	public:
		IUiImageFactory() {/*	m_mapImage.clear();*/}
		virtual ~IUiImageFactory(){};

	public:
		RtwImage* createImage(const std::string& filename);		//根据文件名字得到图像
		RtwImage* createImage(const void* data, int length);    //从内存得到图像
		RtwImage* createImage();								//得到一个图像//引用记数加一//并登记
		RtwImage* createImage(RtArchive *pArc,int length,bool bflip = false);
		RtwComplexImage* createImageSequence();
		CGifSequence* createGifSequence();
		void OnDeleteImage(RtwImage* pImage);
	public:
		EXT_SPACE::unordered_map<RtwImage*, DWORD> m_LiveImages;
		static CUiImageRegister*  UiImageFactory( )
		{
			if( mpkRegister == NULL )
			{
				mpkRegister = RT_NEW CUiImageRegister;
			}
			return mpkRegister;
		}
		static void DestroyImageFactor( )
		{
			if( mpkRegister )
			{
				DEL_ONE(mpkRegister);
				mpkRegister = NULL;
			}
		}
	private:
		static CUiImageRegister*  mpkRegister;

// 		EXT_SPACE::unordered_map<std::string, RtwImage*>				m_mapImage;
// 		EXT_SPACE::unordered_map<std::string, RtwImage*>::iterator	m_mapImageIter;
	protected:
		virtual RtwImage* RealCreateImage()=0;					//得到具体的某种RtwImage
	};
}
#endif 
