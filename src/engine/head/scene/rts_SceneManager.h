
#ifndef  __SceneManager_H__
#define  __SceneManager_H__

class RtsModel;



struct RtsModelWrapper
{	
public:
	RtsModel*  mpkObject;
	int        miRefConter;
};

typedef std::vector<RtsModelWrapper*>  RtsModelWrapperArray;

class RtSceneGrid
{
public:
	RtSceneGrid( );
   ~RtSceneGrid( );
public:
	float  mfMinX;
	float  mfMinY;
	float  mfMaxX;
	float  mfMaxY;	
	RtsModelWrapperArray  mkaObjectArray;
};

class  RtSceneManager
{
public:
	enum
	{
		THREE   = 3 ,
		NINE    = 9 ,
		SIXTEEN = 16
	};
public:
	RtSceneManager( );
   ~RtSceneManager( );
public:
   void  SetCurrentPosition( float x , float y );
   void  AddObjectToSceneManager( RtsModel* );
   void  AfterSceneMove();
private:
	RtSceneGrid*  GetCurrentSceneGrid( float x , float y );	
	void          TransformGridToAnotherGrid( RtSceneGrid* , RtSceneGrid* );
private:
	float         mfGridWidth;
	int           miDeleteIdex;
	int           miNewGridIndex;
	RtSceneGrid*  mkSceneGridCache[ SIXTEEN ];
	RtSceneGrid*  mkSceneNeedDelete[ SIXTEEN ];
	RtSceneGrid*  mkSceneNewGrid[ SIXTEEN ];
	RtSceneGrid*  mkSceneGrid[ THREE ][ THREE ];
};

#endif