#include "rtc2_filemgr_generic.h"

#ifdef WIN32
#   include <io.h>
#   include <direct.h>
#   include <stdio.h>
#else
#   ifndef FREE_BSD
#       include <sys/io.h>
#		ifdef CYGWIN
#			include <dirent.h>
#		else
#			include <sys/dir.h>
#		endif
#   endif
#endif

#include <sys/stat.h>
#include <errno.h>

namespace rt2_core {

#ifdef WIN32    
#   define rtsys_fmode_S_IREAD     _S_IREAD
#   define rtsys_fmode_S_IWRITE    _S_IWRITE
#   define rtsys_fmode_O_RDONLY    _O_RDONLY
#   define rtsys_struct_stat       struct _stat
#   define rtsys_stat              _stat
#   define rtsys_open              _open
#   define rtsys_fstat             _fstat
#   define rtsys_close             _close
#	define DIRECT				   direct
#else
#   ifdef FREE_BSD
#       define rtsys_fmode_S_IREAD      S_IREAD
#       define rtsys_fmode_S_IWRITE     S_IWRITE
#       define rtsys_fmode_O_RDONLY    O_RDONLY
#       define rtsys_struct_stat       struct stat
#       define rtsys_stat              stat
#       define rtsys_open              open
#       define rtsys_fstat             fstat
#       define rtsys_close             close
#		define DIRECT				   direct
#   else
#		ifdef CYGWIN
#			define rtsys_fmode_S_IREAD      S_IREAD
#			define rtsys_fmode_S_IWRITE     S_IWRITE
#			define DIRECT				   dirent
#		else
#			define rtsys_fmode_S_IREAD      __S_IREAD
#			define rtsys_fmode_S_IWRITE     __S_IWRITE
#			define DIRECT				   direct
#		endif
#       define rtsys_fmode_O_RDONLY    O_RDONLY
#       define rtsys_struct_stat       struct stat
#       define rtsys_stat              stat
#       define rtsys_open              open
#       define rtsys_fstat             fstat
#       define rtsys_close             close
#   endif
#endif

/*------------------------------------------------------------------------
    RtArchiveFileReader
------------------------------------------------------------------------*/
class RtArchiveFileReader : public RtArchive
{
public:
    RtArchiveFileReader( FILE* InFile, int InSize ) :
      m_hFile(InFile),
      m_iSize(InSize)
    {
        fseek(m_hFile, 0, SEEK_SET);
        m_nMode |= RtArchive::load;
    }
    virtual ~RtArchiveFileReader()
    {
        if (m_hFile)
            Close();
    }
    virtual void Seek( int InPos, SeekMode eSeek )
    {
        int iSeek = (eSeek==SeekBegin) ? SEEK_SET : ((eSeek==SeekCurrent) ? SEEK_CUR : SEEK_END );
		if( fseek(m_hFile,InPos,iSeek) )
		{
			m_bIsError = 1;
            RtCoreLog().Error("RtArchiveFileReader: Seek Failed %i/%i: %i %i\n", InPos, m_iSize, Tell(), ferror(m_hFile) );
		}
    }
    virtual int  Tell()
    {
		return ftell(m_hFile);
    }
    virtual int  TotalSize()
    {
		return m_iSize;
    }
    virtual BOOL Close()
    {
		if (m_hFile)
			fclose(m_hFile);
		m_hFile = NULL;
		return !m_bIsError;
    }
    virtual void Flush()
    {
        fflush(m_hFile);
    }
    virtual long Serialize( void* V, int iLength )
    {
        if (iLength==0)
        {
            return 0;
        }
        long lResult = (long)fread(V, 1, iLength, m_hFile);
        RtCoreFile().AddReadByte(lResult);
        if (lResult<iLength)
        {
            if (ferror(m_hFile))
            {
                m_bIsError = 1;
                RtCoreLog().Error( "RtArchiveFileReader: fread Failed: Length=%i pError=%i\n", iLength, ferror(m_hFile) );
            }
        }
        return lResult;
    }

protected:
    FILE*           m_hFile;
    int             m_iSize;
};

/*------------------------------------------------------------------------
    RtArchiveFileWriter
------------------------------------------------------------------------*/
class RtArchiveFileWriter : public RtArchive
{
public:
    RtArchiveFileWriter(FILE* InFile) : m_hFile(InFile)
    {
        fseek(m_hFile, 0, SEEK_SET);
        m_nMode |= RtArchive::store;
    }
    virtual ~RtArchiveFileWriter()
    {
        if (m_hFile)
            Close();
    }
    virtual void Seek( int InPos, SeekMode eSeek )
    {
		Flush();
        int iSeek = (eSeek==SeekBegin) ? SEEK_SET : ((eSeek==SeekCurrent) ? SEEK_CUR : SEEK_END );
		if( fseek(m_hFile,InPos,iSeek) )
		{
			m_bIsError = 1;
            RtCoreLog().Error("RtArchiveFileReader: Seek Failed %i: %i %i\n", InPos, Tell(), ferror(m_hFile) );
		}
    }
    virtual int  Tell()
    {
		return ftell(m_hFile);
    }
    virtual BOOL Close()
    {
		Flush();
		if( m_hFile && fclose( m_hFile ) )
		{
			m_bIsError = 1;
            RtCoreLog().Error( "RtArchiveFileReader: fclose Failed: pError=%i\n", ferror(m_hFile) );
		}
        m_hFile = NULL;
		return !m_bIsError;
    }
    virtual void Flush()
    {
        fflush(m_hFile);
    }
    virtual long Serialize( void* V, int iLength )
    {
        long lResult = (long)fwrite(V, 1, iLength, m_hFile);
        RtCoreFile().AddWriteByte(lResult);
        if (lResult<iLength)
        {
			m_bIsError = 1;
            RtCoreLog().Error( "RtArchiveFileReader: fwrite Failed: Length=%i pError=%i\n", iLength, ferror(m_hFile) );
        }
        return lResult;
    }

protected:
    FILE*           m_hFile;
};

/*
class RtArchiveFileConsoleOutput : public RtArchive
{
public:
    RtArchiveFileConsoleOutput( FILE* InFile)
    {
        if (m_iBufferCount==0)
        {
            m_pBuffer = new char[1024];
        }
        m_iBufferCount ++;
    }
    virtual ~RtArchiveFileConsoleOutput()
    {
        m_iBufferCount --;
        if (m_iBufferCount==0)
        {
            DEL_ARRAY(m_pBuffer);
        }
    }
    virtual void Seek( int InPos, SeekMode eSeek )    {}
    virtual int  Tell()                               { return 0; }
    virtual BOOL Close()                              { return TRUE; }
    virtual void Flush()                              {}
    virtual long Serialize( void* V, int iLength )
    {
        if (iLength>1020) iLength = 1020;
        memcpy(m_pBuffer, V, iLength);
        m_pBuffer[iLength] = 0;
        printf(m_pBuffer);
        return iLength;
    }

protected:
    static char* m_pBuffer;
    static int   m_iBufferCount;
};
*/

class RtFileManagerAnsi : public RtFileManagerGeneric
{
public:
    BOOL FileExist( const char* pFilename )
    {
        if(access(pFilename , 0)== -1) return FALSE;
        return TRUE;
    }

    RtArchive* CreateFileReader(const char* pFilename, DWORD dwFlags) {
        // 检查文件名是否为 "stdin"，如果是，则创建一个 RtArchiveFileReader 对象读取标准输入
        if (stricmp(pFilename, "stdin") == 0) {
            // 创建 RtArchiveFileReader 对象读取标准输入，设置缓冲区大小为 1024
            return RT_NEW RtArchiveFileReader(stdin, 1024);
        } else {
            char szFlag[3] = "rb";  // 文件打开模式，"rb" 表示以二进制模式读取文件
            // 打开文件
            FILE* fp = fopen(pFilename, szFlag);
            if (!fp)  // 如果文件打开失败
            {
                // 如果未设置 "FR_NoFail" 标志，则记录错误信息
                if ((dwFlags & RtFileManager::FR_NoFail) == 0) {
                    RtCoreLog().Error("RtFileManagerAnsi: fopen Failed: %s\n", pFilename);
                }
                return NULL;  // 返回 NULL 表示打开文件失败
            }
            // 移动文件指针到文件末尾，以确定文件大小
            fseek(fp, 0, SEEK_END);
            // 创建 RtArchiveFileReader 对象以读取文件，并返回该对象
            return RT_NEW RtArchiveFileReader(fp, ftell(fp));
        }
    }

	RtArchive* CreateFileWriter( const char* pPackageName, const char* pFilename, DWORD dwFlags)
	{
        if (stricmp(pFilename, "stdout")==0)
        {
            //freopen("CONOUT$","w+t",stdout);
            return RT_NEW RtArchiveFileWriter(stdout);
            //return new RtArchiveFileConsoleOutput(NULL, pError);
        }else
        {
		    if( dwFlags & RtFileManager::FW_EvenIfReadOnly )
			    chmod(pFilename, rtsys_fmode_S_IREAD | rtsys_fmode_S_IWRITE);
		    if( (dwFlags & RtFileManager::FW_NoReplaceExisting) && FileSize(pFilename)>=0 )
			    return NULL;
		    char szMode[3] = "wb";
            if (dwFlags&RtFileManager::FW_Append) szMode[0] = 'a';
            if (dwFlags&RtFileManager::FW_Text) szMode[1] = 't';
		    FILE* fp = fopen(pFilename,szMode);
		    if( !fp )
		    {
			    if( (dwFlags & RtFileManager::FW_NoFail)==0 )
                {
                    RtCoreLog().Error("RtFileManagerAnsi: fopen Failed: %s\n", pFilename);
                }
			    return NULL;
		    }
		    if( dwFlags & RtFileManager::FW_Unbuffered )
			    setvbuf( fp, 0, _IONBF, 0 );
		    return RT_NEW RtArchiveFileWriter(fp);
        }
	}
	BOOL Delete( const char* pFilename, BOOL bRequireExists=0, BOOL bEvenReadOnly=0 )
	{
        if( bEvenReadOnly )
            chmod(pFilename, rtsys_fmode_S_IREAD | rtsys_fmode_S_IWRITE);
#ifdef WIN32
		return unlink(pFilename)==0 || (errno==ENOENT && !bRequireExists);
#else
        return unlink(pFilename)==0;
#endif
	}

    BOOL SetFilePermission( const char* pFilename, DWORD dwPermission )
    {
        int pmode = 0;
        if (dwPermission&RtFileManager::FP_READ)  pmode |= rtsys_fmode_S_IREAD;
        if (dwPermission&RtFileManager::FP_WRITE) pmode |= rtsys_fmode_S_IWRITE;
        return chmod(pFilename, pmode)==0;
    }

    DWORD GetFilePermission( const char* pFilename )
    {
        rtsys_struct_stat buf;
        DWORD dwPermission = 0;
        if (rtsys_stat(pFilename, &buf)==0)
        {
            if (buf.st_mode & rtsys_fmode_S_IREAD)  dwPermission |= RtFileManager::FP_READ;
            if (buf.st_mode & rtsys_fmode_S_IWRITE) dwPermission |= RtFileManager::FP_WRITE;
        }
        return dwPermission;
    }

	SQWORD GetCreateTime( const char* pFilename )
	{
        rtsys_struct_stat buf;
        if (rtsys_stat(pFilename, &buf)==0)
        {
            return buf.st_ctime;
        }
        return -1;
	}
	BOOL SetCreateTime( const char* pFilename )
	{
		return FALSE;
	}
    SQWORD GetAccessTime( const char* pFilename )
    {
        rtsys_struct_stat buf;
        if (rtsys_stat(pFilename, &buf)==0)
        {
            return buf.st_atime;
        }
        return -1;
    }
    BOOL SetAccessTime( const char* pFilename )
    {
        return FALSE;
    }
    SQWORD GetModifyTime( const char* pFilename )
    {
        rtsys_struct_stat buf;
        if (rtsys_stat(pFilename, &buf)==0)
        {
            return buf.st_mtime;
        }
        return -1;
    }
    BOOL SetModifyTime( const char* pFilename )
    {
        return FALSE;
    }
	BOOL MakeDirectory( const char* pPath, BOOL bTree=0 )
	{
        if( bTree )
            return RtFileManagerGeneric::MakeDirectory( pPath, bTree );
#ifdef WIN32
		return mkdir(pPath)==0 || errno==EEXIST;
#else
        int mode = S_IRWXU | S_IRWXG; // 应该设置成0777
        return mkdir(pPath, mode)==0 || errno==EEXIST;
#endif
	}
	BOOL DeleteDirectory( const char* pPath, BOOL bRequireExists=0, BOOL bTree=0 )
	{
		if( bTree )
			return RtFileManagerGeneric::DeleteDirectory( pPath, bRequireExists, bTree );
		return rmdir(pPath)==0 || (errno==ENOENT && !bRequireExists);
	}
	std::list<std::string> FindFiles( const char* pFilename, BOOL bFiles, BOOL bDirectories )
	{
		std::list<std::string> Result;
        BOOL isDir;
#ifdef WIN32
        RtString szPathname = pFilename;
        if (pFilename[0])
        {
            if (rtChrInStr(PATH_SEPARATOR, szPathname[(int)(strlen(pFilename)-1)])==-1)
            {
                szPathname += PATH_SEPARATOR_S;
            }
        }
        szPathname += "*.*";
		_finddata_t Found;
        long hFind = _findfirst( szPathname.c_str(), &Found );
		if( hFind != -1 )
		{
			do
			{
				const char* Name = Found.name;
                isDir = (Found.attrib & _A_SUBDIR);
                if (isDir)
                {
                    if (!bDirectories) continue;
                    if (strcmp(Name,".")==0 || strcmp(Name,"..")==0) continue;
                }else
                {
                    if (!bFiles) continue;
                }
                Result.push_back(std::string(Name));
			}while( _findnext( hFind, &Found )!=-1 );
		}
		_findclose(hFind);
#else
        int nLen = strlen(pFilename);
        DIR *pDir;
        rtsys_struct_stat sbuf;
        if(nLen==0) pDir = opendir(".");
        else        pDir = opendir(pFilename);
        if (pDir!=NULL)
        {
            DIRECT *dd;
            while(dd = readdir(pDir))
            {
                if ((rtsys_stat(dd->d_name, &sbuf))<0) continue;
                isDir = S_ISDIR(sbuf.st_mode);
                if (isDir)
                {
                    if (!bDirectories) continue;
                    if (strcmp(dd->d_name,".")==0 || strcmp(dd->d_name,"..")==0) continue;
                }else
                {
                    if (!bFiles) continue;
                }
				Result.push_back(/*RtString*/std::string(dd->d_name));
            }
        }
#endif
		return Result;
	}
	BOOL SetDefaultDirectory( const char* pFilename )
	{
		return chdir(pFilename)==0;
	}
	RtString GetDefaultDirectory()
	{
		char szBuffer[1024]="";
		getcwd( szBuffer, countof(szBuffer) );
		return szBuffer;
	}
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
