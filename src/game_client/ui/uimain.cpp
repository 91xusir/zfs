#include "gc_include.h"

// ���� LoadUI ���������ݿؼ����Ƽ��ؿؼ�
RtwWidget* LoadUI(const char* szName)
{
    // ����ָ�� RtwWidget ���͵�ָ�� vpUI������ʼ��Ϊ nullptr
    RtwWidget* vpUI = nullptr;

    // ������ szName ת��Ϊ�ַ������� strName
    std::string strName = szName;

    // ��� strName �Ƿ���� "layworld"
    if (strName.find("layworld") == std::string::npos)
    {
        // ��������� "layworld"���� strName ǰ��� "layworld."
        strName = "layworld." + strName;
    }

    // ������ȫ�ֹ����ռ� g_workspace �в��ҿؼ������δ�ҵ�
    if (!g_workspace.FindWidget(strName, &vpUI))
    {
        // ���������Ϣ�ַ���
        std::string Temp("Can't find widget [");
        Temp = Temp + szName + "]!!!!!!!!!!!!!!!!!!!";

        //Ԥ��ģʽ�¼�¼������־
        P_LOGERROR(Temp);
        // DEBUG ��¼������־
        RtCoreLog().Error(Temp.c_str());

        // ִ�д������ CHECKEX
        CHECKEX(Temp.c_str());

        // ���� nullptr����ʾδ�ҵ��ؼ�
        return nullptr;
    }

    // �����ҵ��Ŀؼ�ָ��
    return vpUI;
}

