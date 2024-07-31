#include "uimain.h"
#include <string>

// ���� LoadUI ���������ݿؼ����Ƽ��ؿؼ�
RtwWidget* LoadUI(const std::string& szName) {
    // ����ָ�� RtwWidget ���͵�ָ�� vpUI������ʼ��Ϊ nullptr
    RtwWidget* vpUI = nullptr;

    // ��� strName �Ƿ���� "layworld"
    std::string strName = szName;
    if (strName.find("layworld") == std::string::npos) {
        strName = "layworld." + strName;
    }

    // ������ȫ�ֹ����ռ� g_workspace �в��ҿؼ������δ�ҵ�
    if (!g_workspace.FindWidget(strName, &vpUI)) {
        std::string Temp = "Can't find widget [" + strName + "]";
        //Ԥ��ģʽ�¼�¼������־
        P_LOGERROR(Temp);
        // DEBUG ��¼������־
        RtCoreLog().Error(Temp.c_str());
        // ִ�д������ CHECKEX
        CHECKEX(Temp.c_str());
        // ���� nullptr����ʾδ�ҵ��ؼ�
        return nullptr;
    }
    return vpUI;
}
