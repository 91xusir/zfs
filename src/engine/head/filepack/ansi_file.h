
#ifndef ANSI_FILE_H 
#define ANSI_FILE_H 

#include "core/rt2_core.h"
#include "filepack/file_error.h"

using std::string;

class CAnsiFile
{
public:
	CAnsiFile():mpFile(NULL),mpBuffer(NULL),mBufferSize(0),mErr(),mName(),mCurrent(0),mMode(),mLastMode(NULL_MODE){}
	virtual ~CAnsiFile(){Close();};

	bool Create(const string& vrFileName);
	bool Open(const string& vrFileName,const string vMode="rb+");
	void Close();
	//static bool ChangeMode(const char vpFileName[],int vMode);//改变文件属性//SYS\STAT.H
	static bool ChangeMode(const char vpFileName[],DWORD vMode);
	static bool DeleteFile(const string& vrFileName);


	const string& Name();
	const string& OpenMode();
	const long WriteTime();//修改时间

	const long  Size();
	const long	Tell();
	int			Seek(long vOffset,int vOrigin=SEEK_CUR);
	void		Flush(){fflush(mpFile);};



	const char*	GetString(int vCharCount);
	bool	PutString(const char* vpString,const size_t vStringLength=0);

	size_t	GetBuffer(const int vByteCount,void* vpBuffer);
	size_t	PutBuffer(const void* vpBuffer,const int vBufferSize);

private:

	bool ReSizeBuffer(const int vSize);


	FILE*		mpFile;
	char*		mpBuffer;
	int			mBufferSize;
	CFileErrStd	mErr;
	string		mName;
	int			mCurrent;
	string      mMode;
	enum		EReadMode
	{
		NULL_MODE=0,
		READ_MODE=1,
		WRITE_MODE=2
	};

	EReadMode   mLastMode;//最后一次读写的模式
};






#endif//ANSI_FILE_H 

