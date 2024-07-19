#include "core/rt2_core.h"
#include "rtc2_script_comm.h"

namespace rt2_core {

BEGIN_EXTERN_C()

int* ArchiveOpen(char* pFilename, int bRead)
{
    return (int*)(RtCoreFile().CreateFileReader(pFilename, 0));
}

void ArchiveClose(int* pArc)
{
    RtCoreFile().CloseFile((RtArchive*)pArc);
}

int ArchiveRead(void* pBuf, int iSize, int* pArc)
{
    return ((RtArchive*)pArc)->Serialize(pBuf, iSize);
}

struct SMemoryColoct
{
	char*  pkChar;	
};

typedef std::vector<SMemoryColoct>              SMemoryColoctArray;
typedef std::vector<SMemoryColoct>::iterator    SMemoryIterator;
SMemoryColoctArray*   gpMemoryArray = NULL;

void releaseAllBuffer( )
{
	if( gpMemoryArray )
	{
		for( SIZE_T i = 0 ; i < (*gpMemoryArray).size() ; i ++ )
		{
			RtCoreMem().Free( (*gpMemoryArray)[ i ].pkChar );		
		}
		(*gpMemoryArray).clear();
	}
	//delete gpMemoryArray;
	DEL_ONE( gpMemoryArray );
	
}
char *azoth_strndup(const char *s, unsigned int length)
{
    char *p;

    p = (char *)RtCoreMem().Alloc(length+1, __FILE__, __LINE__);// RT_NEW char[ length+1 ];
    if (!p)
    {
        return (char *)NULL;
    }
    if (length)
    {
        memcpy(p, s, length);
    }
    p[length] = 0;

	SMemoryColoct kInsert;
	kInsert.pkChar = p;

	if( NULL == gpMemoryArray )
	{
		gpMemoryArray = RT_NEW SMemoryColoctArray;
		(*gpMemoryArray).clear();
	}
	if( gpMemoryArray )
	{
        (*gpMemoryArray).push_back( kInsert );	
	}
	
    return p;
}

void azoth_freestr(char *s)
{
	if( gpMemoryArray )
	{
		SMemoryIterator  kIterator = (*gpMemoryArray).begin();
		for( kIterator ; kIterator != (*gpMemoryArray).end() ; kIterator ++ )
		{
			if( (*kIterator).pkChar == s )
			{
				(*gpMemoryArray).erase( kIterator );
				break;
			}
		}    
	}    
	
	if( s )
	{
		RtCoreMem().Free( s );
		/*delete s;
		s = NULL;*/
	}
}
END_EXTERN_C()

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
