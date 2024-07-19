
#include "scene/rt_scene.h"
#include "scene/rts_SceneManager.h"

#define   DEFAULT_GRIDWIDTH   ( 500.0f )

inline bool  Box2DIntersection( float xMin0 , float xMax0 , float yMin0 , float yMax0 ,
							    float xMin1 , float xMax1 , float yMin1 , float yMax1 )
{
	if( xMax0 < xMin1 )  return false;
	if( yMax0 < yMin1 )  return false;
	if( xMin0 > xMax1 )  return false;
	if( yMin0 > yMax1 )  return false;

	if( xMax1 < xMin0 )  return false;
	if( yMax1 < yMin0 )  return false;
	if( xMin1 > xMax0 )  return false;
	if( yMin1 > yMax0 )  return false;

	return true;
}

static int nCurrentX = 0;
static int nCurrentY = 0;

static int nLastX    = 0;
static int nLastY    = 0;


RtSceneGrid::RtSceneGrid( )
{

}
RtSceneGrid::~RtSceneGrid( )
{
	for( size_t t = 0 ; t < mkaObjectArray.size( ) ; t ++ )
	{
        mkaObjectArray[ t ]->miRefConter --;
		if( mkaObjectArray[ t ]->miRefConter <= 0 )
		{
			mkaObjectArray[ t ]->mpkObject->ReleaseResource();
			DEL_ONE( mkaObjectArray[ t ] );
		}
	}
	mkaObjectArray.clear( );
}
RtSceneManager::RtSceneManager()
              :mfGridWidth( DEFAULT_GRIDWIDTH ),
			   miDeleteIdex( 0 ),
			   miNewGridIndex( 0 )
{
    for( int i = 0 ; i < SIXTEEN ; i ++ )
	{
		mkSceneGridCache[ i ]  = NULL;
		mkSceneNeedDelete[ i ] = NULL;
		mkSceneNewGrid[ i ]    = NULL;
	}
}

RtSceneManager::~RtSceneManager( )
{
    for( int i = 0 ; i < SIXTEEN ; i ++ )
	{
		if( mkSceneGridCache[ i ] )
		{
			DEL_ONE( mkSceneGridCache[ i ] );
		}
	}
}


void RtSceneManager::SetCurrentPosition(float x , float y )
{
	nLastX    = nCurrentX;
	nLastY    = nCurrentY;
	nCurrentX = x;
	nCurrentY = y;
    for( int i = 0 ; i < THREE ; i ++ )
	{
		for( int j = 0 ; j < THREE ; j ++ )
		{
			float xReal = x + ( j - 1 ) * mfGridWidth;
			float yReal = y + ( i - 1 ) * mfGridWidth;
			mkSceneGrid[ i ][ j ] = GetCurrentSceneGrid( xReal , yReal );
		}
	}
}

RtSceneGrid* RtSceneManager::GetCurrentSceneGrid( float x , float y )
{	
	int nFirstNull = SIXTEEN;
	for (int i = 0; i < SIXTEEN; i ++) 
	{
		if (( mkSceneGridCache[i] != NULL ) )
		{
			if(( mkSceneGridCache[i]->mfMinX <= x ) && 
			   ( mkSceneGridCache[i]->mfMaxX >= x ) && 
			   ( mkSceneGridCache[i]->mfMinY <= y ) && 
			   ( mkSceneGridCache[i]->mfMaxY >= y ))
			{
				return mkSceneGridCache[i];
			}
		}
			
		if (mkSceneGridCache[i] == 0 && i < nFirstNull) 
		{
			nFirstNull = i;
		}
	}
	
	if ( nFirstNull == SIXTEEN )
	{
		int nScore    = 0;
		int nMaxScore = 0;
		int nMaxIdx   = 0;		
		for (int i=0; i< SIXTEEN; i++) 
		{
			float xCenter = mkSceneGridCache[i]->mfMaxX + mkSceneGridCache[i]->mfMinX;
			float yCenter = mkSceneGridCache[i]->mfMaxY + mkSceneGridCache[i]->mfMinY;
			xCenter      *= 0.5f;			
			yCenter      *= 0.5f;

			nScore = abs( xCenter - x ) + abs( yCenter - y );
			if (nScore>nMaxScore) 
			{
				nMaxScore = nScore;
				nMaxIdx   = i;
			}
		}		
		mkSceneNeedDelete[ miDeleteIdex ++ ] = mkSceneGridCache[nMaxIdx];
		nFirstNull      = nMaxIdx;
	}
	float  fX =  (int)( x / mfGridWidth ); 	
	float  fY =  (int)( y / mfGridWidth );
	mkSceneGridCache[nFirstNull]        = RT_NEW RtSceneGrid;	

	mkSceneNewGrid[ miNewGridIndex ++ ] = mkSceneGridCache[nFirstNull];

	float&  fXMin = mkSceneGridCache[nFirstNull]->mfMinX;
	float&  fYMin = mkSceneGridCache[nFirstNull]->mfMinY;
	float&  fXMax = mkSceneGridCache[nFirstNull]->mfMaxX;
	float&  fYMax = mkSceneGridCache[nFirstNull]->mfMaxY;
	if( x < 0 )
	{
        fXMax = fX * mfGridWidth;
		fXMin = ( fX - 1.0f )  * mfGridWidth;
	}else
	{
		fXMin = fX * mfGridWidth;
		fXMax = ( fX + 1.0f )  * mfGridWidth;
	}

	if( y < 0 )
	{
		fYMax = fY * mfGridWidth;
		fYMin = ( fY - 1 )  * mfGridWidth;
	}else
	{
		fYMin = fY * mfGridWidth;
		fYMax = ( fY + 1 )  * mfGridWidth;
	}
	return mkSceneGridCache[nFirstNull];
}

void  RtSceneManager::AddObjectToSceneManager( RtsModel* pkModel )
{
    if( pkModel )
	{
		float xMin = pkModel->Get_AABBox()->vPos.x - pkModel->Get_AABBox()->vExt.x;
		float yMin = pkModel->Get_AABBox()->vPos.y - pkModel->Get_AABBox()->vExt.y;
		float xMax = pkModel->Get_AABBox()->vPos.x + pkModel->Get_AABBox()->vExt.x;
		float yMax = pkModel->Get_AABBox()->vPos.y + pkModel->Get_AABBox()->vExt.y;
		bool  bHasFindOwner       = false;
		RtsModelWrapper* pkInsert = NULL;
		for( int i = 0 ; i < SIXTEEN ; i ++ )
		{
			
            if( mkSceneGridCache[ i ] )
			{
				bool bIntersect = Box2DIntersection( mkSceneGridCache[ i ]->mfMinX ,
					                                 mkSceneGridCache[ i ]->mfMaxX ,
													 mkSceneGridCache[ i ]->mfMinY ,
													 mkSceneGridCache[ i ]->mfMaxY ,
													 xMin ,
													 xMax ,
													 yMin ,
													 yMax);
				if( bIntersect )
				{
					if( pkInsert == NULL )
					{
						pkInsert              = RT_NEW RtsModelWrapper;
						pkInsert->miRefConter = 0;
						pkInsert->mpkObject   = pkModel;
					}
					pkInsert->miRefConter ++;
                    mkSceneGridCache[ i ]->mkaObjectArray.push_back( pkInsert );
					bHasFindOwner = true;						
				}
			}
		}
		///!绝对不会到这个地方，但是有的时候会到这个地方，很奇怪。
		if( bHasFindOwner == false )
		{
			if( mkSceneGrid[ 0 ][ 0 ] != NULL )
			{
				if( pkInsert == NULL )
				{
					pkInsert              = RT_NEW RtsModelWrapper;
					pkInsert->miRefConter = 0;
					pkInsert->mpkObject   = pkModel;
				}
				mkSceneGrid[ 0 ][ 0 ]->mkaObjectArray.push_back( pkInsert );
			}
		}
	}
}

void RtSceneManager::TransformGridToAnotherGrid( RtSceneGrid* pkScr , RtSceneGrid* pkDest )
{
	if( NULL == pkScr || NULL == pkDest ) return;
	RtsModelWrapperArray&  kScrArray  = pkScr->mkaObjectArray;
	RtsModelWrapperArray&  kDestArray = pkDest->mkaObjectArray;		

	for( size_t t = 0 ; t < kScrArray.size( ) ; t ++ )
	{
		RtsModelWrapper* pkCurrent = kScrArray[ t ];
		if( NULL == pkCurrent ) continue;
		RtsModel* pkModel          = pkCurrent->mpkObject;
		if( NULL == pkModel   ) continue;
		float xMin = pkModel->Get_AABBox()->vPos.x - pkModel->Get_AABBox()->vExt.x;
		float yMin = pkModel->Get_AABBox()->vPos.y - pkModel->Get_AABBox()->vExt.y;
		float xMax = pkModel->Get_AABBox()->vPos.x + pkModel->Get_AABBox()->vExt.x;
		float yMax = pkModel->Get_AABBox()->vPos.y + pkModel->Get_AABBox()->vExt.y;
		bool bIntersect = Box2DIntersection( pkDest->mfMinX ,
					                         pkDest->mfMaxX ,
											 pkDest->mfMinY ,
											 pkDest->mfMaxY ,
											 xMin ,
											 xMax ,
											 yMin ,
											 yMax);

		if( bIntersect )
		{
			pkCurrent->miRefConter ++;
            pkDest->mkaObjectArray.push_back( pkCurrent );
		}
	}   
}

void  RtSceneManager::AfterSceneMove( )
{	int i;
    for( i = 0 ; i < miDeleteIdex ; i ++ )
	{
		for( int j = 0 ; j < miNewGridIndex ; j ++ )
		{
            TransformGridToAnotherGrid( mkSceneNeedDelete[ i ] , mkSceneNewGrid[ j ] );
		}		
	}
	if( mkSceneNeedDelete[ i ] )
	{
		DEL_ONE( mkSceneNeedDelete[ i ] ) ;
		mkSceneNeedDelete[ i ] = NULL;
	}
	miNewGridIndex = 0;
	miDeleteIdex   = 0;
}