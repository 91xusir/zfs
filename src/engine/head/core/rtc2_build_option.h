/**
* @file rtc2_build_option.h 编译配置选项.
*  如果用户要修改编译配置，可以在这里修改，但是必须重新编译所有的代码才能应用到库中
* @author janhail
*/

#ifndef DO_CHECK
#   define DO_CHECK 1
#endif

// 效率统计开关.
#ifndef DO_STAT
#   define DO_STAT 1
#endif

 //控制台开关.
 #ifndef DO_CONSOLE
 #   define DO_CONSOLE 1
 #endif

#   define  ENABLE_MEMORY_ALLOCATOR_DEBUG
#   define  ENABLE_LOG

/*
#ifdef _DEBUG
#   define  ENABLE_MEMORY_ALLOCATOR_DEBUG
#   define  ENABLE_LOG
#endif
*/

#define RTE_FILE_VERSION    1
#define RTE_CORE_VERSION    1

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
