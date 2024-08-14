#ifdef _WIN32
#ifndef WSDLPULL_EXPORT_H
#define WSDLPULL_EXPORT_H

// 检查是否定义了 WSDLPULL_EXPORT
#ifndef WSDLPULL_EXPORT
// 如果是 Microsoft 编译器且定义了多线程构建
#if defined(_MSC_VER) && defined(_MT)
/*
                * Windows 构建
                * Microsoft 编译器会定义 _MSC_VER
                * 多线程构建会定义 _MT
                */
#ifdef DLLEXPORT
// 定义 DLLEXPORT 用于创建 LIB 或 DLL
#define WSDLPULL_EXPORT __declspec(dllexport)
#else
// 定义 DLLIMPORT 用于使用 LIB 或 DLL 的情况
#define WSDLPULL_EXPORT __declspec(dllimport)
#endif
#else
/*
                * 非 Windows 构建，如 Linux
                */
#define WSDLPULL_EXPORT
#endif
#endif

#endif  // WSDLPULL_EXPORT_H
#else
// 非 Windows 平台不需要定义
#define WSDLPULL_EXPORT
#endif

#define WSDLPULL_EXPORT
