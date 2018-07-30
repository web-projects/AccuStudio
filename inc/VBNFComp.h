#pragma once

#include "resource.h"

typedef struct template_rec
{
	LPSTR pTemplateName;
	LPSTR pTemplateValue;
	LPSTR pTemplateFile;
	int	 LineNum, ColNum;
} TEMPLATE_REC;

extern int NumTemplates;
extern TEMPLATE_REC * ptemplates;

// CStudioFile form view

class CStudioFile : public CStdioFile
{
	DECLARE_DYNCREATE(CStudioFile)

	CStudioFile();           // protected constructor used by dynamic creation
//protected:
	virtual ~CStudioFile();
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);
	virtual BOOL ReadString(CString& rString);
	virtual void Close();
	void SetTemplate(CString& rString);
	BOOL IsInTemplate(void);
	LPSTR GetStringPtr(void);
	BOOL GetData(void);
	BOOL InTemplate;
	CString Template;
	int CurCol;
	LPSTR lopLineBuf;
	char olCurChar;
	int lLineNumber;
public:
	enum { IDD = IDD_STUDIOFILE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


int VBNFComp(LPSTR projname, FILE * pout, CRichEditCtrl * poutct);