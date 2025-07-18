
// CartoTypeDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CartoTypeDemo.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CartoTypeDemoDoc.h"
#include "CartoTypeDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCartoTypeDemoApp

BEGIN_MESSAGE_MAP(CCartoTypeDemoApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CCartoTypeDemoApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
    ON_COMMAND(ID_FILE_OPEN_ONLINE_MAP, &CCartoTypeDemoApp::OnFileOpenOnlineMap)
END_MESSAGE_MAP()


// CCartoTypeDemoApp construction

CCartoTypeDemoApp::CCartoTypeDemoApp():
    m_zoom_factor(2.0)
    {
    m_nAppLook = 0;
	m_bHiColorIcons = TRUE;
    CartoType::String version = "CartoType.CartoTypeDemo.";
    version += CartoType::Version();
    version += ".";
    version += CartoType::Build();
    version += uint16_t(0);
    SetAppID((LPCTSTR)version.Data());

    //_crtBreakAlloc = 1236971;
    }

CCartoTypeDemoApp::~CCartoTypeDemoApp()
    {
    }

// The one and only CCartoTypeDemoApp object

CCartoTypeDemoApp theApp;

BOOL CCartoTypeDemoApp::InitInstance()
    {
	CWinAppEx::InitInstance();
	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("CartoType"));
	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

    if (Initialize() != CartoType::KErrorNone)
        return FALSE;

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_CartoTypeTYPE,
		RUNTIME_CLASS(CCartoTypeDemoDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CCartoTypeDemoView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	    {
		delete pMainFrame;
		return FALSE;
	    }
	m_pMainWnd = pMainFrame;

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

    if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew ||
        cmdInfo.m_nShellCommand == CCommandLineInfo::FileNothing)
        {
        if (m_pRecentFileList)
            {
            ::CString mru = (*m_pRecentFileList)[0];
            if (!mru.IsEmpty())
                OpenDocumentFile(mru);
            }
        }
    else if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
        // Dispatch commands specified on the command line
        if (!ProcessShellCommand(cmdInfo))
            return FALSE;

    // Override saved accelerators with those from this EXE. I don't want accelerators to be customisable.
    theApp.GetKeyboardManager()->ResetAll();

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
    }

int CCartoTypeDemoApp::ExitInstance()
    {
	//TODO: handle additional resources you may have added
	return CWinAppEx::ExitInstance();
    }

void CCartoTypeDemoApp::ShowError(const _TCHAR* aMessage)
    {
    m_pMainWnd->MessageBox(aMessage, _TEXT("CartoType"), MB_OK | MB_ICONERROR);
    }

void CCartoTypeDemoApp::ShowError(const _TCHAR* aMessage, int aError)
    {
    CartoType::String text;
    text.Set((const uint16_t*)aMessage);
    CartoType::String s = CartoType::ErrorString(aError);
    if (!s.IsEmpty())
        {
        text.Append(": ");
        text.Append(s);
        }
    text.Append((uint16_t)0);
    ShowError((LPCTSTR)text.Data());
    }

void CCartoTypeDemoApp::ShowError(const std::string& aMessage,int aError)
    {
    CartoType::String text = aMessage;
    if (aError)
        text += ": " + CartoType::ErrorString(aError);
    text.Append((uint16_t)0);
    ShowError((LPCTSTR)text.Data());
    }

void CCartoTypeDemoApp::ShowError(int aError)
    {
    ShowError(_TEXT("CartoType error"),aError);
    }

CartoType::String CCartoTypeDemoApp::FontPath(const char* aFontName) const
    {
    ::CString font_devel_path("..\\..\\..\\..\\font\\");
    ::CString font_release_path("font\\");
    ::CString font_path = m_app_path + aFontName;
    if (_taccess((LPCTSTR)font_path, 4) == -1)
        {
        font_path = m_app_path + font_devel_path + CString(aFontName);
        if (_taccess((LPCTSTR)font_path, 4) == -1)
            font_path = m_app_path + font_release_path + CString(aFontName);
        }
    CartoType::String s;
    s.Set((const uint16_t*)(LPCTSTR)font_path);
    return s;
    }

CartoType::Result CCartoTypeDemoApp::Initialize()
    {
    // Determine path of executable.
    _TCHAR path[MAX_PATH];
    GetModuleFileName(AfxGetInstanceHandle(),path,MAX_PATH);
    _TCHAR drive[_MAX_DRIVE];
    _TCHAR dir[_MAX_DIR];
    _TCHAR fname[_MAX_FNAME];
    _TCHAR ext[_MAX_EXT];
    _tsplitpath_s(path,drive,dir,fname,ext);
    m_app_path = (CString)drive + dir;

    /*
    Find the default style sheet.
    Paths are searched in this order:
    the application directory;
    the development tree directory;
    the subdirectory style_sheet of the application directory.
    */
    ::CString style_sheet_name("standard.ctstyle");
    ::CString style_sheet_devel_path("..\\..\\..\\..\\style\\");
    ::CString style_sheet_release_path("style_sheet\\");
    ::CString style_sheet_path = m_app_path + style_sheet_name;
    if (_taccess((LPCTSTR)style_sheet_path,4) == -1)
        {
        style_sheet_path = m_app_path + style_sheet_devel_path + style_sheet_name;
        if (_taccess((LPCTSTR)style_sheet_path,4) == -1)
            style_sheet_path = m_app_path + style_sheet_release_path + style_sheet_name;
        }
    SetString(m_default_style_sheet_path,style_sheet_path);

    return CartoType::KErrorNone;
    }

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
    {
    //{{AFX_DATA_INIT(CAboutDlg)
    iTextLine1 = _T("0");
    iTextLine2 = _T("0");
    iTextLine3 = _T("0");
    //}}AFX_DATA_INIT
    }

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
    {
    CDialogEx::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Text(pDX,IDC_ABOUT_TEXT_LINE_1,iTextLine1);
    DDX_Text(pDX,IDC_ABOUT_TEXT_LINE_2,iTextLine2);
    DDX_Text(pDX,IDC_ABOUT_TEXT_LINE_3,iTextLine3);
    //}}AFX_DATA_MAP
    }

BEGIN_MESSAGE_MAP(CAboutDlg,CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CCartoTypeDemoApp::OnAppAbout()
    {
	CAboutDlg about_dialog;
    SetString(about_dialog.iTextLine1,CartoType::String("CartoType Windows demo"));
    CartoType::String s = "built using CartoType ";
    s += CartoType::Version();
    s += ".";
    s += CartoType::Build();
    SetString(about_dialog.iTextLine2,s);
    SetString(about_dialog.iTextLine3,"");

    about_dialog.DoModal();
    }

void CCartoTypeDemoApp::OnFileOpenOnlineMap()
    {
    auto t = m_pDocManager->GetBestTemplate(_T("test.ctm1"));
    //t->OpenDocumentFile(_T("https://mapscartotype.fjl.uk/denmark.ctm1"),false,true);
    t->OpenDocumentFile(_T("http://localhost/maps/denmark.ctm1"),false,true);
    }

// CCartoTypeDemoApp customization load/save methods

void CCartoTypeDemoApp::PreLoadState()
{
}

void CCartoTypeDemoApp::LoadCustomState()
{
}

void CCartoTypeDemoApp::SaveCustomState()
{
}

// CCartoTypeDemoApp message handlers



