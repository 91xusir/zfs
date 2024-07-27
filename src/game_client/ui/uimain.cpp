#include "gc_include.h"

// 定义 LoadUI 函数，根据控件名称加载控件
RtwWidget* LoadUI(const char* szName)
{
    // 定义指向 RtwWidget 类型的指针 vpUI，并初始化为 nullptr
    RtwWidget* vpUI = nullptr;

    // 将参数 szName 转换为字符串类型 strName
    std::string strName = szName;

    // 检查 strName 是否包含 "layworld"
    if (strName.find("layworld") == std::string::npos)
    {
        // 如果不包含 "layworld"，在 strName 前添加 "layworld."
        strName = "layworld." + strName;
    }

    // 尝试在全局工作空间 g_workspace 中查找控件，如果未找到
    if (!g_workspace.FindWidget(strName, &vpUI))
    {
        // 构造错误消息字符串
        std::string Temp("Can't find widget [");
        Temp = Temp + szName + "]!!!!!!!!!!!!!!!!!!!";

        //预览模式下记录错误日志
        P_LOGERROR(Temp);
        // DEBUG 记录错误日志
        RtCoreLog().Error(Temp.c_str());

        // 执行错误检查宏 CHECKEX
        CHECKEX(Temp.c_str());

        // 返回 nullptr，表示未找到控件
        return nullptr;
    }

    // 返回找到的控件指针
    return vpUI;
}

