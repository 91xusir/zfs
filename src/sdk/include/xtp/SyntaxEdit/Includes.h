// Includes.h : header file
//
// (c)1998-2018 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDIT_INCLUDES_H__)
#define __XTPSYNTAXEDIT_INCLUDES_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SyntaxEdit/Resource.h"

#include "SyntaxEdit/XTPSyntaxEditDefines.h"
#include "SyntaxEdit/XTPSyntaxEditStruct.h"

#include "SyntaxEdit/XTPSyntaxEditTextIterator.h"
#include "SyntaxEdit/XTPSyntaxEditFindReplaceDlg.h"
#include "SyntaxEdit/XTPSyntaxEditCtrl.h"
#include "SyntaxEdit/XTPSyntaxEditDoc.h"
#include "SyntaxEdit/XTPSyntaxEditView.h"
#include "SyntaxEdit/XTPSyntaxEditPaintManager.h"
#include "SyntaxEdit/XTPSyntaxEditColorSampleText.h"
#include "SyntaxEdit/XTPSyntaxEditColorComboBox.h"
#include "SyntaxEdit/XTPSyntaxEditPropertiesPage.h"
#include "SyntaxEdit/XTPSyntaxEditPropertiesDlg.h"
#include "SyntaxEdit/XTPSyntaxEditGoToLineDlg.h"
#include "SyntaxEdit/XTPSyntaxEditToolTipCtrl.h"
#include "SyntaxEdit/XTPSyntaxEditAutoCompleteWnd.h"
#include "SyntaxEdit/XTPSyntaxEditSelection.h"

#ifdef _XTP_INCLUDE_SYNTAXEDIT_LEXER

#include "SyntaxEdit/XTPSyntaxEditUndoManager.h"
#include "SyntaxEdit/XTPSyntaxEditLineMarksManager.h"
#include "SyntaxEdit/XTPSyntaxEditLexPtrs.h"
#include "SyntaxEdit/XTPSyntaxEditLexParser.h"
#include "SyntaxEdit/XTPSyntaxEditBufferManager.h"

// Commented to save compiler heap
#include "SyntaxEdit/XTPSyntaxEditSectionManager.h"
#include "SyntaxEdit/XTPSyntaxEditLexClassSubObjT.h"
#include "SyntaxEdit/XTPSyntaxEditTextIterator.h"
#include "SyntaxEdit/XTPSyntaxEditLexCfgFileReader.h"
#include "SyntaxEdit/XTPSyntaxEditLexClassSubObjDef.h"
#include "SyntaxEdit/XTPSyntaxEditLexClass.h"
#include "SyntaxEdit/XTPSyntaxEditLexColorFileReader.h"

using namespace XTPSyntaxEditLexAnalyser;
#endif

#endif // !defined(__XTPSYNTAXEDIT_INCLUDES_H__)
//}}AFX_CODEJOCK_PRIVATE
