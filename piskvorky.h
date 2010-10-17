// piskvorky.h : main header file for the PISKVORKY application
//

#if !defined(AFX_PISKVORKY_H__5C6B7C29_A6B1_46DE_9639_9CEFAD6DA2CF__INCLUDED_)
#define AFX_PISKVORKY_H__5C6B7C29_A6B1_46DE_9639_9CEFAD6DA2CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyApp:
// See piskvorky.cpp for the implementation of this class
//
BOOL LoadData ();
BOOL DestroyData ();
CHAR* GetPath();
BOOL ShowSetupDlg ();

class CPiskvorkyApp : public CWinApp
{
public:
	CPiskvorkyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPiskvorkyApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPiskvorkyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PISKVORKY_H__5C6B7C29_A6B1_46DE_9639_9CEFAD6DA2CF__INCLUDED_)
