#ifdef _WIN32
#ifndef WSDLPULL_EXPORT_H
#define WSDLPULL_EXPORT_H

// ����Ƿ����� WSDLPULL_EXPORT
#ifndef WSDLPULL_EXPORT
// ����� Microsoft �������Ҷ����˶��̹߳���
#if defined(_MSC_VER) && defined(_MT)
/*
                * Windows ����
                * Microsoft �������ᶨ�� _MSC_VER
                * ���̹߳����ᶨ�� _MT
                */
#ifdef DLLEXPORT
// ���� DLLEXPORT ���ڴ��� LIB �� DLL
#define WSDLPULL_EXPORT __declspec(dllexport)
#else
// ���� DLLIMPORT ����ʹ�� LIB �� DLL �����
#define WSDLPULL_EXPORT __declspec(dllimport)
#endif
#else
/*
                * �� Windows �������� Linux
                */
#define WSDLPULL_EXPORT
#endif
#endif

#endif  // WSDLPULL_EXPORT_H
#else
// �� Windows ƽ̨����Ҫ����
#define WSDLPULL_EXPORT
#endif

#define WSDLPULL_EXPORT
