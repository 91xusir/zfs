#ifndef _RT_SCRIPT_COMMON_H_
#define _RT_SCRIPT_COMMON_H_

#define AZOTH_VERSION "0.0.1"

#ifdef __cplusplus
#define BEGIN_EXTERN_C() extern "C" {
#define END_EXTERN_C() }
#else
#define BEGIN_EXTERN_C()
#define END_EXTERN_C()
#endif

BEGIN_EXTERN_C()

/* azoth types */
typedef unsigned char   azoth_bool;
typedef unsigned char   azoth_uchar;
typedef unsigned int    azoth_uint;
typedef unsigned long   azoth_ulong;
typedef unsigned short  azoth_ushort;

/* data types */
#define IS_NULL		0
#define IS_LONG		1
#define IS_DOUBLE	2
#define IS_STRING	3
#define IS_BOOL		6

#define IS_ARRAY	4
#define IS_OBJECT	5
#define IS_RESOURCE	7
#define IS_CONSTANT	8
#define IS_CONSTANT_ARRAY	9

typedef union _azoth_value_value
{
    long    lval;       /* long value */
    double  dval;       /* double value */
    struct
    {
        char*   val;
        int     len;
    } str;              /* string value */
} azoth_value_value;

typedef struct _azoth_value
{
    azoth_value_value   value;		/* value */
    azoth_uchar         type;	    /* active type */
    azoth_uchar         is_ref;
    azoth_ushort        refcount;
} azoth_value;

#define AZ_SCANNER(v)  (gScanner.v)
#define AZ_PARSER(v)   (gParser.v)

typedef struct struct_azoth_scanner
{
    FILE*   yy_in;
    FILE*   yy_out;
    int     yy_leng;
    char*   yy_text;

    struct  yy_buffer_state *current_buffer;
    char*   c_buf_p;
    int     init;
    int     start;
    char    _yy_hold_char;
    int     yy_n_chars;
    int     _yy_did_buffer_switch_on_eof;

    /* Must be of the same type as yy_state_type, *
    * if for whatever reason it's no longer int! */
    int     _yy_last_accepting_state;
    char*   _yy_last_accepting_cpos;
    int     _yy_more_flag;
    int     _yy_more_len;

    int     yy_start_stack_ptr;
    int     yy_start_stack_depth;
    int*    yy_start_stack;
} s_azoth_scanner;

typedef struct struct_azoth_file
{
    int         bFile;          /* 1 File, 0 Stream */
    char*       szStream;       /* */
    int*        pArc;
    char*       szFilename;
    int         bInitFilename;
} s_azoth_file;

int* ArchiveOpen(char* pFilename, int bRead);
void ArchiveClose(int* pArc);
int ArchiveRead(void* pBuf, int iSize, int* pArc);



void releaseAllBuffer();
char *azoth_strndup(const char *s, unsigned int length);
void azoth_freestr(char *s);
END_EXTERN_C()

#endif  /* _RT_SCRIPT_COMMON_H_ */

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
