//**************************************************************************************************
//
// profile.cpp
//
// Real-Time Hierarchical Profiling for Game Programming Gems 3
//
// by Greg Hjelstrom & Byon Garrabrant
//
//*************************************************************************************************/

#ifdef WIN32
    #include <windows.h>
	#include "filepack/profile.h"

inline void Profile_Get_Ticks(_int64 * ticks)
{
	__asm
	{
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
		_emit 31h
		mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
	}
}

inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;
	
	if (_CPUFrequency == -1.0f) {
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 
	
	return _CPUFrequency;
}

/***************************************************************************************************
**
** CProfileNode
**
***************************************************************************************************/

/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - pointer to a static string which is the name of this profile node                    *
 * parent - parent pointer                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The name is assumed to be a static pointer, only the pointer is stored and compared for     *
 * efficiency reasons.                                                                         *
 *=============================================================================================*/
CProfileNode::CProfileNode( const char * name, CProfileNode * parent ) :
	Name( name ),
	TotalCalls( 0 ),
	TotalTime( 0 ),
	StartTime( 0 ),
	RecursionCounter( 0 ),
	Parent( parent ),
	Child( NULL ),
	Sibling( NULL )
{
	Reset();
}


CProfileNode::~CProfileNode( void )
{
	DEL_ONE(Child);
	DEL_ONE(Sibling);
}


/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - static string pointer to the name of the node we are searching for                   *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All profile names are assumed to be static strings so this function uses pointer compares   *
 * to find the named node.                                                                     *
 *=============================================================================================*/
CProfileNode * CProfileNode::Get_Sub_Node( const char * name )
{
	// Try to find this sub node
	CProfileNode * child = Child;
	while ( child ) {
		if ( child->Name == name ) {
			return child;
		}
		child = child->Sibling;
	}

	// We didn't find it, so add it
	CProfileNode * node = RT_NEW CProfileNode( name, this );
	node->Sibling = Child;
	Child = node;
	return node;
}

int CProfileNode::Get_Child_Count()
{
    int iCount = 0;
    // 
    CProfileNode * child = Child;
    while ( child ) 
    {
        iCount ++;
        child = child->Sibling;
    }
    
    return iCount;
}

CProfileNode* CProfileNode::Get_Sub_Node(int iNo)
{
    int count = iNo;

    CProfileNode * child = Child;
    while ( child ) 
    {
        if( count==0 )
        {
            return child;
        }
        count--;
        child = child->Sibling;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Function		: CProfileNode::Get_Time_Per_Call
// Description	: 平均每次调用占用的时间(ms)
// Return type	: float 
// Comments		: lasted modified by eMean Liu 2003-9-16 15:15:08
//-----------------------------------------------------------------------------
float CProfileNode::Get_Time_Per_Call()
{
    float fTime = 0.00f;

    if( Get_Total_Calls()>0 )
        fTime = Get_Total_Time()/Get_Total_Calls();

    return fTime;
}

//-----------------------------------------------------------------------------
// Function		: CProfileNode::Get_Percent_of_Parent
// Description	: 占用父节点时间的百分比
// Return type	: float 
// Comments		: lasted modified by eMean Liu 2003-9-16 15:14:48
//-----------------------------------------------------------------------------
float CProfileNode::Get_Percent_of_Parent()
{
    float fPer = 100.0f;
    CProfileNode *p = Get_Parent();
    if( p )
    {
        if( p->Get_Total_Time()>1)
            fPer = 100*Get_Total_Time() / p->Get_Total_Time();
        if( fPer>100.0f)
            fPer = 100.0f;
    }
    
    return fPer;
}

//-----------------------------------------------------------------------------
// Function		: CProfileNode::Get_Percent_of_Total
// Description	: 占用总时间的百分比
// Return type	: float 
// Comments		: lasted modified by eMean Liu 2003-9-16 15:14:43
//-----------------------------------------------------------------------------
float CProfileNode::Get_Percent_of_Total()
{
    float fPer = 100.0f;

    CProfileIterator *it = CProfileManager::Get_Iterator();
    it->First();
    CProfileNode *p = it->CurrentChild;
    if( p )
    {
        if( p->Get_Total_Time()>1)
            fPer = 100*Get_Total_Time() / p->Get_Total_Time();
        if( fPer>100.0f)
            fPer = 100.0f;
    }
    
    return fPer;
}

void	CProfileNode::Reset( void )
{
	TotalCalls = 0;
	TotalTime = 0.0f;

	if ( Child ) {
		Child->Reset();
	}
	if ( Sibling ) {
		Sibling->Reset();
	}
}


void	CProfileNode::Call( void )
{
	TotalCalls++;
	if (RecursionCounter++ == 0) 
    {
        ::QueryPerformanceCounter(&tStartTime);
//		Profile_Get_Ticks(&StartTime);
	}
}


bool	CProfileNode::Return( void )
{
	if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
/*
		__int64 time;
		Profile_Get_Ticks(&time);
		time-=StartTime;
*/
        LARGE_INTEGER time;
        ::QueryPerformanceCounter(&time);
        float t = (float)(time.QuadPart - tStartTime.QuadPart);
		TotalTime += 1000*t / Profile_Get_Tick_Rate();
	}
	return ( RecursionCounter == 0 );
}


/***************************************************************************************************
**
** CProfileIterator
**
***************************************************************************************************/
CProfileIterator::CProfileIterator( CProfileNode * start )
{
	CurrentParent = start;
	CurrentChild = CurrentParent->Get_Child();
}


void	CProfileIterator::First(void)
{
	CurrentChild = CurrentParent->Get_Child();
}


void	CProfileIterator::Next(void)
{
	CurrentChild = CurrentChild->Get_Sibling();
}


bool	CProfileIterator::Is_Done(void)
{
	return CurrentChild == NULL;
}


void	CProfileIterator::Enter_Child( int index )
{
	CurrentChild = CurrentParent->Get_Child();
	while ( (CurrentChild != NULL) && (index != 0) ) {
		index--;
		CurrentChild = CurrentChild->Get_Sibling();
	}

	if ( CurrentChild != NULL ) {
		CurrentParent = CurrentChild;
		CurrentChild = CurrentParent->Get_Child();
	}
}


void	CProfileIterator::Enter_Parent( void )
{
	if ( CurrentParent->Get_Parent() != NULL ) {
		CurrentParent = CurrentParent->Get_Parent();
	}
	CurrentChild = CurrentParent->Get_Child();
}


/***************************************************************************************************
**
** CProfileManager
**
***************************************************************************************************/

CProfileNode	CProfileManager::Root( "Root", NULL );
CProfileNode *	CProfileManager::CurrentNode = &CProfileManager::Root;
int				CProfileManager::FrameCounter = 0;
__int64			CProfileManager::ResetTime = 0;


/***********************************************************************************************
 * CProfileManager::Start_Profile -- Begin a named profile                                    *
 *                                                                                             *
 * Steps one level deeper into the tree, if a child already exists with the specified name     *
 * then it accumulates the profiling; otherwise a RT_NEW child node is added to the profile tree. *
 *                                                                                             *
 * INPUT:                                                                                      *
 * name - name of this profiling record                                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The string used is assumed to be a static string; pointer compares are used throughout      *
 * the profiling code for efficiency.                                                          *
 *=============================================================================================*/
void	CProfileManager::Start_Profile( const char * name )
{
	if (name != CurrentNode->Get_Name()) {
		CurrentNode = CurrentNode->Get_Sub_Node( name );
	} 
	
	CurrentNode->Call();
}


/***********************************************************************************************
 * CProfileManager::Stop_Profile -- Stop timing and record the results.                       *
 *=============================================================================================*/
void	CProfileManager::Stop_Profile( void )
{
	// Return will indicate whether we should back up to our parent (we may
	// be profiling a recursive function)
	if (CurrentNode->Return()) {
		CurrentNode = CurrentNode->Get_Parent();
	}
}


/***********************************************************************************************
 * CProfileManager::Reset -- Reset the contents of the profiling system                       *
 *                                                                                             *
 *    This resets everything except for the tree structure.  All of the timing data is reset.  *
 *=============================================================================================*/
void	CProfileManager::Reset( void )
{ 
	Root.Reset(); 
	FrameCounter = 0;
	Profile_Get_Ticks(&ResetTime);
}


/***********************************************************************************************
 * CProfileManager::Increment_Frame_Counter -- Increment the frame counter                    *
 *=============================================================================================*/
void CProfileManager::Increment_Frame_Counter( void )
{
	FrameCounter++;
}


/***********************************************************************************************
 * CProfileManager::Get_Time_Since_Reset -- returns the elapsed time since last reset         *
 *=============================================================================================*/
float CProfileManager::Get_Time_Since_Reset( void )
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= ResetTime;
	return (float)time / Profile_Get_Tick_Rate();
}


#endif
