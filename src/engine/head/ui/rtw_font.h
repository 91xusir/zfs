//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RTUI_FONT_H
#define _RTUI_FONT_H

namespace ui {

class RtwFont {
    friend class IUiFontManager;

   public:
    enum {
        FONT_NORMAL = 0x00,
        ///!斜体
        FONT_ITALIC = 0x01 << 0,
        ///!粗体
        FONT_BOLD = 0x01 << 1,
        ///!空心
        FONT_HOLLOW = 0x01 << 2,
        ///!带立体阴影的字体
        FONT_SHADOW = 0x01 << 3,
    };

   public:
    virtual ~RtwFont() {}

   protected:
    RtwFont(const std::string& FontName, int FontSize)  //add by fox
        : m_Id(0), m_FontName(FontName), m_FontSize(FontSize), m_bIsItalic(false),
          m_b3DShadow(false), m_bIsBolder(false), m_bIsHollow(false), m_nFontStyle(FONT_NORMAL) {}

   public:
    virtual void Close() = 0;
    virtual void OnInvalidateDevice() = 0;
    virtual void OnRestoreDevice() = 0;
    virtual void DrawString3D(const std::string& str, const SRect3D& rect,
                              const RtwPixel& textColor, bool bWordWrap = true,
                              SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear),
                              bool bUnderLine = false, const RtwRect* pClipRect = NULL,
                              int RenderSize = 0) = 0;
    virtual void DrawString(const std::string& str, const RtwRect& rect, const RtwPixel& textColor,
                            bool             bWordWrap = true,
                            SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear),
                            bool bUnderLine = false, const RtwRect* pClipRect = NULL,
                            int RenderSize = 0) = 0;
    virtual void Flush() = 0;
    virtual int  GetStringWidth(const std::string& str, int length = -1, int RenderSize = 0) = 0;
    virtual bool PickCharacter(const SPoint& point, int& outIndex, bool& outDoubleByte,
                               bool& outNear, const std::string& str, const RtwRect& rect,
                               int RenderSize = 0) = 0;
    virtual bool PickCharacter(int width, int& outIndex, bool& outDoubleByte, bool& outNear,
                               const std::string& str, int RenderSize = 0) = 0;

    const std::string& getFontName() const { return m_FontName; }

    const int& getFontSize() const { return m_FontSize; }

   public:
    inline void SetFontStyle(int nSpecialStyle) {
        if (nSpecialStyle & FONT_ITALIC)
            m_bIsItalic = true;
        if (nSpecialStyle & FONT_BOLD)
            m_bIsBolder = true;
        if (nSpecialStyle & FONT_HOLLOW)
            m_bIsHollow = true;
        if (nSpecialStyle & FONT_SHADOW)
            m_b3DShadow = true;
        m_nFontStyle = nSpecialStyle;
    }

    inline int GetFontStyle() const { return m_nFontStyle; }

   protected:
    DWORD       m_Id;        // 唯一ID
    std::string m_FontName;  // 字体名字
    int         m_FontSize;  // 字体大小
    ///-------------------------------------------
    int  m_nFontStyle;
    bool m_bIsBolder;  //粗体
    bool m_b3DShadow;  //阴影
    bool m_bIsItalic;  //斜体
    bool m_bIsHollow;  //空心
                       ///-------------------------------------------
};

class IUiFontManager {
   public:
    IUiFontManager();
    virtual ~IUiFontManager();

   public:
    RtwFont*     CreateFont(const std::string& FontName, const int FontSize,
                            int nFontStyle = RtwFont::FONT_NORMAL);
    void         CloseFont(const std::string& FontName);
    void         Close();
    virtual void Render() = 0;
    virtual void OnInvalidateDevice();
    virtual void OnRestoreDevice();

    void DrawString3D(const std::string& str, const SRect3D& rect, const RtwPixel& textColor,
                      bool             bWordWrap = true,
                      SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear),
                      bool bUnderLine = false, const RtwRect* pClipRect = NULL,
                      const std::string& FontName = "");
    void DrawString(const std::string& str, const RtwRect& rect, const RtwPixel& textColor,
                    bool             bWordWrap = true,
                    SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear),
                    bool bUnderLine = false, const RtwRect* pClipRect = NULL,
                    const std::string& FontName = "", const int& iFontSize = 0);
    void DrawString(const std::string& str, const RtwRect& rect, const RtwPixel& textColor,
                    RtwFont* pkFont, bool bWordWrap = true,
                    SUiTextAlignment align = SUiTextAlignment(alignNear, alignNear),
                    bool bUnderLine = false, const RtwRect* pClipRect = NULL);

    int GetStringWidth(const std::string& str, int length = -1, const std::string& FontName = "");
    //int CutStringToFitWidth(const std::string& str, int width, const std::string& FontName = ""); // 取得width长度可以显示多少个str字符
    bool PickCharacter(const SPoint& point, int& outIndex, bool& outDoubleByte, bool& outNear,
                       const std::string& str, const RtwRect& rect,
                       const std::string& FontName = "");
    bool PickCharacter(int width, int& outIndex, bool& outDoubleByte, bool& outNear,
                       const std::string& str, const std::string& FontName = "");

    RtwFont* getDefaultFont();
    RtwFont* getChineseFont();
    RtwFont* getEnglishFont();
    // 		RtwFont* getUISystemFont() const
    // 		{
    // 			return ( m_Fonts.size() < 2 )  ? NULL : m_Fonts[ 1 ];
    // 		}
    // 		RtwFont* getUISystemFont16() const
    // 		{
    // 			return ( m_Fonts.size() < 3 )  ? NULL : m_Fonts[ 2 ];
    // 		}
    // 		RtwFont* getUISystemFont14() const
    // 		{
    // 			return ( m_Fonts.size() < 4 )  ? NULL : m_Fonts[ 3 ];
    // 		}
    // 		RtwFont* getUIBillBordFont() const
    // 		{
    // 			return ( m_Fonts.size() < 5 )  ? NULL : m_Fonts[ 4 ];
    // 		}
    // 		RtwFont* getUIUltraTextFont() const
    // 		{
    // 			return ( m_Fonts.size() < 6 )  ? NULL : m_Fonts[ 5 ];
    // 		}

    int getFontSize(const std::string& FontName = "");

   protected:
    virtual RtwFont* OnCreateFont(const std::string& FontName, const int FontSize,
                                  int nFontStyle) = 0;  //创建一个系列字体
    RtwFont* FindFont(const std::string& FontName, int nFontStyle = -1, int nFontSize = -1) const;

   protected:
    std::vector<RtwFont*> m_Fonts;
};

}  // namespace ui

#endif  // _RTUI_FONT_H
