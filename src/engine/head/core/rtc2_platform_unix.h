/**
* @file rtc2_platform_unix.h ��Unixƽ̨��صĶ��壬����Linux,FreeBSD��
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
-   ������ת�� (��һЩ��ͬƽ̨����ͬ���ܣ��������ֲ�ͬ�ĺ���������ת��)
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
-   �������Ͷ���
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
-   ����ϵͳ����ַ�������
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
-   �̣߳��ź�����ز���
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

/** �ź�����ʹ��˵��.
    ��������������ܺ��ź����йص�һЩ���������Ƕ���ͷ�ļ�/usr/include/semaphore.h�ж���
    �ź�����������һ���Ǹ��������������������������ƶԹ�����Դ�ķ��ʡ���������Դ����ʱ�����ú���
      sem_post()�����ź�����ֻ�е��ź���ֵ����0ʱ������ʹ�ù�����Դ��ʹ�ú󣬺���sem_wait()����
      �ź���������sem_trywait()�ͺ���pthread_ mutex_trylock()��ͬ�������ã����Ǻ���sem_wait()��
      �������汾��
    �ź�������������Ϊ�ṹsem_t������������һ�������͵���������sem_init()������ʼ��һ���ź�����
      ����ԭ��Ϊ�� int sem_init(sem_t *sem, int pshared, unsigned int value) semΪָ���ź����ṹ
      ��һ��ָ�룻pshared��Ϊ0ʱ���ź����ڽ��̼乲������ֻ��Ϊ��ǰ���̵������̹߳���value��
      �����ź����ĳ�ʼֵ��
    ����sem_post( sem_t *sem )���������ź�����ֵ�������߳�����������ź�����ʱ���������������ʹ
      ���е�һ���̲߳���������ѡ�����ͬ�������̵߳ĵ��Ȳ��Ծ����ġ�
    ����sem_wait( sem_t *sem )������������ǰ�߳�ֱ���ź���sem��ֵ����0�����������sem��ֵ��һ��
      ����������Դ��ʹ�ú���١�
    ����sem_trywait ( sem_t *sem )�Ǻ���sem_wait�����ķ������汾����ֱ�ӽ��ź���sem��ֵ��һ��
    ����sem_destroy(sem_t *sem)�����ͷ��ź���sem��
    ����int sem_getvalue(sem_t * sem, int * sval)���Ի�ȡ�ź�����ֵ
    ע�⣺Linuxֻʵ�������ź���������sem_init�Ĳ���psharedֻ�ܴ�0�����򽫷���-1
    sem_wait()��ʵ��Ϊȡ���㣬������֧��ԭ��"�Ƚ��ҽ���"ָ�����ϵ�ṹ�ϣ�sem_post()��Ψһ��������
       ���źŴ�������POSIX�첽�źŰ�ȫ��API��
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
#   define RT_TRYLOCK_CS(p)                    (sem_trywait(p)!=11 /*��Ϊ Linux ��֧�� ���԰�EAGAIN �ĳ�11*/)
#   define RT_LOCK_CS(p)                       sem_wait(p)
#   define RT_UNLOCK_CS(p)                     sem_post(p)
#endif

/** �߳�����ʹ��˵��.
   ������Posix Thread��ͬ������ʹ��IPC���ź���������ʵ�ֻ�����mutex���ܣ�����Ȼsemphore�Ĺ��ܹ���ǿ���ˣ���
       Posix Thread�ж���������һ��ר�������߳�ͬ����mutex������
   ����������
       �����ַ�����������������̬��ʽ�Ͷ�̬��ʽ��POSIX������һ����PTHREAD_MUTEX_INITIALIZER����̬��ʼ������
           �����������£� pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER; ��LinuxThreadsʵ���У�pthread_mutex_t
           ��һ���ṹ����PTHREAD_MUTEX_INITIALIZER����һ���ṹ������
       ��̬��ʽ�ǲ���pthread_mutex_init()��������ʼ����������API�������£�
           int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) 
           ����mutexattr����ָ�����������ԣ����£������ΪNULL��ʹ��ȱʡ���ԡ�
       pthread_mutex_destroy()����ע��һ����������API�������£�
           int pthread_mutex_destroy(pthread_mutex_t *mutex) 
           ����һ������������ζ���ͷ�����ռ�õ���Դ����Ҫ������ǰ���ڿ���״̬��������Linux�У�����������ռ��
           �κ���Դ�����LinuxThreads�е�pthread_mutex_destroy()���˼����״̬���⣨����״̬�򷵻�EBUSY��û��
           ����������
   ����������
       �������������ڴ�������ʱ��ָ������LinuxThreadsʵ���н���һ�����������ԣ���ͬ������������ͼ��һ���Ѿ���
           �����Ļ���������ʱ���ֲ�ͬ����ǰ��glibc2.2.3,linuxthreads0.9�����ĸ�ֵ�ɹ�ѡ��
         PTHREAD_MUTEX_TIMED_NP������ȱʡֵ��Ҳ������ͨ������һ���̼߳����Ժ��������������߳̽��γ�һ���ȴ�
             ���У����ڽ��������ȼ�����������������Ա�֤����Դ����Ĺ�ƽ�ԡ� 
         PTHREAD_MUTEX_RECURSIVE_NP��Ƕ����������ͬһ���̶߳�ͬһ�����ɹ���ö�Σ���ͨ�����unlock���������
             �ǲ�ͬ�߳��������ڼ����߳̽���ʱ���¾����� 
         PTHREAD_MUTEX_ERRORCHECK_NP������������ͬһ���߳�����ͬһ�������򷵻�EDEADLK��������PTHREAD_MUTEX_TIMED_NP
             ���Ͷ�����ͬ�������ͱ�֤���������μ���ʱ��������������µ������� 
         PTHREAD_MUTEX_ADAPTIVE_NP����Ӧ����������򵥵������ͣ����ȴ����������¾�����
   ������
       ��������Ҫ��������pthread_mutex_lock()������pthread_mutex_unlock()�Ͳ��Լ���pthread_mutex_trylock()������
           �����������͵������������ܱ�������ͬ���߳�ͬʱ�õ���������ȴ�������������ͨ������Ӧ�����ͣ������߿�
           ����ͬ�������κ��̣߳��������������ɼ����߽�������Ч�����򷵻�EPERM������Ƕ�������ĵ���ʵ��Ҫ�����
           �ɼ����߽�������ʵ����������û���������ƣ������ͬĿǰ��û�еõ����͡���ͬһ�����е��̣߳��������
           ��û�н��������κ������̶߳��޷��ٻ������
         int pthread_mutex_lock(pthread_mutex_t *mutex)
         int pthread_mutex_unlock(pthread_mutex_t *mutex)
         int pthread_mutex_trylock(pthread_mutex_t *mutex)
         pthread_mutex_trylock()������pthread_mutex_lock()���ƣ���ͬ���������Ѿ���ռ��ʱ����EBUSY�����ǹ���ȴ���
   ����
       POSIX�߳������Ƶ�Linuxʵ�ֶ�����ȡ���㣬��ˣ��ӳ�ȡ�����͵��̲߳������յ�ȡ���źŶ��뿪�����ȴ���ֵ��ע��
           ���ǣ�����߳��ڼ��������ǰ��ȡ����������Զ��������״̬���������ڹؼ���������ȡ������ڣ�����������
           �첽ȡ�����ͣ���������˳��ص������н�����
       ���������ͬʱҲ�����첽�źŰ�ȫ�ģ�Ҳ����˵����Ӧ�����źŴ��������ʹ�û������������������������
   ʹ��˵��
       �ṹpthread_mutex_tΪ���������������ͣ����а���һ��ϵͳ��������Զ��󡣺���pthread_mutex_init��������һ������
           ����NULL��������ʹ��Ĭ�����ԡ������Ҫ�����ض����ԵĻ�����������ú���pthread_mutexattr_init������
           pthread_mutexattr_setpshared�ͺ���pthread_mutexattr_settype�������û��������ԡ�ǰһ��������������pshared��
           ��������ȡֵ��PTHREAD_PROCESS_PRIVATE��PTHREAD_PROCESS_SHARED��ǰ��������ͬ�����е��߳�ͬ������������ͬ��
           �����̵Ĳ�ͬ�̡߳���һ�������������û��������ͣ���ѡ��������PTHREAD_MUTEX_NORMAL��PTHREAD_MUTEX_ERRORCHECK��
           PTHREAD_MUTEX_RECURSIVE��PTHREAD _MUTEX_DEFAULT�����Ƿֱ����˲�ͬ���������������ƣ�һ������£�ѡ�����
           һ��Ĭ�����ԡ�
       pthread_mutex_lock������ʼ�û������������˺�Ĵ���ֱ������pthread_mutex_unlockΪֹ��������������ͬһʱ��ֻ�ܱ�
           һ���̵߳���ִ�С���һ���߳�ִ�е�pthread_mutex_lock��ʱ�����������ʱ����һ���߳�ʹ�ã��Ǵ��̱߳���������
           ���򽫵ȴ�����һ���߳��ͷŴ˻�����������ʹ����pthread_delay_np���������߳�˯��һ��ʱ�䣬����Ϊ�˷�ֹһ����
           ��ʼ��ռ�ݴ˺�����
       ��Ҫ���������ʹ�û������Ĺ����к��п��ܻ���������������߳���ͼͬʱռ��������Դ��������ͬ�Ĵ���������Ӧ�Ļ���
           �������������̶߳���Ҫ����������1�ͻ�����2��a�߳�������������1��b�߳�������������2����ʱ�ͳ�������������ʱ
           ���ǿ���ʹ�ú���pthread_mutex_trylock�����Ǻ���pthread_mutex_lock�ķ������汾�����������������ɱ���ʱ����
           �᷵����Ӧ����Ϣ������Ա�����������������Ӧ�Ĵ������ⲻͬ�Ļ��������Ͷ������Ĵ���һ����������Ҫ�Ļ���
           Ҫ����Ա�Լ��ڳ������ע����һ�㡣
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

/** ԭ�Ӳ�����ʹ��˵��.
    ԭ�Ӳ�����ҪӲ����֧�֣�����Ǽܹ���صģ���API��ԭ�����͵Ķ��嶼�������ں�Դ������include/asm/atomic.h�ļ���
    ԭ�����Ͷ�������
      typedef struct
      {
          volatile int counter; // volatile�����ֶθ���gcc��Ҫ�Ը����͵��������Ż����������ķ��ʶ��Ƕ��ڴ�ķ��ʣ������ǶԼĴ����ķ���
      } atomic_t;
    atomic_read(atomic_t * v);
        �ú�����ԭ�����͵ı�������ԭ�Ӷ�������������ԭ�����͵ı���v��ֵ��
    atomic_set(atomic_t * v, int i);
        �ú�������ԭ�����͵ı���v��ֵΪi��
    void atomic_add(int i, atomic_t *v);
        �ú�����ԭ�����͵ı���v����ֵi��
    void atomic_sub(int i, atomic_t *v);
        �ú�����ԭ�����͵ı���v�м�ȥi��
    int atomic_sub_and_test(int i, atomic_t *v);
        �ú�����ԭ�����͵ı���v�м�ȥi�����жϽ���Ƿ�Ϊ0�����Ϊ0�������棬���򷵻ؼ١�
    void atomic_inc(atomic_t *v);
        �ú�����ԭ�����ͱ���vԭ�ӵ�����1��
    void atomic_dec(atomic_t *v);
        �ú�����ԭ�����͵ı���vԭ�ӵؼ�1�� 
    int atomic_dec_and_test(atomic_t *v);
        �ú�����ԭ�����͵ı���vԭ�ӵؼ�1�����жϽ���Ƿ�Ϊ0�����Ϊ0�������棬���򷵻ؼ١�
    int atomic_inc_and_test(atomic_t *v);
        �ú�����ԭ�����͵ı���vԭ�ӵ�����1�����жϽ���Ƿ�Ϊ0�����Ϊ0�������棬���򷵻ؼ١�
    int atomic_add_negative(int i, atomic_t *v);
        �ú�����ԭ�����͵ı���vԭ�ӵ�����I�����жϽ���Ƿ�Ϊ����������ǣ������棬���򷵻ؼ١�
    int atomic_add_return(int i, atomic_t *v);
        �ú�����ԭ�����͵ı���vԭ�ӵ�����i�����ҷ���ָ��v��ָ�롣
    int atomic_sub_return(int i, atomic_t *v);
        �ú�����ԭ�����͵ı���v�м�ȥi�����ҷ���ָ��v��ָ�롣
    int atomic_inc_return(atomic_t * v);
        �ú�����ԭ�����͵ı���vԭ�ӵ�����1���ҷ���ָ��v��ָ�롣 
    int atomic_dec_return(atomic_t * v);
        �ú�����ԭ�����͵ı���vԭ�ӵؼ�1���ҷ���ָ��v��ָ�롣
    ԭ�Ӳ���ͨ��ֻ�����ں�ģʽ�����û����������һ������
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
