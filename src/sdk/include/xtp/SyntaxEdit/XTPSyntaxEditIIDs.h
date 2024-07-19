// DO NOT MODIFY MANUALLY
// Generated on Sat 07/28/2018 at 10:06:20.33 using cj-update-iids.cmd

// XTPSyntaxEditIIDs.h: GUID definitions for SyntaxEdit.odl.
//
// (c)1998-2018 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __XTPSyntaxEditIIDs_H__
#define __XTPSyntaxEditIIDs_H__

#define XTP_DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
       EXTERN_C __declspec(selectany) const IID XTP##name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}; \
       const unsigned long  XTP##name##__Data1   = (l);  \
       const unsigned short XTP##name##__Data2   = (w1); \
       const unsigned short XTP##name##__Data3   = (w2); \
       const unsigned char  XTP##name##__Data4_0 = (b1); \
       const unsigned char  XTP##name##__Data4_1 = (b2); \
       const unsigned char  XTP##name##__Data4_2 = (b3); \
       const unsigned char  XTP##name##__Data4_3 = (b4); \
       const unsigned char  XTP##name##__Data4_4 = (b5); \
       const unsigned char  XTP##name##__Data4_5 = (b6); \
       const unsigned char  XTP##name##__Data4_6 = (b7); \
       const unsigned char  XTP##name##__Data4_7 = (b8);

XTP_DEFINE_GUID(LIBID_XtremeSyntaxEdit,0xCA73588D,0x282F,0x4592,0x93,0x69,0xA6,0x1C,0xC2,0x44,0xFA,0xDA);
XTP_DEFINE_GUID(DIID_ISyntaxEditGlobalSettings,0xC0DE1850,0xC45B,0x4319,0xA3,0x8F,0x9F,0x92,0x97,0xAC,0x56,0xEF);
XTP_DEFINE_GUID(DIID_SyntaxEditTextPosition,0xC0DE1850,0x9B37,0x4919,0x98,0x53,0xF8,0xC7,0x1A,0xC1,0x31,0x70);
XTP_DEFINE_GUID(DIID_SyntaxEditTextSelection,0xC0DE1850,0x1E28,0x4880,0x9D,0x4E,0x97,0x06,0x9B,0x34,0x58,0x99);
XTP_DEFINE_GUID(DIID_SyntaxEditPrintOptions,0xC0DE1850,0xF565,0x4076,0x95,0x30,0x5B,0x63,0x86,0x39,0x96,0x4D);
XTP_DEFINE_GUID(DIID_SyntaxEditDataManager,0xC0DE1850,0x4EF1,0x4ea0,0x9C,0x0C,0xF2,0xFF,0x65,0x18,0xA0,0x0E);
XTP_DEFINE_GUID(DIID_SyntaxEditPaintManager,0xC0DE1850,0xE55D,0x40e1,0xA0,0xE5,0x4A,0xFF,0x47,0x8A,0x70,0x42);
XTP_DEFINE_GUID(DIID__DSyntaxEdit,0xC0DE1850,0xCC75,0x4462,0xAA,0xBB,0x21,0x18,0x09,0x2E,0x0E,0x1A);
XTP_DEFINE_GUID(DIID__DSyntaxEditEvents,0xC0DE1850,0x13B2,0x4eeb,0xB4,0xB8,0xDB,0x0D,0x5E,0x3F,0x7B,0x24);
XTP_DEFINE_GUID(CLSID_SyntaxEdit,0xC0DE1850,0x1557,0x497D,0xB5,0x22,0x62,0xBC,0x13,0x29,0xA5,0xA7);
XTP_DEFINE_GUID(DIID__DSyntaxEditFrame,0xC0DE1850,0x49DD,0x423A,0xA4,0x81,0x5F,0x37,0x07,0x4C,0xD2,0xB7);
XTP_DEFINE_GUID(DIID__DSyntaxEditFrameEvents,0xC0DE1850,0xAE1F,0x4ECC,0x81,0x1C,0xE2,0x3F,0xD6,0xC4,0xA5,0xBA);
XTP_DEFINE_GUID(CLSID_SyntaxEditFrame,0xC0DE1850,0x4E21,0x4EE0,0xAE,0x53,0x0C,0x2C,0x36,0x4C,0x51,0xDD);
XTP_DEFINE_GUID(CLSID_SyntaxEditGlobalSettings,0xC0DE1850,0x48B3,0x4DD2,0xA4,0xC6,0x1F,0xAB,0xA9,0xAC,0x06,0x41);

#undef XTP_DEFINE_GUID

#endif // __XTPSyntaxEditIIDs_H__

