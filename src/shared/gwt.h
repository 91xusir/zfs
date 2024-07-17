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

	void				Initialize(unsigned		originTicks, // ��ʼ��Ϸ����ʱ�䣬��λ��
								   float		scaling); // ��ʵ����ʱ������Ϸ����ʱ�����

	void				Synchronize(unsigned originTicks);

public:
	GWT();

private:
	unsigned			m_originSysTicks;	// ��ʵ��ʵ�����ʱ�䣬��λ ��
	unsigned			m_originTicks;		// ��ʼ��Ϸ�����ʱ�䣬��λ ��
	unsigned			m_ticks;			// ��ǰ��Ϸ�����ʱ�䣬��λ ��
	float				m_factor;			// ��ʵ����ʱ������Ϸ����ʱ��Ļ������ӣ��԰������뵽�ֵ�ת��
};

#endif // _INC_GWT_H_
