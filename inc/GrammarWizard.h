#if !defined(AFX_GRAMMARWIZARD_H__41298195_ECF1_4646_A414_ACC121FFFBB2__INCLUDED_)
#define AFX_GRAMMARWIZARD_H__41298195_ECF1_4646_A414_ACC121FFFBB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrammarWizard.h : header file
//

#include "project.h"

/////////////////////////////////////////////////////////////////////////////
// CGrammarWizard dialog

class CGrammarWizard : public CDialog
{
// Construction
public:

	CGrammarWizard(CWnd* pParent = NULL);   // standard constructor

  BOOL CheckList();
  BOOL BldDialogShell(void);
  void SetSpeechObject(LPCONTROLRO pc);
	void PrerecCommon(LPDIALOGRO pdres);

// Dialog Data
	//{{AFX_DATA(CGrammarWizard)
	enum { IDD = IDD_GRAMMAR_WIZARD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrammarWizard)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL DestroyWindow();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrammarWizard)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusGramname();
	afx_msg void OnSetfocusControlname();
	afx_msg void OnSetfocusFocusPrompt();
	afx_msg void OnSetfocusDataEntryPrompt();
	virtual void OnCancel();
	afx_msg void OnNextControl();
	afx_msg void OnSelchangeDlg();
	afx_msg void OnSetfocusPhraseList();
	afx_msg void OnBuild();
	afx_msg void OnGrammarBuild();
	afx_msg void OnAdd();
	afx_msg void OnSubtract();
	afx_msg void OnClose();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnDeadd();
	afx_msg void OnRclickDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturnDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickDataEntryPrompt(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnSelchangeDlgControl();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPrerec();
public:
	//afx_msg void OnBnClickedPrerec2();
	afx_msg BOOL CreateDialogShell(CString gramname, LPDIALOGRO lprun);
};

/////////////////////////////////////////////////////////////////////////////
// CObjectWizard dialog

class CObjectWizard : public CDialog
{
// Construction
public:
	CObjectWizard(CWnd* pParent = NULL);   // standard constructor

  void SetSpeechObject(LPCONTROLRO pc);
  int SetCandidates(LPCONTROLRO pnode);

// Dialog Data
	//{{AFX_DATA(CObjectWizard)
	enum { IDD = IDD_OBJECT_WIZARD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectWizard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectWizard)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnNextControl();
	afx_msg void OnSelchangeDlg();
	afx_msg void OnAttach();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnSelchangeDlgControl();
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CGramExerciser dialog

class CGramExerciser : public CDialog
{
// Construction
public:
	CGramExerciser(CWnd* pParent = NULL);   // standard constructor

	BOOL IsRecognizing, SpeechOpened;
	BOOL OpenSpeech(void);
  void SetSpeechObject(LPCONTROLRO pc);
  int SetCandidates(LPCONTROLRO pnode);

// Dialog Data
	//{{AFX_DATA(CGramExerciser)
	enum { IDD = IDD_GRAMMAR_EXERSIZER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGramExerciser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGramExerciser)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnNextControl();
	afx_msg void OnSelchangeDlg();
	afx_msg void OnRecognize();
	afx_msg void OnSelchangeRecResults();
	//}}AFX_MSG
	afx_msg void OnSelchangeDlgControl();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAMMARWIZARD_H__41298195_ECF1_4646_A414_ACC121FFFBB2__INCLUDED_)
