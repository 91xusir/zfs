#ifndef GC_SEQUENCEIMAGE_H
#define GC_SEQUENCEIMAGE_H

namespace rt_graph {
    struct _RtgTextItem;
    typedef _RtgTextItem RtgTextItem;
}

using namespace std;

struct SequenceImageItem
{
    string filename;
    float x;
    float y;
    float width;
    float height;

    SequenceImageItem() {}
    SequenceImageItem(const char* name, float _x, float _y, float _width, float _height)
    {
        filename = name; x = _x; y = _y; width = _width; height = _height;
    }
};

class GcSequenceImage
{
public:
    GcSequenceImage();
    ~GcSequenceImage();
    void Render();
    void SetImages(vector<SequenceImageItem>& sequence);
    void SetRenderFlag(bool bRender);
    bool GetRenderFlag() { return m_bRenderFlag; }

private:
    void DrawSingleImage(const char* szFileName, float x, float y, float width, float height);
    void DrawSingleImage(SequenceImageItem& sequence);

    vector<SequenceImageItem> m_images;
    map<string, RtgTextItem*> m_listTex;

    bool m_bRenderFlag;
};


extern GcSequenceImage		g_LoadingMapRenderer;
extern GcSequenceImage		g_WorldMapRenderer;


#endif//GC_SEQUENCEIMAGE_H
