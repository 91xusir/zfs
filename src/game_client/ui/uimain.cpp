#include "uimain.h"
#include <string>

// 定义 LoadUI 函数，根据控件名称加载控件
RtwWidget* LoadUI(const std::string& szName) {
    // 定义指向 RtwWidget 类型的指针 vpUI，并初始化为 nullptr
    RtwWidget* vpUI = nullptr;

    // 检查 strName 是否包含 "layworld"
    std::string strName = szName;
    if (strName.find("layworld") == std::string::npos) {
        strName = "layworld." + strName;
    }

    // 尝试在全局工作空间 g_workspace 中查找控件，如果未找到
    if (!g_workspace.FindWidget(strName, &vpUI)) {
        std::string Temp = "Can't find widget [" + strName + "]";
        //预览模式下记录错误日志
        P_LOGERROR(Temp);
        // DEBUG 记录错误日志
        RtCoreLog().Error(Temp.c_str());
        // 执行错误检查宏 CHECKEX
        CHECKEX(Temp.c_str());
        // 返回 nullptr，表示未找到控件
        return nullptr;
    }
    return vpUI;
}
