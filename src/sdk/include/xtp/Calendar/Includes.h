// Includes.h : header file
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPCALENDAR_INCLUDES_H__)
#define __XTPCALENDAR_INCLUDES_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Calendar/Resource.h"
#include "Calendar/XTPCalendarDefines.h"

//----------------------------
// Xtreme Calendar Control classes:
//----------------------------

#include "Calendar/XTPCalendarUtils.h"
#include "Calendar/XTPCalendarOptions.h"
#include "Calendar/XTPCalendarNotifications.h"
#include "Calendar/XTPCalendarPtrCollectionT.h"
#include "Calendar/XTPCalendarTimeZoneHelper.h"
#include "Calendar/XTPCalendarEvent.h"
#include "Calendar/XTPCalendarEventLabel.h"
#include "Calendar/XTPCalendarEvents.h"
#include "Calendar/XTPCalendarRecurrencePattern.h"
#include "Calendar/XTPCalendarView.h"
#include "Calendar/XTPCalendarViewDay.h"
#include "Calendar/XTPCalendarViewEvent.h"
#include "Calendar/XTPCalendarViewPart.h"
#include "Calendar/XTPCalendarDayViewEvent.h"
#include "Calendar/XTPCalendarDayViewDay.h"
#include "Calendar/XTPCalendarDayView.h"
#include "Calendar/XTPCalendarDayViewTimeScale.h"
#include "Calendar/XTPCalendarMonthViewEvent.h"
#include "Calendar/XTPCalendarMonthViewDay.h"
#include "Calendar/XTPCalendarMonthView.h"
#include "Calendar/XTPCalendarWeekViewEvent.h"
#include "Calendar/XTPCalendarWeekViewDay.h"
#include "Calendar/XTPCalendarWeekView.h"
#include "Calendar/XTPCalendarTimeLineView.h"
#include "Calendar/XTPCalendarTimeLineViewGroup.h"
#include "Calendar/XTPCalendarTimeLineViewTimeScale.h"
#include "Calendar/XTPCalendarResource.h"
#include "Calendar/XTPCalendarTip.h"
#include "Calendar/XTPCalendarControl.h"

//----------------------------
// Xtreme Calendar Caption Bar classes:
//----------------------------

#include "Calendar/CaptionBar/XTPCalendarCaptionBarThemePart.h"
#include "Calendar/CaptionBar/XTPCalendarCaptionBarTheme.h"
#include "Calendar/CaptionBar/XTPCalendarCaptionBarPaintManager.h"
#include "Calendar/CaptionBar/XTPCalendarCaptionBarControl.h"
#include "Calendar/CaptionBar/Themes/XTPCalendarCaptionBarOffice2000Theme.h"
#include "Calendar/CaptionBar/Themes/XTPCalendarCaptionBarOfficeXPTheme.h"
#include "Calendar/CaptionBar/Themes/XTPCalendarCaptionBarOffice2003Theme.h"
#include "Calendar/CaptionBar/Themes/XTPCalendarCaptionBarOffice2013Theme.h"
#include "Calendar/CaptionBar/Themes/XTPCalendarCaptionBarResourceTheme.h"

//----------------------------
// Xtreme Calendar Control classes:
//----------------------------

#include "Calendar/XTPCalendarControlView.h"
#include "Calendar/XTPCalendarController.h"
#include "Calendar/XTPCalendarPaintManager.h"
#include "Calendar/XTPCalendarTheme.h"
#include "Calendar/XTPCalendarThemePrevNextEventButton.h"
#include "Calendar/XTPCalendarThemePrevNextEventButtons.h"
#include "Calendar/XTPCalendarThemeOffice2007.h"
#include "Calendar/XTPCalendarRemindersManager.h"
#include "Calendar/XTPCalendarResourcesManager.h"
#include "Calendar/XTPCalendarCustomProperties.h"
#include "Calendar/XTPCalendarOccurSeriesChooseDlg.h"
#include "Calendar/XTPCalendarEventPropertiesDlg.h"
#include "Calendar/XTPCalendarRemindersDialog.h"
#include "Calendar/XTPCalendarEventRecurrenceDlg.h"
#include "Calendar/XTPCalendarData.h"
#include "Calendar/XTPCalendarMemoryDataProvider.h"
#include "Calendar/XTPCalendarADO.h"
#include "Calendar/XTPCalendarCustomDataProvider.h"
#include "Calendar/XTPCalendarDatabaseDataProvider.h"
#include "Calendar/XTPCalendarMsgNotifier.h"
#include "Calendar/XTPCalendarShadowDrawer.h"
#include "Calendar/Themes/XTPCalendarOffice2000Theme.h"
#include "Calendar/Themes/XTPCalendarOfficeXPTheme.h"
#include "Calendar/Themes/XTPCalendarOffice2003Theme.h"
#include "Calendar/Themes/XTPCalendarOffice2013Theme.h"
#include "Calendar/Themes/XTPCalendarResourceTheme.h"
#include "Calendar/XTPCalendarEventColorNewDlg.h"
#include "Calendar/XTPCalendarEventColorListBox.h"
#include "Calendar/XTPCalendarEventColorDlg.h"

#ifdef _XTP_INCLUDE_CALENDAR_MAPI

//----------------------------
// MAPI Support:
//----------------------------

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include "Calendar/MAPI/mapidefs.h"
#include "Calendar/MAPI/mapicode.h"
#include "Calendar/MAPI/mapiguid.h"
#include "Calendar/MAPI/mapitags.h"
#include "Calendar/MAPI/mapix.h"
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Calendar/XTPCalendarMAPIWrapper.h"
#include "Calendar/XTPCalendarMAPIDataProvider.h"

#endif /*_XTP_INCLUDE_CALENDAR_MAPI*/


//----------------------------
// Xtreme DatePicker Control classes:
//----------------------------

#include "Calendar/DatePicker/XTPDatePickerControl.h"
#include "Calendar/DatePicker/XTPDatePickerDaysCollection.h"
#include "Calendar/DatePicker/XTPDatePickerItemDay.h"
#include "Calendar/DatePicker/XTPDatePickerItemMonth.h"
#include "Calendar/DatePicker/XTPDatePickerList.h"
#include "Calendar/DatePicker/XTPDatePickerNotifications.h"
#include "Calendar/DatePicker/XTPDatePickerPaintManager.h"
#include "Calendar/DatePicker/Themes/XTPDatePickerOffice2000Theme.h"
#include "Calendar/DatePicker/Themes/XTPDatePickerOfficeXPTheme.h"
#include "Calendar/DatePicker/Themes/XTPDatePickerOffice2003Theme.h"
#include "Calendar/DatePicker/Themes/XTPDatePickerOffice2013Theme.h"
#include "Calendar/DatePicker/Themes/XTPDatePickerResourceTheme.h"

#endif // #if !defined(__XTPCALENDAR_INCLUDES_H__)

//}}AFX_CODEJOCK_PRIVATE
