#pragma once
#include "windows.h"
#include "filepack/file_error.h"
#include <cstdio>
#include <string>
#include <fileapi.h>


using std::string;

class CAnsiFile {
   public:
    CAnsiFile()
        : mpFile(NULL), mpBuffer(NULL), mBufferSize(0), mErr(), mName(), mCurrent(0), mMode(),
          mLastMode(NULL_MODE) {}

    // ���캯������ʼ���ļ�ָ�롢������ָ�롢��������С�ȱ���

    virtual ~CAnsiFile() { Close(); };  // �������������� Close() �����ر��ļ����ͷ���Դ

    bool Create(const string& vrFileName);  // �����ļ��ķ���������Ϊ�ļ���
        // ���ļ��ķ���������Ϊ�ļ�����ģʽ��Ĭ���Զ����ƶ�дģʽ��
    bool Open(const string& vrFileName, const string vMode = "rb+");
    void Close();  // �ر��ļ����ͷ���Դ

    // static bool ChangeMode(const char vpFileName[],int vMode); // �ı��ļ����Եķ�����ע�͵��İ汾��
    // �ı��ļ����Եķ���������Ϊ�ļ�����ģʽ
    static bool ChangeMode(const char vpFileName[], DWORD vMode);
    static bool DeleteFile(const string& vrFileName);  // ��̬������ɾ��ָ���ļ�

    const string& Name();       // �����ļ���
    const string& OpenMode();   // �����ļ���ģʽ
    const long    WriteTime();  // �����ļ�����޸�ʱ��

    const long Size();  // �����ļ���С
    const long Tell();  // �����ļ���ǰλ�ã��ļ�ָ��λ�ã�
        // �����ļ�ָ��λ�ã�����Ϊƫ������ƫ����ʼλ�ã�Ĭ�ϴӵ�ǰλ��ƫ��
    int Seek(long vOffset, int vOrigin = SEEK_CUR);

    void Flush() { fflush(mpFile); };  // ˢ�»�������������д���ļ�

    const char* GetString(int vCharCount);  // ���ļ���ȡָ���������ַ�������ָ���ַ���ָ��
    bool PutString(const char* vpString,
                   const size_t vStringLength = 0);  // ���ַ���д���ļ�������Ϊ�ַ����ͳ��ȣ���ѡ��

    size_t GetBuffer(const int vByteCount, void* vpBuffer);  // ���ļ���ȡָ���ֽ��������ݵ�������
    size_t PutBuffer(const void* vpBuffer, const int vBufferSize);  // ��������������д���ļ�

   private:
    bool ReSizeBuffer(const int vSize);  // ������������С�ķ���

    FILE*       mpFile;       // �ļ�ָ��
    char*       mpBuffer;     // ָ�򻺳�����ָ��
    int         mBufferSize;  // ��������С
    CFileErrStd mErr;         // �ļ����������
    string      mName;        // �ļ���
    int         mCurrent;     // ��ǰ��/дλ��
    string      mMode;        // �ļ���ģʽ

    enum EReadMode  // ö�����ͣ���ʾ��дģʽ
    {
        NULL_MODE  = 0,  // ��ģʽ
        READ_MODE  = 1,  // ��ģʽ
        WRITE_MODE = 2   // дģʽ
    };

    EReadMode mLastMode;  // ���һ�ζ�д��ģʽ
};
