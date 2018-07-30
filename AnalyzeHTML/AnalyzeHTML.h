// AnalyzeHTML.h : main header file for the AnalyzeHTML application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAnalyzeHTMLApp:
// See AnalyzeHTML.cpp for the implementation of this class
//

class CAnalyzeHTMLApp : public CWinApp
{
public:
	CAnalyzeHTMLApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
};

extern CAnalyzeHTMLApp theApp;