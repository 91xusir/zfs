
#ifndef UI_PROJECT_SHELL_H
#define UI_PROJECT_SHELL_H

#include <string>
class CUIProject;
class CUIShowProject;


using std::string;

//���
class CProject
{
public:
	/* gao 2009.12.26
	���ö�٣���������
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

	bool SetInputProject(string vInputProject="");//ѡ��һ������
	bool SetShowProject(string vShowProject="");//ѡ��һ����ʾģʽ
	/* gao 2009.12.26
	// ��ȡ��ǰ����������
	*/
	INPUTTYPE GetInputProject(){return m_eInputType;}
	/* end */

	void KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift);//��������
	void KeyChar(int vChar);//�ַ���Ϣ


private:
	/* gao 2009.12.26
	��ǰ����������
	*/
	INPUTTYPE			m_eInputType;
	/* end */
	CUIProject*			mpInputProject;//����ģʽ
	CUIShowProject*		mpShowProject;//��ʾģʽ
};


#endif//UI_PROJECT_SHELL_H