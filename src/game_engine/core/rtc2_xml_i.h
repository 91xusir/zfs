#ifndef AZOTH_XML_PARSER_H
#define AZOTH_XML_PARSER_H

#include "rtc2_script_comm.h"

#define XML_ENDTAG_START            259
#define XML_TAG_END2                261
#define XML_ENCAPSULATED_STRING     263

#define	XML_SPACES	258
#define	XML_IDENTITY	259
#define	XML_STRING	260
#define	XML_TEXT	261
#define	XML_START	262
#define	XML_TAG_START	263
#define	XML_TAG_END	264
#define	XML_TAG_CLOSE	265
#define	XML_TAG_XEND	266
#define	XML_TAG_EMPTY	267
#define	XML_ERROR	268

BEGIN_EXTERN_C()

void azoth_error(int type, const char *format, ...);

typedef void (*CB_AzothError)(const char* szFilename, const char* szErrMsg, long lLineNo, void *arg);
typedef void (*CB_AzothXmlStart)(const char *pTag, void *arg);
typedef void (*CB_AzothXmlTagStart)(const char *pTag, void *arg);
typedef void (*CB_AzothXmlTagEnd)(void *arg);
typedef void (*CB_AzothXmlTagEndImme)(void *arg);
typedef void (*CB_AzothXmlCloseTagStart)(const char *pTag, void *arg);
typedef void (*CB_AzothXmlTagAttrib)(const char *pAttribute, const char *pValue, void *arg);
typedef void (*CB_AzothXmlItem)(char bSpace, const char *pItem, void *arg);

typedef struct struct_azoth_parser_param
{
    CB_AzothError               cbError;
    CB_AzothXmlStart            cbXmlStart;
    CB_AzothXmlTagStart         cbTagStart;
    CB_AzothXmlTagEnd           cbTagEnd;
    CB_AzothXmlTagEndImme       cbTagEndImme;
    CB_AzothXmlCloseTagStart    cbCloseTagStart;
    CB_AzothXmlTagAttrib        cbTagAttribute;
    CB_AzothXmlItem             cbItem;

	void *arg;
    azoth_bool ignore_errors;
} s_azoth_parser_param;

typedef struct struct_azoth_parser
{
	s_azoth_parser_param* parser_param;
} s_azoth_parser;

/* bFile = 1 from file, 0 from string */
int AzothParseXml(int bFile, int bHtmlCompatible, const char* szStream,
                      CB_AzothError callbackIniError,
                      CB_AzothXmlStart callbackXmlStart,
                      CB_AzothXmlTagStart cbTagStart,
                      CB_AzothXmlTagEnd cbTagEnd,
                      CB_AzothXmlTagEndImme cbTagEndImme,
                      CB_AzothXmlCloseTagStart cbCloseTagStart,
                      CB_AzothXmlTagAttrib cbTagAttribute,
                      CB_AzothXmlItem cbItem,
                      void *arg);

/* Stack */
void AzothStackPushString(const char* pString);
void AzothStackPopString();
const char* AzothStackTopString();
void AzothStackClear();
int AzothStackIsEmpty();

int     azxml_lex(azoth_value *pIniVal);
void    azxml_scanner_init();
int     azxml_scanner_get_lineno();
char*   azxml_scanner_get_filename();
int     azxml_open_file_for_scanning(s_azoth_file* pFile);
void    azxml_close_file(s_azoth_file* pFile);

END_EXTERN_C()

#endif // AZOTH_XML_PARSER_H
