/**
* @file rtc2_platform_unix.h 和Unix平台相关的定义，包括Linux,FreeBSD等
* @author janhail
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

// linux header file
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>
#ifndef FREE_BSD
#include <sys/io.h>
#endif
#ifdef CYGWIN
	#include <dirent.h>
#else
	#include <sys/dir.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>


// Unsigned base types.
typedef unsigned char       BYTE;       // 8-bit  unsigned.
typedef unsigned short      WORD;       // 16-bit unsigned.
typedef unsigned int        UINT;       // 32-bit unsigned.
typedef unsigned long       DWORD;      // 32-bit unsigned.
typedef unsigned long long  QWORD;      // 64-bit unsigned.

// Signed base types.
typedef signed char         SBYTE;      // 8-bit  signed.
typedef signed short        SWORD;      // 16-bit signed.
typedef signed int          INT;        // 32-bit signed.
typedef signed long long    SQWORD;     // 64-bit signed.

// Other base types.
typedef signed int          BOOL;       // Boolean 0 (false) or 1 (true).
typedef float               FLOAT;      // 32-bit IEEE floating point.
typedef double              DOUBLE;     // 64-bit IEEE double.
typedef unsigned long       SIZE_T;     // Corresponds to C SIZE_T.

#ifndef	VOID
#define	VOID				void
typedef char				CHAR;
typedef short				SHORT;
typedef long				LONG;
#endif



typedef unsigned char byte;
typedef byte cs_byte;
typedef unsigned char boolean;

typedef long				HRESULT;

typedef void*               RTHINSTANCE;// HINSTANCE
typedef void*               RTHWND;     // HWND

typedef long long           RTTIME64;
typedef pthread_t           RtThreadHandle;
typedef LONG				HRESULT;

#undef	NULL
#undef	TRUE
#undef	FALSE
#define	BOOL		long
#define	NULL		0
#define	TRUE		1
#define	FALSE		0
#define	LONG		long
#define	ULONG		DWORD
#define	TCHAR		char
// #define CDECL

#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

#define __FUNCTION__ "unsupported"


#	define S_OK                             0x00000000L
#	define E_FAIL                           0x80004005L


/*-----------------------------------------------------------------------------
-   函数名转换 (把一些不同平台有相同功能，但是名字不同的函数名进行转换)
-----------------------------------------------------------------------------*/

#define     strcmpi                 strcasecmp
#define     stricmp                 strcasecmp
#define     strnicmp                strncasecmp

#ifdef FREE_BSD
#   define EXT_SPACE std
#else
#   define EXT_SPACE __gnu_cxx
#endif

inline char *strupr(char *s)
{
	char *beg = s;
	while(*s)
	{
		if(*s >= 'a' && *s <= 'z')
			*s = *s - ('a' - 'A');
		s++;
	}
	return beg;
}

inline char *strlwr(char *s)
{
	char *beg = s;
	while(*s)
	{
		if(*s >= 'A' && *s <= 'Z')
			*s = *s + ('a' - 'A');
		s++;
	}
	return beg;
}

inline char *strrev(char *s)
{
	char *b = s;
	while(*s++);
	char *e = s--;
	char tmp;
	while(b > e)
	{
		tmp = *b;
		*b++ = *e;
		*e-- = tmp;
	}
	return s;  
}

/*-----------------------------------------------------------------------------
-   函数类型定义
-----------------------------------------------------------------------------*/
#undef STDCALL
#define DLL_IMPORT          
#define DLL_EXPORT          
#define DLL_EXPORT_CLASS    
#define VARARGS										/* Functions with variable arguments */
#define CDECL               __cdecl                 /* Standard C function */
#define STDCALL             __stdcall               /* Standard calling convention */
#define FORCEINLINE         __forceinline           /* Force code to be inline */
#define ZEROARRAY                                   /* Zero-length arrays in structs */
#define APICALL             _far _pascal            /* Win api function */
#define RT_NOVTABLE			__declspec(novtable)

#define HANDLE void*
#define HINSTANCE void*

#define HASH_STR(key,val) EXT_SPACE::unordered_map<key,val,hash_str>
#define HASH_MULTISTR(key,val) EXT_SPACE::hash_multimap<key,val,hash_str>
#define HASH_QWORD(key,val) EXT_SPACE::unordered_map<key,val,hash_qword>
#define HASH_MULTIQWORD(key,val) EXT_SPACE::hash_multimap<key,val,hash_qword>
#define Sleep(s)	sleep(s)

//#define max(a,b)    (((a) > (b)) ? (a) : (b))
//#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define itoa(i,c,dec)	sprintf(c,"%d",i)

#define __TEXT(quote) quote
#define TEXT(quote) __TEXT(quote)


namespace rt2_core {

/*-----------------------------------------------------------------------------
-   操作系统相关字符串常量
-----------------------------------------------------------------------------*/
#define RT_COMPILER "GCC"
#ifdef FREE_BSD
#   define OS_TYPE_STR     "FreeBSD"
#else
#   define OS_TYPE_STR     "Linux"
#endif
#define LINE_TERMINATOR     "\r\n"
#define PATH_SEPARATOR      "/\\"
#define PATH_SEPARATOR_S    "/"
#define PATH_SEPARATOR_C    '/'

/*-----------------------------------------------------------------------------
-   线程，信号量相关操作
-----------------------------------------------------------------------------*/
#include <pthread.h>
#include <semaphore.h>

#ifdef FREE_BSD
#   define RT_CREATE_THREAD(func,arg)           false
#   define RT_CREATE_THREAD_RET(ret)            ((ret)!=0)
#   define RT_TERMINATE_THREAD(vHandle, vCode)  false
#else
#   define RT_CREATE_THREAD(func,arg)           pthread_create(&_pthreadid,NULL,func,(void*)arg)
#   define RT_CREATE_THREAD_RET(ret)            ((ret)!=0)
#   define RT_TERMINATE_THREAD(vHandle, vCode)  pthread_cancel(vHandle)
#endif

/** 信号量的使用说明.
    下面我们逐个介绍和信号量有关的一些函数，它们都在头文件/usr/include/semaphore.h中定义
    信号量本质上是一个非负的整数计数器，它被用来控制对公共资源的访问。当公共资源增加时，调用函数
      sem_post()增加信号量。只有当信号量值大于0时，才能使用公共资源，使用后，函数sem_wait()减少
      信号量。函数sem_trywait()和函数pthread_ mutex_trylock()起同样的作用，它是函数sem_wait()的
      非阻塞版本。
    信号量的数据类型为结构sem_t，它本质上是一个长整型的数。函数sem_init()用来初始化一个信号量。
      它的原型为： int sem_init(sem_t *sem, int pshared, unsigned int value) sem为指向信号量结构
      的一个指针；pshared不为0时此信号量在进程间共享，否则只能为当前进程的所有线程共享；value给
      出了信号量的初始值。
    函数sem_post( sem_t *sem )用来增加信号量的值。当有线程阻塞在这个信号量上时，调用这个函数会使
      其中的一个线程不在阻塞，选择机制同样是由线程的调度策略决定的。
    函数sem_wait( sem_t *sem )被用来阻塞当前线程直到信号量sem的值大于0，解除阻塞后将sem的值减一，
      表明公共资源经使用后减少。
    函数sem_trywait ( sem_t *sem )是函数sem_wait（）的非阻塞版本，它直接将信号量sem的值减一。
    函数sem_destroy(sem_t *sem)用来释放信号量sem。
    函数int sem_getvalue(sem_t * sem, int * sval)可以获取信号量的值
    注意：Linux只实现无名信号量，所以sem_init的参数pshared只能传0，否则将返回-1
    sem_wait()被实现为取消点，而且在支持原子"比较且交换"指令的体系结构上，sem_post()是唯一能用于异
       步信号处理函数的POSIX异步信号安全的API。
*/
#ifdef FREE_BSD
#   define RT_TYPE_CS                          int
#   define RT_INIT_CS(p)                       
#   define RT_DELETE_CS(p)                     
#   define RT_TRYLOCK_CS(p)                       
#   define RT_LOCK_CS(p)                       
#   define RT_UNLOCK_CS(p)                     
#else
#   define RT_TYPE_CS                          sem_t
#   define RT_INIT_CS(p)                       sem_init(p,0,1)
#   define RT_DELETE_CS(p)                     sem_destroy(p)
#   define RT_TRYLOCK_CS(p)                    (sem_trywait(p)!=11 /*因为 Linux 不支持 所以把EAGAIN 改成11*/)
#   define RT_LOCK_CS(p)                       sem_wait(p)
#   define RT_UNLOCK_CS(p)                     sem_post(p)
#endif

/** 线程锁的使用说明.
   尽管在Posix Thread中同样可以使用IPC的信号量机制来实现互斥锁mutex功能，但显然semphore的功能过于强大了，在
       Posix Thread中定义了另外一套专门用于线程同步的mutex函数。
   创建和销毁
       有两种方法创建互斥锁，静态方式和动态方式。POSIX定义了一个宏PTHREAD_MUTEX_INITIALIZER来静态初始化互斥
           锁，方法如下： pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER; 在LinuxThreads实现中，pthread_mutex_t
           是一个结构，而PTHREAD_MUTEX_INITIALIZER则是一个结构常量。
       动态方式是采用pthread_mutex_init()函数来初始化互斥锁，API定义如下：
           int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) 
           其中mutexattr用于指定互斥锁属性（见下），如果为NULL则使用缺省属性。
       pthread_mutex_destroy()用于注销一个互斥锁，API定义如下：
           int pthread_mutex_destroy(pthread_mutex_t *mutex) 
           销毁一个互斥锁即意味着释放它所占用的资源，且要求锁当前处于开放状态。由于在Linux中，互斥锁并不占用
           任何资源，因此LinuxThreads中的pthread_mutex_destroy()除了检查锁状态以外（锁定状态则返回EBUSY）没有
           其他动作。
   互斥锁属性
       互斥锁的属性在创建锁的时候指定，在LinuxThreads实现中仅有一个锁类型属性，不同的锁类型在试图对一个已经被
           锁定的互斥锁加锁时表现不同。当前（glibc2.2.3,linuxthreads0.9）有四个值可供选择
         PTHREAD_MUTEX_TIMED_NP，这是缺省值，也就是普通锁。当一个线程加锁以后，其余请求锁的线程将形成一个等待
             队列，并在解锁后按优先级获得锁。这种锁策略保证了资源分配的公平性。 
         PTHREAD_MUTEX_RECURSIVE_NP，嵌套锁，允许同一个线程对同一个锁成功获得多次，并通过多次unlock解锁。如果
             是不同线程请求，则在加锁线程解锁时重新竞争。 
         PTHREAD_MUTEX_ERRORCHECK_NP，检错锁，如果同一个线程请求同一个锁，则返回EDEADLK，否则与PTHREAD_MUTEX_TIMED_NP
             类型动作相同。这样就保证当不允许多次加锁时不会出现最简单情况下的死锁。 
         PTHREAD_MUTEX_ADAPTIVE_NP，适应锁，动作最简单的锁类型，仅等待解锁后重新竞争。
   锁操作
       锁操作主要包括加锁pthread_mutex_lock()、解锁pthread_mutex_unlock()和测试加锁pthread_mutex_trylock()三个，
           不论哪种类型的锁，都不可能被两个不同的线程同时得到，而必须等待解锁。对于普通锁和适应锁类型，解锁者可
           以是同进程内任何线程；而检错锁则必须由加锁者解锁才有效，否则返回EPERM；对于嵌套锁，文档和实现要求必须
           由加锁者解锁，但实验结果表明并没有这种限制，这个不同目前还没有得到解释。在同一进程中的线程，如果加锁
           后没有解锁，则任何其他线程都无法再获得锁。
         int pthread_mutex_lock(pthread_mutex_t *mutex)
         int pthread_mutex_unlock(pthread_mutex_t *mutex)
         int pthread_mutex_trylock(pthread_mutex_t *mutex)
         pthread_mutex_trylock()语义与pthread_mutex_lock()类似，不同的是在锁已经被占据时返回EBUSY而不是挂起等待。
   其他
       POSIX线程锁机制的Linux实现都不是取消点，因此，延迟取消类型的线程不会因收到取消信号而离开加锁等待。值得注意
           的是，如果线程在加锁后解锁前被取消，锁将永远保持锁定状态，因此如果在关键区段内有取消点存在，或者设置了
           异步取消类型，则必须在退出回调函数中解锁。
       这个锁机制同时也不是异步信号安全的，也就是说，不应该在信号处理过程中使用互斥锁，否则容易造成死锁。
   使用说明
       结构pthread_mutex_t为不公开的数据类型，其中包含一个系统分配的属性对象。函数pthread_mutex_init用来生成一个互斥
           锁。NULL参数表明使用默认属性。如果需要声明特定属性的互斥锁，须调用函数pthread_mutexattr_init。函数
           pthread_mutexattr_setpshared和函数pthread_mutexattr_settype用来设置互斥锁属性。前一个函数设置属性pshared，
           它有两个取值，PTHREAD_PROCESS_PRIVATE和PTHREAD_PROCESS_SHARED。前者用来不同进程中的线程同步，后者用于同步
           本进程的不同线程。后一个函数用来设置互斥锁类型，可选的类型有PTHREAD_MUTEX_NORMAL、PTHREAD_MUTEX_ERRORCHECK、
           PTHREAD_MUTEX_RECURSIVE和PTHREAD _MUTEX_DEFAULT。它们分别定义了不同的上所、解锁机制，一般情况下，选用最后
           一个默认属性。
       pthread_mutex_lock声明开始用互斥锁上锁，此后的代码直至调用pthread_mutex_unlock为止，均被上锁，即同一时间只能被
           一个线程调用执行。当一个线程执行到pthread_mutex_lock处时，如果该锁此时被另一个线程使用，那此线程被阻塞，即
           程序将等待到另一个线程释放此互斥锁。可以使用了pthread_delay_np函数，让线程睡眠一段时间，就是为了防止一个线
           程始终占据此函数。
       需要提出的是在使用互斥锁的过程中很有可能会出现死锁：两个线程试图同时占用两个资源，并按不同的次序锁定相应的互斥
           锁，例如两个线程都需要锁定互斥锁1和互斥锁2，a线程先锁定互斥锁1，b线程先锁定互斥锁2，这时就出现了死锁。此时
           我们可以使用函数pthread_mutex_trylock，它是函数pthread_mutex_lock的非阻塞版本，当它发现死锁不可避免时，它
           会返回相应的信息，程序员可以针对死锁做出相应的处理。另外不同的互斥锁类型对死锁的处理不一样，但最主要的还是
           要程序员自己在程序设计注意这一点。
*/

#define RtThreadMutexType                       pthread_mutex_t
#define RtThreadMutexInit(p)                    pthread_mutex_init(p, NULL)
#define RtThreadMutexDestroy(p)                 pthread_mutex_destroy(p)
#define RtThreadMutexLock(p)                    pthread_mutex_lock(p)
#define RtThreadMutexUnlock(p)                  pthread_mutex_unlock(p)

#define RtProcessMutexType                      pthread_mutex_t
#define RtProcessMutexInit(p)                   pthread_mutex_init(p, NULL)
#define RtProcessMutexDestroy(p)                pthread_mutex_destroy(p)
#define RtProcessMutexLock(p)                   pthread_mutex_lock(p)
#define RtProcessMutexUnlock(p)                 pthread_mutex_unlock(p)

/** 原子操作的使用说明.
    原子操作需要硬件的支持，因此是架构相关的，其API和原子类型的定义都定义在内核源码树的include/asm/atomic.h文件中
    原子类型定义如下
      typedef struct
      {
          volatile int counter; // volatile修饰字段告诉gcc不要对该类型的数据做优化处理，对它的访问都是对内存的访问，而不是对寄存器的访问
      } atomic_t;
    atomic_read(atomic_t * v);
        该函数对原子类型的变量进行原子读操作，它返回原子类型的变量v的值。
    atomic_set(atomic_t * v, int i);
        该函数设置原子类型的变量v的值为i。
    void atomic_add(int i, atomic_t *v);
        该函数给原子类型的变量v增加值i。
    void atomic_sub(int i, atomic_t *v);
        该函数从原子类型的变量v中减去i。
    int atomic_sub_and_test(int i, atomic_t *v);
        该函数从原子类型的变量v中减去i，并判断结果是否为0，如果为0，返回真，否则返回假。
    void atomic_inc(atomic_t *v);
        该函数对原子类型变量v原子地增加1。
    void atomic_dec(atomic_t *v);
        该函数对原子类型的变量v原子地减1。 
    int atomic_dec_and_test(atomic_t *v);
        该函数对原子类型的变量v原子地减1，并判断结果是否为0，如果为0，返回真，否则返回假。
    int atomic_inc_and_test(atomic_t *v);
        该函数对原子类型的变量v原子地增加1，并判断结果是否为0，如果为0，返回真，否则返回假。
    int atomic_add_negative(int i, atomic_t *v);
        该函数对原子类型的变量v原子地增加I，并判断结果是否为负数，如果是，返回真，否则返回假。
    int atomic_add_return(int i, atomic_t *v);
        该函数对原子类型的变量v原子地增加i，并且返回指向v的指针。
    int atomic_sub_return(int i, atomic_t *v);
        该函数从原子类型的变量v中减去i，并且返回指向v的指针。
    int atomic_inc_return(atomic_t * v);
        该函数对原子类型的变量v原子地增加1并且返回指向v的指针。 
    int atomic_dec_return(atomic_t * v);
        该函数对原子类型的变量v原子地减1并且返回指向v的指针。
    原子操作通常只用在内核模式，被用户包含后会有一个警告
*/

#define RtResLockType                       RT_TYPE_CS
#define RtResLockInit(p)                    RT_INIT_CS(p)
#define RtResLockDestroy(p)                 RT_DELETE_CS(p)
#define RtResLockLock(p)                    RT_LOCK_CS(p)
#define RtResLockTryLock(p)                 RT_TRYLOCK_CS(p)
#define RtResLockUnlock(p)                  RT_UNLOCK_CS(p)

inline long RtInterlockedIncrement(long *plValue)
{
	(*plValue)++;
    return *plValue;
}

inline long RtInterlockedDecrement(long *plValue)
{
	(*plValue)--;
    return *plValue;
}

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
