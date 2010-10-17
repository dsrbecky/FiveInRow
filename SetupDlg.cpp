// SetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "piskvorky.h"
#include "piskvorkyDlg.h"
#include "SetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog
extern BOOL LoadData ();
extern BOOL DestroyData ();
extern CHAR* GetPath();
extern SETUP m_setup;

CSetupDlg::CSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDlg)
	//}}AFX_DATA_INIT
}


void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDlg)
	DDX_Control(pDX, IDC_LIST1, m_listbox);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate != 0)
		m_listbox.GetText (m_listbox.GetCurSel(), m_setup.skin);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

BOOL CSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	WIN32_FIND_DATA finddata;
	HANDLE findhandle;
	CHAR * path=GetPath ();
	findhandle = FindFirstFile (CString(path)+CString ("*"),&finddata);
	do
	{
		if (finddata.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			if (finddata.cFileName[0] != '.')
				m_listbox.AddString (finddata.cFileName);
	} while (FindNextFile (findhandle,&finddata) != 0);
	FindClose (findhandle);
	free (path);

	m_listbox.SetCurSel (0);
	m_listbox.SelectString (-1,m_setup.skin);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
