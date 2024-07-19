
/*  A Bison parser, made from xml_parser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse azxml_parse
#define yylex azxml_lex
#define yyerror azxml_error
#define yylval azxml_lval
#define yychar azxml_char
#define yydebug azxml_debug
#define yynerrs azxml_nerrs
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

#line 1 "xml_parser.y"

/*********************************************************/
#define DEBUG_CFG_PARSER 0
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "rtc2_script_comm.h"
#include "rtc2_xml_i.h"

#define YYSTYPE azoth_value
#define YYERROR_VERBOSE

#ifdef WIN32
#   define  xmlStricmp      stricmp
#   define  xmlVsnprintf    _vsnprintf
#else
#   define  xmlStricmp      strcasecmp
#   define  xmlVsnprintf    vsnprintf
#endif


int azxml_lex(azoth_value *pIniVal);
int azxml_parse(void);

azoth_value yylval;
s_azoth_parser gParser;

struct SXML_STACK
{
	char* pStr;
	struct SXML_STACK* pNext;
};

static struct SXML_STACK* pHead = 0;
static int s_bHtmlCompatible = 0;

int AzothParseXml(int bFile, int bHtmlCompatible, const char* szStream,
                      CB_AzothError callbackIniError,
                      CB_AzothXmlStart callbackXmlStart,
                      CB_AzothXmlTagStart cbTagStart,
                      CB_AzothXmlTagEnd cbTagEnd,
                      CB_AzothXmlTagEndImme cbTagEndImme,
                      CB_AzothXmlCloseTagStart cbCloseTagStart,
                      CB_AzothXmlTagAttrib cbTagAttribute,
                      CB_AzothXmlItem cbItem,
                      void *arg)
{
	int retval;
	s_azoth_file file;
	s_azoth_parser_param parser_param;
	
	s_bHtmlCompatible = bHtmlCompatible;

	parser_param.cbError		    = callbackIniError;
    parser_param.cbXmlStart		    = callbackXmlStart;
	parser_param.cbTagStart		    = cbTagStart;
	parser_param.cbTagEnd		    = cbTagEnd;
	parser_param.cbTagEndImme	    = cbTagEndImme;
	parser_param.cbCloseTagStart    = cbCloseTagStart;
	parser_param.cbTagAttribute     = cbTagAttribute;
	parser_param.cbItem			    = cbItem;
	parser_param.arg			    = arg;
	AZ_PARSER(parser_param)		    = &parser_param;

	file.bFile = bFile;
	if (bFile)
	{
		file.szFilename = strdup(szStream);
		file.pArc = 0;
	}else
	{
		file.szFilename = strdup("String");
		file.szStream = (char*)szStream;
	}
	file.bInitFilename = 1;

	retval = azxml_open_file_for_scanning(&file);
    if (retval==0)
	{
		return -1;
	}
	retval = azxml_parse();
	azxml_close_file(&file);

	free(file.szFilename);
	file.bInitFilename = 0;
	return retval;
}

void azxml_error(char *str)
{
	if (AZ_PARSER(parser_param)->cbError)
	{
		AZ_PARSER(parser_param)->cbError(azxml_scanner_get_filename(), str, azxml_scanner_get_lineno(), AZ_PARSER(parser_param)->arg);
	}
}

void azoth_error(int type, const char *format, ...)
{
	char msgBuf[256];
	va_list args;
	va_start(args, format);
	xmlVsnprintf(msgBuf, 255, format, args);
	va_end(args);
	azxml_error(msgBuf);
}

void AzothXmlStart(azoth_value *pTag)
{
	if (AZ_PARSER(parser_param)->cbXmlStart)
	{
		AZ_PARSER(parser_param)->cbXmlStart(pTag->value.str.val, AZ_PARSER(parser_param)->arg);
	}
	azoth_freestr(pTag->value.str.val);
}

void AzothXmlTagStart(azoth_value *pTag)
{
	AzothStackPushString(pTag->value.str.val);
	if (AZ_PARSER(parser_param)->cbTagStart)
	{
		AZ_PARSER(parser_param)->cbTagStart(pTag->value.str.val, AZ_PARSER(parser_param)->arg);
	}
	azoth_freestr(pTag->value.str.val);
}

void AzothXmlTagEnd()
{
	if (AZ_PARSER(parser_param)->cbTagEnd)
	{
		AZ_PARSER(parser_param)->cbTagEnd(AZ_PARSER(parser_param)->arg);
	}

    if (s_bHtmlCompatible && pHead)
    {
	    if (   xmlStricmp(AzothStackTopString(), "input")==0
	        || xmlStricmp(AzothStackTopString(), "meta")==0
	       )
	    {
	        AzothStackPopString();
	    }
    }
}

void AzothXmlTagEndImme()
{
	AzothStackPopString();
	if (AZ_PARSER(parser_param)->cbTagEndImme)
	{
		AZ_PARSER(parser_param)->cbTagEndImme(AZ_PARSER(parser_param)->arg);
	}
}

void AzothXmlCloseTagStart(azoth_value *pTag)
{
	if (xmlStricmp(AzothStackTopString(), pTag->value.str.val))
	{
		azoth_error(0, "Tag [%s] ²»Æ¥Åä.", pTag->value.str.val);
	}
	AzothStackPopString();
	if (AZ_PARSER(parser_param)->cbCloseTagStart)
	{
		AZ_PARSER(parser_param)->cbCloseTagStart(pTag->value.str.val, AZ_PARSER(parser_param)->arg);
	}
	azoth_freestr(pTag->value.str.val);
}

void AzothXmlTagAttrib(azoth_value *pAttrib, azoth_value *pValue)
{
	if (AZ_PARSER(parser_param)->cbTagAttribute)
	{
		AZ_PARSER(parser_param)->cbTagAttribute(pAttrib->value.str.val, pValue->value.str.val, AZ_PARSER(parser_param)->arg);
	}
	azoth_freestr(pAttrib->value.str.val);
	azoth_freestr(pValue->value.str.val);
}

void AzothXmlItem(azoth_value *pItem)
{
	if (AZ_PARSER(parser_param)->cbItem)
	{
		AZ_PARSER(parser_param)->cbItem(0, pItem->value.str.val, AZ_PARSER(parser_param)->arg);
	}
	azoth_freestr(pItem->value.str.val);
}

void AzothXmlSpace(azoth_value *pItem)
{
	if (AZ_PARSER(parser_param)->cbItem)
	{
		AZ_PARSER(parser_param)->cbItem(1, pItem->value.str.val, AZ_PARSER(parser_param)->arg);
	}
	azoth_freestr(pItem->value.str.val);
}

void AzothStackPushString(const char* pString)
{
	struct SXML_STACK* pItem = (struct SXML_STACK*)malloc(sizeof(struct SXML_STACK));
	int len = strlen(pString);
	pItem->pStr = (char*)malloc(len+1);
	strcpy(pItem->pStr, pString);
	pItem->pStr[len] = 0;
	pItem->pNext = pHead;
	pHead = pItem;
}

void AzothStackPopString()
{
	struct SXML_STACK* pItem = pHead;
	if (pItem)
	{
		pHead = pHead->pNext;
		free(pItem->pStr);
		free(pItem);
	}
}

const char* AzothStackTopString()
{
    return (const char*)pHead->pStr;
}

void AzothStackClear()
{
    while (pHead)
    {
        AzothStackPopString();
    }
}

int AzothStackIsEmpty()
{
    return pHead?1:0;
}

void signalevent(int event, azoth_value *pTag)
{
    switch (event)
    {
    case  XML_SPACES:       printf(" ");break;
    case  XML_IDENTITY:     printf(" ");break;
    case  XML_STRING:       printf("\"");break;
    case  XML_TEXT:         printf("");break;
    case  XML_START:        printf("<?xml");break;
    case  XML_TAG_START:    printf("<");break;
    case  XML_TAG_END:      printf(">");break;
    case  XML_TAG_CLOSE:    printf("</");break;
    case  XML_TAG_XEND:     printf("/>");break;
    case  XML_TAG_EMPTY:    printf("</>");break;
    }
    if (pTag)
    {
        printf("%s", pTag->value.str.val);
    }
    switch (event)
    {
    case  XML_SPACES:       printf("");break;
    case  XML_IDENTITY:     printf("=");break;
    case  XML_STRING:       printf("\"");break;
    case  XML_TEXT:         printf("");break;
    case  XML_START:        printf("?>");break;
    case  XML_TAG_START:    printf("");break;
    case  XML_TAG_END:      printf("");break;
    case  XML_TAG_CLOSE:    printf("");break;
    case  XML_TAG_XEND:     printf("");break;
    case  XML_TAG_EMPTY:    printf("");break;
    }
}


#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		36
#define	YYFLAG		-32768
#define	YYNTBASE	15

#define YYTRANSLATE(x) ((unsigned)(x) <= 268 ? yytranslate[x] : 30)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    14,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     6,     9,    10,    12,    13,    15,    17,    19,    21,
    23,    25,    27,    30,    31,    33,    35,    37,    44,    48,
    54,    57,    60,    61
};

static const short yyrhs[] = {    16,
    17,    16,    27,    16,     0,    16,     3,     0,     0,     7,
     0,     0,     8,     0,     9,     0,    10,     0,    11,     0,
    12,     0,     4,     0,     5,     0,    26,    25,     0,     0,
     3,     0,     6,     0,    27,     0,    18,    28,    19,    25,
    20,    19,     0,    18,    28,    21,     0,    18,    28,    19,
    25,    22,     0,     1,    19,     0,    28,    29,     0,     0,
    23,    14,    24,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   290,   294,   296,   299,   301,   304,   308,   312,   316,   320,
   324,   328,   332,   334,   337,   339,   340,   343,   345,   346,
   347,   350,   352,   355
};

static const char * const yytname[] = {   "$","error","$undefined.","XML_SPACES",
"XML_IDENTITY","XML_STRING","XML_TEXT","XML_START","XML_TAG_START","XML_TAG_END",
"XML_TAG_CLOSE","XML_TAG_XEND","XML_TAG_EMPTY","XML_ERROR","'='","xml_doc","xml_opspaces",
"xml_start","xml_tag_start","xml_tag_end","xml_tag_close","xml_tag_xend","xml_tag_empty",
"xml_identity","xml_string","xml_blocks","xml_block","xml_element","xml_options",
"xml_option",""
};
#endif

static const short yyr1[] = {     0,
    15,    16,    16,    17,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    25,    26,    26,    26,    27,    27,    27,
    27,    28,    28,    29
};

static const short yyr2[] = {     0,
     5,     2,     0,     1,     0,     1,     1,     1,     1,     1,
     1,     1,     2,     0,     1,     1,     1,     6,     3,     5,
     2,     2,     0,     3
};

static const short yydefact[] = {     3,
     5,     2,     4,     3,     0,     0,     6,    23,     3,     7,
    21,     0,     1,    11,     9,     0,    19,     0,    22,    15,
    16,     0,     0,    17,     0,     8,    10,     0,    20,    13,
    12,    24,    18,     0,     0,     0
};

static const short yydefgoto[] = {    34,
     1,     4,     8,    11,    28,    17,    29,    18,    32,    22,
    23,    24,    12,    19
};

static const short yypact[] = {-32768,
     4,-32768,-32768,-32768,     1,    -4,-32768,-32768,-32768,-32768,
-32768,     9,    18,-32768,-32768,     0,-32768,     8,-32768,-32768,
-32768,     5,     0,-32768,    19,-32768,-32768,    -4,-32768,-32768,
-32768,-32768,-32768,    23,    25,-32768
};

static const short yypgoto[] = {-32768,
    10,-32768,-32768,   -12,-32768,-32768,-32768,-32768,-32768,     3,
-32768,    22,-32768,-32768
};


#define	YYLAST		27


static const short yytable[] = {    16,
     6,     6,    20,     2,    10,    21,     2,     7,     7,   -14,
     3,   -14,    14,     5,    26,    33,    27,    10,    13,    15,
     2,    25,    35,    31,    36,    30,     9
};

static const short yycheck[] = {    12,
     1,     1,     3,     3,     9,     6,     3,     8,     8,    10,
     7,    12,     4,     4,    10,    28,    12,     9,     9,    11,
     3,    14,     0,     5,     0,    23,     5
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

  memset(&yyval,0,sizeof(YYSTYPE));

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 300 "xml_parser.y"
{ AzothXmlStart(&yyvsp[0]); ;
    break;}
case 6:
#line 305 "xml_parser.y"
{ AzothXmlTagStart(&yyvsp[0]);;
    break;}
case 7:
#line 309 "xml_parser.y"
{ AzothXmlTagEnd(); ;
    break;}
case 8:
#line 313 "xml_parser.y"
{ AzothXmlCloseTagStart(&yyvsp[0]); ;
    break;}
case 9:
#line 317 "xml_parser.y"
{ AzothXmlTagEndImme(); ;
    break;}
case 10:
#line 321 "xml_parser.y"
{ ;
    break;}
case 11:
#line 325 "xml_parser.y"
{ ;
    break;}
case 12:
#line 329 "xml_parser.y"
{ ;
    break;}
case 15:
#line 338 "xml_parser.y"
{ AzothXmlSpace(&yyvsp[0]); ;
    break;}
case 16:
#line 339 "xml_parser.y"
{ AzothXmlItem(&yyvsp[0]); ;
    break;}
case 21:
#line 347 "xml_parser.y"
{ azxml_error("xml parser error."); ;
    break;}
case 24:
#line 356 "xml_parser.y"
{AzothXmlTagAttrib(&yyvsp[-2], &yyvsp[0]);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 358 "xml_parser.y"
