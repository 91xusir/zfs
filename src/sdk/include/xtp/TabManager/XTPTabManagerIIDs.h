// DO NOT MODIFY MANUALLY
// Generated on Sat 07/28/2018 at 10:06:05.95 using cj-update-iids.cmd

// XTPTabManagerIIDs.h: GUID definitions for TabPaintManager.odl.
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

#ifndef __XTPTabManagerIIDs_H__
#define __XTPTabManagerIIDs_H__

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

XTP_DEFINE_GUID(DIID_ITabManagerMargin,0xC0DE1850,0xB4E9,0x45c7,0xAA,0xF6,0xEF,0x34,0x0E,0x78,0x55,0x90);
XTP_DEFINE_GUID(CLSID_TabManagerMargin,0xC0DE1850,0x90EE,0x4D24,0x8F,0x7A,0xDA,0x71,0xBD,0xEC,0x35,0xA8);
XTP_DEFINE_GUID(DIID_ITabPaintManagerColorSet,0xC0DE1850,0xED71,0x4B4E,0x91,0xCF,0x84,0x44,0xD6,0x24,0x4F,0x56);
XTP_DEFINE_GUID(CLSID_TabPaintManagerColorSet,0xC0DE1850,0x494B,0x4521,0xB7,0x09,0xC3,0x24,0xC2,0xC0,0x95,0x78);
XTP_DEFINE_GUID(DIID_ITabPaintManager,0xC0DE1850,0x9CE6,0x4D40,0xB6,0xC5,0x3B,0x94,0x25,0xFB,0xB5,0x5A);
XTP_DEFINE_GUID(CLSID_TabPaintManager,0xC0DE1850,0xBDD4,0x4257,0x8C,0xF4,0xA2,0xAB,0x16,0x74,0xC5,0x1E);
XTP_DEFINE_GUID(DIID_ITabControlItem,0xC0DE1850,0x9479,0x48D5,0xA1,0x4D,0x07,0x7A,0x9F,0xD5,0xD2,0x76);
XTP_DEFINE_GUID(CLSID_TabControlItem,0xC0DE1850,0x9808,0x4CBF,0xBA,0xC2,0xF2,0x45,0x5C,0x8C,0x8A,0x46);

#undef XTP_DEFINE_GUID

#endif // __XTPTabManagerIIDs_H__

