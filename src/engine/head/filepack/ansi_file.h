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

    // 构造函数，初始化文件指针、缓冲区指针、缓冲区大小等变量

    virtual ~CAnsiFile() { Close(); };  // 析构函数，调用 Close() 方法关闭文件并释放资源

    bool Create(const string& vrFileName);  // 创建文件的方法，参数为文件名
        // 打开文件的方法，参数为文件名和模式，默认以二进制读写模式打开
    bool Open(const string& vrFileName, const string vMode = "rb+");
    void Close();  // 关闭文件并释放资源

    // static bool ChangeMode(const char vpFileName[],int vMode); // 改变文件属性的方法（注释掉的版本）
    // 改变文件属性的方法，参数为文件名和模式
    static bool ChangeMode(const char vpFileName[], DWORD vMode);
    static bool DeleteFile(const string& vrFileName);  // 静态方法，删除指定文件

    const string& Name();       // 返回文件名
    const string& OpenMode();   // 返回文件打开模式
    const long    WriteTime();  // 返回文件最后修改时间

    const long Size();  // 返回文件大小
    const long Tell();  // 返回文件当前位置（文件指针位置）
        // 设置文件指针位置，参数为偏移量和偏移起始位置，默认从当前位置偏移
    int Seek(long vOffset, int vOrigin = SEEK_CUR);

    void Flush() { fflush(mpFile); };  // 刷新缓冲区，将数据写入文件

    const char* GetString(int vCharCount);  // 从文件读取指定数量的字符，返回指向字符的指针
    bool PutString(const char* vpString,
                   const size_t vStringLength = 0);  // 将字符串写入文件，参数为字符串和长度（可选）

    size_t GetBuffer(const int vByteCount, void* vpBuffer);  // 从文件读取指定字节数的数据到缓冲区
    size_t PutBuffer(const void* vpBuffer, const int vBufferSize);  // 将缓冲区的数据写入文件

   private:
    bool ReSizeBuffer(const int vSize);  // 调整缓冲区大小的方法

    FILE*       mpFile;       // 文件指针
    char*       mpBuffer;     // 指向缓冲区的指针
    int         mBufferSize;  // 缓冲区大小
    CFileErrStd mErr;         // 文件错误处理对象
    string      mName;        // 文件名
    int         mCurrent;     // 当前读/写位置
    string      mMode;        // 文件打开模式

    enum EReadMode  // 枚举类型，表示读写模式
    {
        NULL_MODE  = 0,  // 无模式
        READ_MODE  = 1,  // 读模式
        WRITE_MODE = 2   // 写模式
    };

    EReadMode mLastMode;  // 最后一次读写的模式
};
