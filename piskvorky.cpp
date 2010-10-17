// piskvorky.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "piskvorky.h"
#include "piskvorkyDlg.h"
#include "setupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyApp

BEGIN_MESSAGE_MAP(CPiskvorkyApp, CWinApp)
	//{{AFX_MSG_MAP(CPiskvorkyApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyApp construction

CPiskvorkyApp::CPiskvorkyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPiskvorkyApp object

CPiskvorkyApp theApp;
extern int m_data [spacewidth][spaceheight];
extern int m_animID [spacewidth][spaceheight];
extern int m_phase [spacewidth][spaceheight];
extern CBitmap **m_bmps[7];
extern int m_bmpsCount[7];
extern int m_bmpxsize,m_bmpysize;
extern HCURSOR m_cursor;
extern STATES m_status;
extern HANDLE calculatingThread;
extern CCriticalSection cs_anim,cs_state;
extern int Player;
extern SETUP m_setup;

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyApp initialization
extern void RecalcInfo (int u,int x,int y,int direction,INFOARR* outinfo);

BOOL CPiskvorkyApp::InitInstance()
{

#pragma warning(disable : 4996)
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	int nResponse;
	CPiskvorkyDlg dlg;
	m_pMainWnd = 0;

	SetRegistryKey(_T("Piskvorky"));
	m_setup.skin = GetProfileString ("data","skin","Default");

	dlg.NewGame ();

	if (LoadData ()==TRUE)
	{
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
		m_pMainWnd = 0;
		return TRUE;
	}

	AfxMessageBox (CString ((LPCSTR)IDS_NEWSKIN));
    ShowSetupDlg ();
    DestroyData();

	if (LoadData ()==TRUE)
	{
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
		m_pMainWnd = 0;
		return TRUE;
	}

	m_pMainWnd = 0;
	return FALSE;
}

int CPiskvorkyApp::ExitInstance() 
{
	WriteProfileString ("data","skin",m_setup.skin);

	DestroyData ();
/*	if (calculatingThread != 0)	
	{
		::TerminateThread (calculatingThread,999);
		calculatingThread = 0;
	}*/
	return CWinApp::ExitInstance();
}

CHAR* GetPath()
{
	CHAR *lpnamepart;
	CHAR *lpExeName=GetCommandLine();
	CHAR ExeName[1024];
	CHAR* lppath = new CHAR[1024];
	strcpy (&ExeName[0],lpExeName);
	int i=1;
	while (!((*(lpExeName+i) == '\"')||(*(lpExeName+i) == '\0')))
		i++;
	ExeName[i] = '\0';
	GetFullPathName(&ExeName[1],1024,lppath,&lpnamepart);
	*lpnamepart = '\0';

	return lppath;
}

BOOL LoadData ()
{
	CHAR (filenameformat[7])[8]={"None","Xdrw","Odrw","X","O","Xwin","Owin"};
	CHAR filename[1024],*lppath;
	HANDLE tempbmphandle=0;

	lppath = GetPath ();
	ZeroMemory (&m_bmpsCount,sizeof(m_bmpsCount));
	ZeroMemory (&m_bmps,sizeof(m_bmps));

	for (int c=0;c<7;c++)
	{
		int file=0;
		m_bmps[c] = (CBitmap**) malloc (0);
		do 
		{
			file++;
			sprintf(filename,"%s%s\\%s%04d.bmp",lppath,m_setup.skin,filenameformat[c],file);
			tempbmphandle = LoadImage(0,&filename[0],IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_DEFAULTCOLOR|LR_LOADFROMFILE);
			//DeleteObject (tempbmphandle);
			if (tempbmphandle != 0)
			{
				m_bmps[c] = (CBitmap**) realloc (m_bmps[c], sizeof (CBitmap*) * file);
				m_bmps[c][file-1] = new CBitmap;
				m_bmps[c][file-1]->Attach(tempbmphandle);
			}
		}
		while (tempbmphandle != 0);
		m_bmpsCount[c] = file-1;
		if (file == 1) goto fileopenerror;
	}

	BITMAP bmpinfo;
	m_bmps[0][0]->GetBitmap(&bmpinfo);
	m_bmpxsize = bmpinfo.bmWidth;
	m_bmpysize = bmpinfo.bmHeight;

	sprintf(filename,"%s%s\\cursor.ani",lppath,m_setup.skin);
	m_cursor = 0;
	m_cursor = (HCURSOR)LoadCursorFromFile(&filename[0]);
	if (m_cursor == 0) 
	{
		sprintf(filename,"%s%s\\cursor.cur",lppath,m_setup.skin);
		m_cursor = (HCURSOR)LoadCursorFromFile(&filename[0]);
	}
	free (lppath);
	return TRUE;
fileopenerror:
	CHAR errmsg[1024];
	CString errformat;
	errformat.LoadString(IDS_ERRMSG);
	sprintf(errmsg,errformat,&filename);
	::MessageBox(0,&errmsg[0],"Error",MB_OK);
	free (lppath);
	return FALSE;
}

BOOL DestroyData ()
{
	for (int c2=0;c2<7;c2++)
	{
		for (int c3=0;c3<m_bmpsCount[c2];c3++)
			delete m_bmps[c2][c3];
		if (m_bmps[c2] != 0) free (m_bmps[c2]);
		m_bmps[c2] = 0;
	}
	SetCursor ( LoadCursor (0,IDC_ARROW));
	if (m_cursor != 0) DestroyCursor(m_cursor);
	m_cursor = 0;

	return TRUE;
}

BOOL ShowSetupDlg ()
{
	CSetupDlg dlgSetup;
	CString oldskinname;
	oldskinname = m_setup.skin;

	dlgSetup.DoModal();

	if (m_setup.skin != oldskinname)
	{
		DestroyData ();
		if (LoadData () == FALSE)
		{
			DestroyData ();
			m_setup.skin = oldskinname;
			if (LoadData ()== FALSE) return FALSE;
		}
		RECT rect;
		if (AfxGetApp()->m_pMainWnd != 0)
		{
			AfxGetApp()->m_pMainWnd->GetWindowRect (&rect);
			AfxGetApp()->m_pMainWnd->MoveWindow (rect.left,rect.top,spacewidth*m_bmpxsize+GetSystemMetrics(SM_CXFIXEDFRAME)*2,spaceheight*m_bmpysize+GetSystemMetrics(SM_CYFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION));
			AfxGetApp()->m_pMainWnd->Invalidate (FALSE);
		}
		cs_anim.Lock ();
		{
			ZeroMemory (&m_phase,sizeof(m_phase));
			for (int x = 0;x<spacewidth;x++)
				for (int y = 0;y <spaceheight;y++)
				{
					if ((m_animID[x][y]==3)|(m_animID[x][y]==4)) m_animID[x][y] -= 2;
				}
		}
		cs_anim.Unlock ();
	}
	return TRUE;
}
