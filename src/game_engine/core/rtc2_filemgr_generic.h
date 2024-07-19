
namespace rt2_core {

class RtFileManagerGeneric : public RtFileManager
{
public:
	int FileSize( const char* pFilename )
	{
		RtArchive* pAr = CreateFileReader( pFilename );
		if (!pAr) return -1;
		int Result = pAr->TotalSize();
		delete pAr;
		return Result;
	}
	BOOL Copy( const char* pPackageName, const char* pDestFile, const char* pSrcFile, BOOL bReplaceExisting, BOOL bEvenIfReadOnly, BOOL bAttributes, void (*fnProgress)(FLOAT fFraction) )
	{
		BOOL bSuccess = 0;
		if (fnProgress) fnProgress( 0.0f );
		RtArchive* pSrc = CreateFileReader( pSrcFile );
		if( pSrc )
		{
			int iSize = pSrc->TotalSize();
            RtArchive* pDest = CreateFileWriter( pPackageName, pDestFile, (bReplaceExisting?0:RtFileManager::FW_NoReplaceExisting) | (bEvenIfReadOnly?RtFileManager::FW_EvenIfReadOnly:0) );
			if( pDest )
			{
				static BYTE szBuffer[4096];
				for( int Total=0; Total<iSize; Total+=sizeof(szBuffer) )
				{
					int Count = Min( iSize-Total, (int)sizeof(szBuffer) );
					pSrc->Serialize( szBuffer, Count );
					if( pSrc->IsError() )
						break;
					pDest->Serialize( szBuffer, Count );
					if( pDest->IsError() )
						break;
					if( fnProgress )
						fnProgress( (FLOAT)Total / iSize );
				}
				bSuccess = pDest->Close();
				delete pDest;
				if( !bSuccess )
					Delete( pDestFile );
			}
			bSuccess = bSuccess && pSrc->Close();
			delete pSrc;
		}
		if( fnProgress )
			fnProgress( 1.0 );
		return bSuccess;
	}
	BOOL MakeDirectory( const char* pPath, BOOL bTree=0 )
	{
		CHECK(bTree);
		int iSlashCount=0, iCreateCount=0;
		for( char Full[256]="", *Ptr=Full; ; *Ptr++=*pPath++ )
		{
			if( rtChrInStr(PATH_SEPARATOR, *pPath)!=-1 || *pPath==0 )
			{
				if( iSlashCount++>0 && !IsDrive(Full) )
				{
					*Ptr = 0;
					if( !MakeDirectory( Full, 0 ) )
						return 0;
					iCreateCount++;
				}
			}
			if( *pPath==0 )
				break;
		}
		return iCreateCount!=0;
	}
	BOOL DeleteDirectory( const char* pPath, BOOL bRequireExists=0, BOOL bTree=0 )
	{
		CHECK(bTree);
		if( !strlen(pPath) )
			return FALSE;
		RtString szSpec = pPath/*RtString(pPath) * "*"*/;
        std::list<std::string> List = FindFiles( szSpec.c_str(), 1, 0 );
        std::list<std::string>::iterator it;
		for( it=List.begin(); it!=List.end(); it++ )
        {
            if( !Delete(RtString(pPath)*(*it).c_str(),1,1) )
				return FALSE;
        }
        List = FindFiles( szSpec.c_str(), 0, 1 );
		for( it=List.begin(); it!=List.end(); it++ )
        {
			if( !DeleteDirectory(RtString(pPath)*(*it).c_str(),1,1) )
				return FALSE;
        }
		return DeleteDirectory( pPath, bRequireExists, 0 );
	}
	BOOL Move( const char* pPackageName, const char* Dest, const char* Src, BOOL ReplaceExisting=1, BOOL EvenIfReadOnly=0, BOOL Attributes=0 )
	{
		if( !Copy(pPackageName,Dest,Src,ReplaceExisting,EvenIfReadOnly,Attributes,NULL) )
			return FALSE;
		Delete( Src, 1, 1 );
		return TRUE;
	}
private:
	BOOL IsDrive( const char* pPath )
	{
		if( stricmp(pPath,"")==0 )
			return TRUE;
		else if( rtToUpper(pPath[0])!=rtToLower(pPath[0]) && pPath[1]==':' && pPath[2]==0 )
			return TRUE;
		else if( stricmp(pPath,"\\")==0 )
			return TRUE;
		else if( stricmp(pPath,"\\\\")==0 )
			return TRUE;
		else if( pPath[0]=='\\' && pPath[1]=='\\' && !strchr(pPath+2,'\\') )
			return TRUE;
		else if( pPath[0]=='\\' && pPath[1]=='\\' && strchr(pPath+2,'\\') && !strchr(strchr(pPath+2,'\\')+1,'\\') )
			return TRUE;
		else
			return FALSE;
	}
};

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
