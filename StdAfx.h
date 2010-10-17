// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DA9361ED_4497_4B2C_BDB1_ACFBAAFB190B__INCLUDED_)
#define AFX_STDAFX_H__DA9361ED_4497_4B2C_BDB1_ACFBAAFB190B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>	// mulithread
#include <vector>

#define Xmark 1
#define Omark 2
#define Xwin 3
#define Owin 4
#define spacewidth 16
#define spaceheight 16
#define OpPlayer Player ^ 3
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DA9361ED_4497_4B2C_BDB1_ACFBAAFB190B__INCLUDED_)
