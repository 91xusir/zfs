//*************************************************************************************************
//
// profile.h
//
// Real-Time Hierarchical Profiling for Game Programming Gems 3
//
// by Greg Hjelstrom & Byon Garrabrant
//
//*************************************************************************************************/


//A node in the Profile Hierarchy Tree

#ifndef PROFILE_H
#define PROFILE_H
#include <core/rt2_core.h>
#include "profile_result.h"

#ifdef WIN32
	//#define PROFILE_DEBUG//是否需要报告效率
    const int REPORT_STYLE=NORMAL_REPORT;//普通模式
    //const int REPORT_STYLE = COMPACT_REPORT;//紧凑模式
#endif


#ifdef PROFILE_DEBUG
//报告
#define	PROFILE( name )			CProfileSample __profile( name )
#define EXPORT_PROFILE			CProfileResult::Instance()->ExportToXML()
#else
//不报告
#define	PROFILE( name )
#define EXPORT_PROFILE
#endif


//单个对象安全释放
#ifndef DEL_ONE
#   define DEL_ONE(x)        do { if (x) { delete (x); (x)=nullptr; } } while(0)
#endif
//对象数组的分配和释放安全释放
#ifndef DEL_ARRAY
#   define DEL_ARRAY(x)      do { if (x) { delete[] (x); (x)=nullptr; } } while(0)
#endif



#ifdef WIN32


class	CProfileNode {
    friend class CProfileResult;
public:
    CProfileNode( const char * name, CProfileNode * parent );
    ~CProfileNode( void );
    
    CProfileNode * Get_Sub_Node( const char * name );
    
    CProfileNode * Get_Parent( void )		{ return Parent; }
    CProfileNode * Get_Sibling( void )		{ return Sibling; }
    CProfileNode * Get_Child( void )		{ return Child; }
    
    void			Reset( void );
    void			Call( void );
    bool			Return( void );
    // 
    const char *	Get_Name( void )		{ return Name; }
    int				Get_Total_Calls( void )	{ return TotalCalls; }
    float			Get_Total_Time( void )	{ return TotalTime; }
    // 
    float           Get_Time_Per_Call();
    float           Get_Percent_of_Parent();
    float           Get_Percent_of_Total();
    int             Get_Child_Count();
    CProfileNode*   Get_Sub_Node(int iNo);
protected:
    
    const char*	    Name;
    int				TotalCalls;
    float			TotalTime;
    __int64			StartTime;   
    LARGE_INTEGER   tStartTime;
    int				RecursionCounter;
    
    CProfileNode*	Parent;
    CProfileNode*	Child;
    CProfileNode*	Sibling;
};

/*
** An iterator to navigate through the tree
*/
class CProfileIterator
{
    friend class CProfileResult;
    friend class CProfileNode;
public:
    // Access all the children of the current parent
    void				First(void);
    void				Next(void);
    bool				Is_Done(void);
    
    void				Enter_Child( int index );		// Make the given child the RT_NEW parent
    void				Enter_Largest_Child( void );	// Make the largest child the RT_NEW parent
    void				Enter_Parent( void );			// Make the current parent's parent the RT_NEW parent
    
    // Access the current child
    const char *	    Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
    int				    Get_Current_Total_Calls( void )	    { return CurrentChild->Get_Total_Calls(); }
    float				Get_Current_Total_Time( void )	    { return CurrentChild->Get_Total_Time(); }
    
    // Access the current parent
    const char *	    Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
    int				    Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
    float				Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }
    
protected:
    
    CProfileNode *	    CurrentParent;
    CProfileNode *	    CurrentChild;
    
    CProfileIterator( CProfileNode * start );
    friend	class		CProfileManager;
};


/*
** The Manager for the Profile system
*/
class	CProfileManager {
    friend class CProfileResult;
public:
    static	void				Start_Profile( const char * name );
    static	void				Stop_Profile( void );
    
    static	void				Reset( void );
    static	void				Increment_Frame_Counter( void );
    static	int					Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
    static	float				Get_Time_Since_Reset( void );
    
    static	CProfileIterator *	Get_Iterator( void )	{ return RT_NEW CProfileIterator( &Root ); }
    static	void				Release_Iterator(CProfileIterator* iterator) { DEL_ONE(iterator); }
    
private:
    static	CProfileNode		Root;
    static	CProfileNode *		CurrentNode;
    static	int					FrameCounter;
    static	__int64				ResetTime;
};


/*
** ProfileSampleClass is a simple way to profile a function's scope
** Use the PROFILE macro at the start of scope to time
*/
class	CProfileSample {
public:
    CProfileSample( const char * name )
    { 
        CProfileManager::Start_Profile( name ); 
    }
    
    ~CProfileSample( void )					
    { 
        CProfileManager::Stop_Profile(); 
    }
};




#endif//win32

#endif//PROFILE_H


