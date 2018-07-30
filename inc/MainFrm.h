// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__66B77AB5_63AE_468D_988A_93C02379CB42__INCLUDED_)
#define AFX_MAINFRM_H__66B77AB5_63AE_468D_988A_93C02379CB42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
  CStatusBar * CMainFrame::GetStatusBar();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileCloseProject();
	afx_msg void OnFileNewProject();
	afx_msg void OnFileOpenProject();
	afx_msg void OnViewProject();
	afx_msg void OnFileSaveProject();
	afx_msg void OnProjectAddgrammarfiles();
	afx_msg void OnProjectAddGrammarset();
	afx_msg void OnBuildBuildProject();
	afx_msg void OnBuildBuildfromdefinitionfile();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateBuildBuildProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBuildBuildfromdefinitionfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileCloseProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNewProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveProject(CCmdUI* pCmdUI);
	afx_msg void OnViewGrammarWizard();
	afx_msg void OnUpdateViewGrammarWizard(CCmdUI* pCmdUI);
	afx_msg void OnViewResults();
	afx_msg void OnUpdateViewResults(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddGrammarset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddgrammarfiles(CCmdUI* pCmdUI);
	afx_msg void OnViewObjectwizard();
	afx_msg void OnUpdateViewObjectwizard(CCmdUI* pCmdUI);
	afx_msg void OnGrammarExercise();
	afx_msg void OnUpdateGrammarExercise(CCmdUI* pCmdUI);
	afx_msg void OnToolsAnalysehtml();
	afx_msg void OnGrammarBuildlibrary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnFileSaveas();
	afx_msg void OnProjectSettings();
	afx_msg void OnUpdateProjectSettings(CCmdUI *pCmdUI);
public:
	afx_msg void OnProjectUpdateprojectfromresourcefile32825();
public:
	afx_msg void OnUpdateProjectUpdateprojectfromresourcefile32825(CCmdUI *pCmdUI);
public:
	afx_msg void OnProjectAddresourcesfromresourcefile();
public:
	afx_msg void OnUpdateProjectAddresourcesfromresourcefile(CCmdUI *pCmdUI);
	afx_msg void OnHelpSemanticcodes();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__66B77AB5_63AE_468D_988A_93C02379CB42__INCLUDED_)
