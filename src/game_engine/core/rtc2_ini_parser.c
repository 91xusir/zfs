
/*  A Bison parser, made from ini_parser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1 /* Identify Bison output.  */

#define yyparse azini_parse
#define yylex azini_lex
#define yyerror azini_error
#define yylval azini_lval
#define yychar azini_char
#define yydebug azini_debug
#define yynerrs azini_nerrs
#define INI_STRING 258
#define INI_BOOL 259
#define INI_LONG 260
#define INI_DOUBLE 261
#define INI_ENCAPSULATED_STRING 262
#define INI_SECTION 263

#line 1 "ini_parser.y"

/*********************************************************/
#define DEBUG_CFG_PARSER 0
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "rtc2_script_comm.h"
#include "rtc2_ini_i.h"

#define YYSTYPE azoth_value
#define YYERROR_VERBOSE

int azini_lex(azoth_value* pIniVal);
int azini_parse(void);

azoth_value    yylval;
s_azoth_parser gParser;

int AzothParseIniFile(const char* szFilename, TypeCallbackAzothIniError callbackIniError,
                      TypeCallbackAzothIniParser callbackParserIni, void* arg) {
    int                      retval;            // 存储返回值
    s_azoth_file             file;              // 定义文件结构体
    s_azoth_ini_parser_param ini_parser_param;  // 定义解析参数结构体

    // 初始化解析参数结构体
    ini_parser_param.callbackParser = callbackParserIni;  // 设置解析回调函数
    ini_parser_param.callbackError = callbackIniError;    // 设置错误回调函数
    ini_parser_param.arg = arg;                           // 设置用户参数
    AZ_PARSER(ini_parser_param) = &ini_parser_param;      // 将参数指针赋值给全局解析器

    file.szFilename = strdup(szFilename);  // 复制文件名到 file.szFilename
    file.bInitFilename = 1;                // 设置文件名初始化标志
    file.pArc = 0;                         // 初始化 pArc 指针为 0

    retval = azini_open_file_for_scanning(
        &file);       // 调用 azini_open_file_for_scanning 函数打开文件进行扫描
    if (retval == 0)  // 如果返回值为 0，则表示文件打开失败
    {
        return -1;  // 返回 -1 表示出错
    }

    retval = azini_parse();   // 调用 azini_parse 函数进行解析
    azini_close_file(&file);  // 关闭文件

    free(file.szFilename);   // 释放复制的文件名
    file.bInitFilename = 0;  // 重置文件名初始化标志

    return retval;  // 返回解析结果
}

static void azini_error(char* str) {
    if (AZ_PARSER(ini_parser_param)->callbackError) {
        AZ_PARSER(ini_parser_param)
            ->callbackError(azini_scanner_get_filename(), str, azini_scanner_get_lineno(),
                            AZ_PARSER(ini_parser_param)->arg);
    }
}

void AzothIniDoOperate(char cOpType, azoth_value* pResult, azoth_value* pOperate1,
                       azoth_value* pOperate2) {
    pResult->type = IS_BOOL;
    switch (cOpType) {
        case '|':
            pResult->value.lval = pOperate1->value.lval | pOperate2->value.lval;
            break;
        case '&':
            pResult->value.lval = pOperate1->value.lval & pOperate2->value.lval;
            break;
        case '~':
            pResult->value.lval = ~pOperate1->value.lval;
            break;
        case '!':
            pResult->value.lval = !pOperate1->value.lval;
            break;
        default:
            pResult->value.lval = 0;
            break;
    }
}

void AzothIniGetConstant(azoth_value* result, azoth_value* name) {
    *result = *name;
}

void AzothIniAddItem(azoth_value* arg1, azoth_value* arg2, int callback_type) {
    AZ_PARSER(ini_parser_param)
        ->callbackParser(arg1, arg2, callback_type, AZ_PARSER(ini_parser_param)->arg);
}

#ifndef YYLTYPE
typedef struct yyltype {
    int   timestamp;
    int   first_line;
    int   first_column;
    int   last_line;
    int   last_column;
    char* text;
} yyltype;

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

#define YYFINAL 30
#define YYFLAG -32768
#define YYNTBASE 18

#define YYTRANSLATE(x) ((unsigned)(x) <= 264 ? yytranslate[x] : 23)

static const char yytranslate[] = {
    0, 2,  2, 2,  2, 2, 2,  2, 2,  2, 14, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 12, 2, 2, 2,  2, 10, 2, 16, 17, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 13, 2, 2,  2, 2, 2,  2, 2,  2, 2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 2,  2, 2, 2,  2, 2,  2, 2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 2,  9, 2, 11, 2, 2,  2, 2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 2,  2, 2, 2,  2, 2,  2, 2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 2,  2, 2, 2,  2, 2,  2, 2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 2,  2, 2, 2,  2, 2,  2, 2,  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2,  2, 2,  2, 2, 2,  2, 2,  2, 2,  2,  2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 15};

#if YYDEBUG != 0
static const short yyprhs[] = {0,  0,  3,  4,  8,  10, 12, 14, 16, 18, 20,
                               22, 24, 26, 28, 30, 32, 36, 40, 43, 46, 50};

static const short yyrhs[] = {18, 19, 0,  0, 3,  13, 20, 0,  3,  0, 8,  0,  14, 0, 22, 0, 21, 0,
                              7,  0,  4,  0, 5,  0,  6,  0,  14, 0, 15, 0,  4,  0, 21, 9, 21, 0,
                              21, 10, 21, 0, 11, 21, 0,  12, 21, 0, 16, 21, 17, 0, 3,  0};

#endif

#if YYDEBUG != 0
static const short yyrline[] = {0,   102, 104, 107, 114, 115, 116, 119, 121, 122, 123,
                                124, 125, 126, 127, 130, 132, 133, 134, 135, 136, 139};

static const char* const yytname[] = {"$",
                                      "error",
                                      "$undefined.",
                                      "INI_STRING",
                                      "INI_BOOL",
                                      "INI_LONG",
                                      "INI_DOUBLE",
                                      "INI_ENCAPSULATED_STRING",
                                      "INI_SECTION",
                                      "'|'",
                                      "'&'",
                                      "'~'",
                                      "'!'",
                                      "'='",
                                      "'\\n'",
                                      "'000'",
                                      "'('",
                                      "')'",
                                      "statement_list",
                                      "statement",
                                      "string_or_value",
                                      "expr",
                                      "constant_string",
                                      ""};
#endif

static const short yyr1[] = {0,  18, 18, 19, 19, 19, 19, 20, 20, 20, 20,
                             20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22};

static const short yyr2[] = {0, 2, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 2, 2, 3, 1};

static const short yydefact[] = {2, 0, 4, 5, 6,  1,  0,  21, 10, 11, 12, 9,  0,  0, 13, 14,
                                 0, 3, 8, 7, 15, 18, 19, 0,  0,  0,  20, 16, 17, 0, 0};

static const short yydefgoto[] = {1, 5, 17, 18, 19};

static const short yypact[] = {-32768, 0,      -7,     -32768, -32768, -32768, 6,      -32768,
                               -8,     -32768, -32768, -32768, 19,     19,     -32768, -32768,
                               19,     -32768, -5,     -32768, -32768, -32768, -32768, 15,
                               19,     19,     -32768, -32768, -32768, 7,      -32768};

static const short yypgoto[] = {-32768, -32768, -32768, 3, -32768};

#define YYLAST 35

static const short yytable[] = {29, -15, -15, 2,  24, 25, 6,  30, 3,  7,  8,  9,
                                10, 11,  4,   21, 22, 12, 13, 23, 14, 15, 16, 20,
                                24, 25,  0,   27, 28, 0,  12, 13, 26, 0,  0,  16};

static const short yycheck[] = {0, 9,  10, 3,  9,  10, 13, 0,  8,  3,  4,  5,
                                6, 7,  14, 12, 13, 11, 12, 16, 14, 15, 16, 4,
                                9, 10, -1, 24, 25, -1, 11, 12, 17, -1, -1, 16};
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
#if (!defined(__STDC__) && defined(sparc)) || defined(__sparc__) || defined(__sparc) ||            \
    defined(__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined(MSDOS) && !defined(__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
#pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void* alloca(unsigned int);
};
#else  /* not __cplusplus */
void* alloca();
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

#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = YYEMPTY)
#define YYEMPTY -2
#define YYEOF 0
#define YYACCEPT return (0)
#define YYABORT return (1)
#define YYERROR goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL goto yyerrlab
#define YYRECOVERING() (!!yyerrstatus)
#define YYBACKUP(token, value)                                                                     \
    do                                                                                             \
        if (yychar == YYEMPTY && yylen == 1) {                                                     \
            yychar = (token), yylval = (value);                                                    \
            yychar1 = YYTRANSLATE(yychar);                                                         \
            YYPOPSTACK;                                                                            \
            goto yybackup;                                                                         \
        } else {                                                                                   \
            yyerror("syntax error: cannot back up");                                               \
            YYERROR;                                                                               \
        }                                                                                          \
    while (0)

#define YYTERROR 1
#define YYERRCODE 256

#ifndef YYPURE
#define YYLEX yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int     yychar; /*  the lookahead symbol		*/
YYSTYPE yylval; /*  the semantic value of the		*/
/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc; /*  location data for the lookahead	*/
/*  symbol				*/
#endif

int yynerrs; /*  number of parse errors so far       */
#endif       /* not YYPURE */

#if YYDEBUG != 0
int yydebug; /*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef YYINITDEPTH
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
int yyparse(void);
#endif

#if __GNUC__ > 1 /* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM, TO, COUNT) __builtin_memcpy(TO, FROM, COUNT)
#else /* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void __yy_memcpy(from, to, count) char* from;
char*       to;
int         count;
{
    register char* f = from;
    register char* t = to;
    register int   i = count;

    while (i-- > 0)
        *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void __yy_memcpy(char* from, char* to, int count) {
    register char* f = from;
    register char* t = to;
    register int   i = count;

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
#define YYPARSE_PARAM_DECL void* YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int yyparse(YYPARSE_PARAM) YYPARSE_PARAM_DECL {
    register int      yystate;
    register int      yyn;
    register short*   yyssp;
    register YYSTYPE* yyvsp;
    int               yyerrstatus; /*  number of tokens to shift before error messages enabled */
    int               yychar1 = 0; /*  lookahead token as an internal (translated) token number */

    short   yyssa[YYINITDEPTH]; /*  the state stack			*/
    YYSTYPE yyvsa[YYINITDEPTH]; /*  the semantic value stack		*/

    short*   yyss = yyssa; /*  refer to the stacks thru separate pointers */
    YYSTYPE* yyvs = yyvsa; /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
    YYLTYPE  yylsa[YYINITDEPTH]; /*  the location stack			*/
    YYLTYPE* yyls = yylsa;
    YYLTYPE* yylsp;

#define YYPOPSTACK (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK (yyvsp--, yyssp--)
#endif

    int yystacksize = YYINITDEPTH;

#ifdef YYPURE
    int     yychar;
    YYSTYPE yylval;
    int     yynerrs;
#ifdef YYLSP_NEEDED
    YYLTYPE yylloc;
#endif
#endif

    YYSTYPE yyval; /*  the variable used to return		*/
                   /*  semantic values from the action	*/
                   /*  routines				*/

    int yylen;

    memset(&yyval, 0, sizeof(YYSTYPE));

#if YYDEBUG != 0
    if (yydebug)
        fprintf(stderr, "Starting parse\n");
#endif

    yystate = 0;
    yyerrstatus = 0;
    yynerrs = 0;
    yychar = YYEMPTY; /* Cause a token to be read.  */

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

    if (yyssp >= yyss + yystacksize - 1) {
        /* Give user a chance to reallocate the stack */
        /* Use copies of these so that the &'s don't force the real ones into memory. */
        YYSTYPE* yyvs1 = yyvs;
        short*   yyss1 = yyss;
#ifdef YYLSP_NEEDED
        YYLTYPE* yyls1 = yyls;
#endif

        /* Get the current used size of the three stacks, in elements.  */
        int size = yyssp - yyss + 1;

#ifdef yyoverflow
        /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
        /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
        yyoverflow("parser stack overflow", &yyss1, size * sizeof(*yyssp), &yyvs1,
                   size * sizeof(*yyvsp), &yyls1, size * sizeof(*yylsp), &yystacksize);
#else
        yyoverflow("parser stack overflow", &yyss1, size * sizeof(*yyssp), &yyvs1,
                   size * sizeof(*yyvsp), &yystacksize);
#endif

        yyss = yyss1;
        yyvs = yyvs1;
#ifdef YYLSP_NEEDED
        yyls = yyls1;
#endif
#else /* no yyoverflow */
        /* Extend the stack our own way.  */
        if (yystacksize >= YYMAXDEPTH) {
            yyerror("parser stack overflow");
            return 2;
        }
        yystacksize *= 2;
        if (yystacksize > YYMAXDEPTH)
            yystacksize = YYMAXDEPTH;
        yyss = (short*)alloca(yystacksize * sizeof(*yyssp));
        __yy_memcpy((char*)yyss1, (char*)yyss, size * sizeof(*yyssp));
        yyvs = (YYSTYPE*)alloca(yystacksize * sizeof(*yyvsp));
        __yy_memcpy((char*)yyvs1, (char*)yyvs, size * sizeof(*yyvsp));
#ifdef YYLSP_NEEDED
        yyls = (YYLTYPE*)alloca(yystacksize * sizeof(*yylsp));
        __yy_memcpy((char*)yyls1, (char*)yyls, size * sizeof(*yylsp));
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

    if (yychar == YYEMPTY) {
#if YYDEBUG != 0
        if (yydebug)
            fprintf(stderr, "Reading a token: ");
#endif
        yychar = YYLEX;
    }

    /* Convert token to internal form (in yychar1) for indexing tables with */

    if (yychar <= 0) /* This means end of input. */
    {
        yychar1 = 0;
        yychar = YYEOF; /* Don't call YYLEX any more */

#if YYDEBUG != 0
        if (yydebug)
            fprintf(stderr, "Now at end of input.\n");
#endif
    } else {
        yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
        if (yydebug) {
            fprintf(stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
            /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
            YYPRINT(stderr, yychar, yylval);
#endif
            fprintf(stderr, ")\n");
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

    if (yyn < 0) {
        if (yyn == YYFLAG)
            goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
    } else if (yyn == 0)
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
    if (yyerrstatus)
        yyerrstatus--;

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
        yyval = yyvsp[1 - yylen]; /* implement default value of the action */

#if YYDEBUG != 0
    if (yydebug) {
        int i;

        fprintf(stderr, "Reducing via rule %d (line %d), ", yyn, yyrline[yyn]);

        /* Print the symbols being reduced, and their result.  */
        for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
            fprintf(stderr, "%s ", yytname[yyrhs[i]]);
        fprintf(stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

    switch (yyn) {

        case 3:
#line 108 "ini_parser.y"
        {
#if DEBUG_CFG_PARSER
            printf("'%s' = '%s'\n", yyvsp[-2].value.str.val, yyvsp[0].value.str.val);
#endif
            AzothIniAddItem(&yyvsp[-2], &yyvsp[0], AZOTH_INI_ENTRY);
            ;
            break;
        }
        case 4:
#line 114 "ini_parser.y"
        {
            AzothIniAddItem(&yyvsp[0], NULL, AZOTH_INI_ENTRY);
            ;
            break;
        }
        case 5:
#line 115 "ini_parser.y"
        {
            AzothIniAddItem(&yyvsp[0], NULL, AZOTH_INI_SECTION);
            ;
            break;
        }
        case 7:
#line 120 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 8:
#line 121 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 9:
#line 122 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 10:
#line 123 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 11:
#line 124 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 12:
#line 125 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 13:
#line 126 "ini_parser.y"
        {
            yyval.value.str.val = strdup("");
            yyval.value.str.len = 0;
            yyval.type = IS_STRING;
            ;
            break;
        }
        case 14:
#line 127 "ini_parser.y"
        {
            yyval.value.str.val = strdup("");
            yyval.value.str.len = 0;
            yyval.type = IS_STRING;
            ;
            break;
        }
        case 15:
#line 131 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
        case 16:
#line 132 "ini_parser.y"
        {
            AzothIniDoOperate('|', &yyval, &yyvsp[-2], &yyvsp[0]);
            ;
            break;
        }
        case 17:
#line 133 "ini_parser.y"
        {
            AzothIniDoOperate('&', &yyval, &yyvsp[-2], &yyvsp[0]);
            ;
            break;
        }
        case 18:
#line 134 "ini_parser.y"
        {
            AzothIniDoOperate('~', &yyval, &yyvsp[0], NULL);
            ;
            break;
        }
        case 19:
#line 135 "ini_parser.y"
        {
            AzothIniDoOperate('!', &yyval, &yyvsp[0], NULL);
            ;
            break;
        }
        case 20:
#line 136 "ini_parser.y"
        {
            yyval = yyvsp[-1];
            ;
            break;
        }
        case 21:
#line 140 "ini_parser.y"
        {
            yyval = yyvsp[0];
            ;
            break;
        }
    }
        /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

    yyvsp -= yylen;
    yyssp -= yylen;
#ifdef YYLSP_NEEDED
    yylsp -= yylen;
#endif

#if YYDEBUG != 0
    if (yydebug) {
        short* ssp1 = yyss - 1;
        fprintf(stderr, "state stack now");
        while (ssp1 != yyssp)
            fprintf(stderr, " %d", *++ssp1);
        fprintf(stderr, "\n");
    }
#endif

    *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
    yylsp++;
    if (yylen == 0) {
        yylsp->first_line = yylloc.first_line;
        yylsp->first_column = yylloc.first_column;
        yylsp->last_line = (yylsp - 1)->last_line;
        yylsp->last_column = (yylsp - 1)->last_column;
        yylsp->text = 0;
    } else {
        yylsp->last_line = (yylsp + yylen - 1)->last_line;
        yylsp->last_column = (yylsp + yylen - 1)->last_column;
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

yyerrlab: /* here on detecting error */

    if (!yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
        ++yynerrs;

#ifdef YYERROR_VERBOSE
        yyn = yypact[yystate];

        if (yyn > YYFLAG && yyn < YYLAST) {
            int   size = 0;
            char* msg;
            int   x, count;

            count = 0;
            /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
            for (x = (yyn < 0 ? -yyn : 0); x < (sizeof(yytname) / sizeof(char*)); x++)
                if (yycheck[x + yyn] == x)
                    size += strlen(yytname[x]) + 15, count++;
            msg = (char*)malloc(size + 15);
            if (msg != 0) {
                strcpy(msg, "parse error");

                if (count < 5) {
                    count = 0;
                    for (x = (yyn < 0 ? -yyn : 0); x < (sizeof(yytname) / sizeof(char*)); x++)
                        if (yycheck[x + yyn] == x) {
                            strcat(msg, count == 0 ? ", expecting `" : " or `");
                            strcat(msg, yytname[x]);
                            strcat(msg, "'");
                            count++;
                        }
                }
                yyerror(msg);
                free(msg);
            } else
                yyerror("parse error; also virtual memory exceeded");
        } else
#endif /* YYERROR_VERBOSE */
            yyerror("parse error");
    }

    goto yyerrlab1;
yyerrlab1: /* here on error raised explicitly by an action */

    if (yyerrstatus == 3) {
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

    yyerrstatus = 3; /* Each real token shifted decrements this */

    goto yyerrhandle;

yyerrdefault: /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop: /* pop the current state because it cannot handle the error token */

    if (yyssp == yyss)
        YYABORT;
    yyvsp--;
    yystate = *--yyssp;
#ifdef YYLSP_NEEDED
    yylsp--;
#endif

#if YYDEBUG != 0
    if (yydebug) {
        short* ssp1 = yyss - 1;
        fprintf(stderr, "Error: state stack now");
        while (ssp1 != yyssp)
            fprintf(stderr, " %d", *++ssp1);
        fprintf(stderr, "\n");
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
    if (yyn < 0) {
        if (yyn == YYFLAG)
            goto yyerrpop;
        yyn = -yyn;
        goto yyreduce;
    } else if (yyn == 0)
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

#line 142 "ini_parser.y"
