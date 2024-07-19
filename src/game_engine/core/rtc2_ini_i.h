#ifndef AZOTH_INI_PARSER_H
#define AZOTH_INI_PARSER_H

#include "rtc2_script_comm.h"

#define	INI_STRING	258
#define	INI_BOOL	259
#define	INI_LONG	260
#define	INI_DOUBLE	261
#define	INI_ENCAPSULATED_STRING	262
#define	INI_SECTION	263

#define AZOTH_INI_ENTRY     1
#define AZOTH_INI_SECTION   2

BEGIN_EXTERN_C()

typedef void (*TypeCallbackAzothIniParser)(azoth_value *arg1, azoth_value *arg2, int type, void *arg);
typedef void (*TypeCallbackAzothIniError)(const char* szFilename, const char* szErrMsg, long lLineNo, void *arg);

typedef struct struct_azoth_ini_parser_param
{
	TypeCallbackAzothIniParser  callbackParser;
    TypeCallbackAzothIniError   callbackError;
	void *arg;
    azoth_bool ignore_errors;
} s_azoth_ini_parser_param;

typedef struct struct_azoth_parser
{
	struct struct_azoth_ini_parser_param*   ini_parser_param;
} s_azoth_parser;

int AzothParseIniFile(const char* szFilename, TypeCallbackAzothIniError callbackIniError, TypeCallbackAzothIniParser callbackParserIni, void *arg);

int     azini_lex(azoth_value *pIniVal);
void    azini_scanner_init();
int     azini_scanner_get_lineno();
char*   azini_scanner_get_filename();
int     azini_open_file_for_scanning(s_azoth_file* pFile);
void    azini_close_file(s_azoth_file* pFile);

END_EXTERN_C()

#endif // AZOTH_INI_PARSER_H
