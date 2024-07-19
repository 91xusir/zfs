
namespace rt_audio {

class RtaBackMusic
{
public:
    friend bool rtSoundInit(const char* szPackage, HWND hWnd);
	friend bool rtMusicInit();
    friend void rtAudioPreExit();
    friend void rtAudioExit();

protected:
    RtaBackMusic();
    ~RtaBackMusic();

    bool Initialize(int iMaxVolume=128);
    void Cleanup();

public:
    void SetMusicPath(const char* szPathname); // 设置目录，所有的音乐文件都要在这个目录里面，(eg. "audio/")

    bool Play(const char* szFileName, bool bLoop = false);
    void SetVolume(float fVolume=1.0f); // 设置音量，值的范围为0-1, 0最小, 1最大
    float GetVolume();
    bool Stop();
	static void Finish();
	bool IsFinished();
	void Quiet(bool bQuiet); // 静音

	bool IsPlaying();

protected:
    bool        m_bInit;
    bool        m_bQuiet;
    bool        m_bLoop;
    bool        m_bPlaying;
	static bool m_bfinished;
    RtString    m_strFilename;
    RtString    m_strPathname;

private:
    void*       m_pMusic;
};
} // namespace rt_audio
using namespace rt_audio;

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
