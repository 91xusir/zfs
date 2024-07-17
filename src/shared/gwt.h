#ifndef _INC_GWT_H_
#define _INC_GWT_H_


/**
	GameWorldTime class
*/
class GWT
{
public:
	unsigned			Days() const { return m_ticks / (60 * 24); }

	unsigned			Hours() const { return (m_ticks / 60) % 24; }

	// Get real time in hour, plus 8 form time zone...... not good..... louis
	unsigned			RealHours() const { return (((m_ticks + m_originSysTicks / 60) / 60 ) % 24 + 8) % 24; }

	unsigned			Minutes() const { return m_ticks % 60; }

	unsigned			Ticks() const { return m_ticks; }

	float				Scaling() const { return m_factor * 60.0f; }
	
	unsigned			GetTrueTime();
public:
	void				Update();

	void				Initialize(unsigned		originTicks, // 起始游戏世界时间，单位分
								   float		scaling); // 真实世界时间与游戏世界时间比例

	void				Synchronize(unsigned originTicks);

public:
	GWT();

private:
	unsigned			m_originSysTicks;	// 其实真实世界的时间，单位 秒
	unsigned			m_originTicks;		// 起始游戏世界的时间，单位 分
	unsigned			m_ticks;			// 当前游戏世界的时间，单位 分
	float				m_factor;			// 真实世界时间与游戏世界时间的换算因子，以包括了秒到分的转换
};

#endif // _INC_GWT_H_
