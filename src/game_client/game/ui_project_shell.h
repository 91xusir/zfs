
#ifndef UI_PROJECT_SHELL_H
#define UI_PROJECT_SHELL_H

#include <string>
class CUIProject;
class CUIShowProject;


using std::string;

//外壳
class CProject
{
public:
	/* gao 2009.12.26
	添加枚举，输入类型
	*/
	enum INPUTTYPE
	{
		AllInput,
		EnterInput,
		Undefine,
	};
	/* end */
	CProject();
	~CProject();

	bool Init();

	bool Clear();

	bool Load(string& vrString);
	bool Save(string& vrString);

	bool SetInputProject(string vInputProject="");//选择一个配置
	bool SetShowProject(string vShowProject="");//选择一个显示模式
	/* gao 2009.12.26
	// 获取当前的输入类型
	*/
	INPUTTYPE GetInputProject(){return m_eInputType;}
	/* end */

	void KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift);//按键按下
	void KeyChar(int vChar);//字符信息


private:
	/* gao 2009.12.26
	当前的输入类型
	*/
	INPUTTYPE			m_eInputType;
	/* end */
	CUIProject*			mpInputProject;//输入模式
	CUIShowProject*		mpShowProject;//显示模式
};


#endif//UI_PROJECT_SHELL_H