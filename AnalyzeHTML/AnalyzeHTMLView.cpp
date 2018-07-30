// AnalyzeHTMLView.cpp : implementation of the CAnalyzeHTMLView class
//

#include "stdafx.h"
#include "AnalyzeHTML.h"
#include "FindURL.h"
#include "AnalyzeHTMLDoc.h"
#include "AnalyzeHTMLView.h"
#include "SetRCName.h"
#include <mshtml.h>
#include <comutil.h>
#include <oleidl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAnalyzeHTMLView * phView;


// CAnalyzeHTMLView

IMPLEMENT_DYNCREATE(CAnalyzeHTMLView, CHtmlView)

BEGIN_MESSAGE_MAP(CAnalyzeHTMLView, CHtmlView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
	ON_COMMAND(ID_FILE_OPEN, &CAnalyzeHTMLView::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CAnalyzeHTMLView::OnUpdateFileOpen)
	ON_COMMAND(ID_GENERATE_CREATERCFILE, &CAnalyzeHTMLView::OnGenerateCreatercfile)
END_MESSAGE_MAP()

// CAnalyzeHTMLView construction/destruction

CAnalyzeHTMLView::CAnalyzeHTMLView()
{
	// TODO: add construction code here

}

CAnalyzeHTMLView::~CAnalyzeHTMLView()
{
}

BOOL CAnalyzeHTMLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CHtmlView::PreCreateWindow(cs);
}

void CAnalyzeHTMLView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
	phView = this;
	OnFileOpen();
//	Navigate2(_T("http://edit.finance.yahoo.com/e6?.portkey=pf_1&.src=my&.done=http%3a%2f%2fmy.yahoo.com%2findex.html"),
//	Navigate2(_T("http://otis.absolute.net/pcdemo/"),
//	Navigate2(_T("http://www.microsoft.com"),
//	Navigate2(_T("http://192.168.2.95/htmldemo/ControlDemo.html"),
//	Navigate2(_T("c:/MyProjects/AccuStudio/AnalyzeHTML/1-Menu.htm"),
//		Navigate2(_T("http://otis.absolute.net/IDVelocity/"),

//		NULL,NULL);
}


// CAnalyzeHTMLView printing



// CAnalyzeHTMLView diagnostics

#ifdef _DEBUG
void CAnalyzeHTMLView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CAnalyzeHTMLView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CAnalyzeHTMLDoc* CAnalyzeHTMLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnalyzeHTMLDoc)));
	return (CAnalyzeHTMLDoc*)m_pDocument;
}
#endif //_DEBUG


// CAnalyzeHTMLView message handlers

void CAnalyzeHTMLView::OnFileOpen()
{
	CFindURL * purl = new CFindURL;
	int res = (int)purl->DoModal();
	if (res == IDOK)
	{
		CString ts;
		if (purl->m_urlname.GetLength())
		{
			Navigate2(purl->m_urlname);
		}
	}
	delete purl;
}

void CAnalyzeHTMLView::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

FILE * f, * fi, *flog;
BSTR bs;
CString cID = " ";
int cid = 1000;

BOOL GetString(CString & cs)
{
	if (!bs)
		return FALSE;
	cs = bs;
	SysFreeString(bs);
	return TRUE;
}
void MakeID()
{
	char s[256];
	if (cID != " ")
		return;
// we have no ID - suppy one.
	sprintf(s, "%i", cid++);
	cID = s;
	fprintf(flog, "Warning: the next INPUT item has no ID value, %s supplied", s);
}

void CAnalyzeHTMLView::GenRC(IHTMLElementCollection *p)
{
	BOOL inform = FALSE;
	long xoffset,xwidth,yoffset,height;
	long gxoffset,gyoffset;
	char gs[256], s[256];
	CString formname;
	CString ts, cTAG, cNAME, cTYPE, cVALUE;
  IHTMLElement *pElement, *parent, *ptemp;
	IHTMLInputTextElement	* pInputTextElement;
	IHTMLInputElement * pInputElement;
	IHTMLInputButtonElement * pButtonElement;
	IHTMLSelectElement * pListElement;
	IHTMLTextAreaElement * pTextAreaElement;
	IHTMLFormElement * pFormElement;
	long i, numentries;
	IDispatch  * pitem;
	VARIANT index;
	HRESULT res;
	double xscale, yscale;
	cid = 1000;
	res = p->get_length(&numentries);

	// fixed scale factors, for now
	xscale = 0.5;
	yscale = 0.5;
	for (i = 0; i < numentries; i++)
	{
		index.lVal = i;
		index.vt = VT_I4;
		res = p->item(index, index, &pitem);
		if (res || !pitem)
			continue;
    res = pitem->QueryInterface( IID_IHTMLElement, (void **) &pElement);
		if (res || !pElement)
		{
			pitem->Release();
			continue;
		}
		HRESULT eres = pitem->QueryInterface( IID_IHTMLInputTextElement, (void **) &pInputTextElement);
		HRESULT ieres = pitem->QueryInterface( IID_IHTMLInputElement, (void **) &pInputElement);
		HRESULT bres = pitem->QueryInterface( IID_IHTMLInputButtonElement, (void **) &pButtonElement);
		HRESULT lres = pitem->QueryInterface( IID_IHTMLSelectElement, (void **) &pListElement);
		HRESULT tares = pitem->QueryInterface( IID_IHTMLTextAreaElement, (void **) &pTextAreaElement);
		HRESULT formres = pitem->QueryInterface( IID_IHTMLFormElement, (void **) &pFormElement);

		gs[0] = 0;
		xoffset = xwidth = yoffset = height = 0;
		gxoffset = gyoffset = 0;
		ptemp = pElement;		// search for container object
		while(1)
		{
			long left, top;
			res = ptemp->get_offsetParent((IHTMLElement **) &parent);
			if (res || !parent)
				break;
			left = top = 0;
			res = parent->get_offsetLeft(&left);
			res = parent->get_offsetTop(&top);	
			gxoffset += left;
			gyoffset += top;
			ptemp = parent;
		}
		res = pElement->get_offsetLeft(&xoffset);
		res = pElement->get_offsetWidth(&xwidth);
		res = pElement->get_offsetTop(&yoffset);
		res = pElement->get_offsetHeight(&height);
		xoffset += gxoffset;
		yoffset += gyoffset;

		xoffset = (long) (xscale * xoffset);
		xwidth = (long) (xscale * xwidth);
		yoffset = (long) (yscale * yoffset);
		height = (long) (yscale * height);

		bs = NULL;
		cID = cTAG = " ";
		res = pElement->get_id(&bs);
		if (!res && bs)
		{
			GetString(cID);
			res = res;
		}
		sprintf(s, "ID= %s", cID);
		strcat(gs, s);

		bs = NULL;
		ts = ' ';
		res = pElement->get_className(&bs);
		if (!res && bs)
		{
			GetString(ts);
		}
		sprintf(s, ",CLASS= %s", ts);
		strcat(gs, s);

		bs = NULL;
		cTAG = ' ';
		res = pElement->get_tagName(&bs);
		if (!res && bs)
		{
			GetString(cTAG);
		}
		sprintf(s, ",TAG= %s", cTAG);
		strcat(gs, s);

		if (!formres && pFormElement)
		{
			if (inform)																	
				fprintf(f, "END\n");
			inform = TRUE;
			bs = NULL;
			pFormElement->get_name(&bs);
			if (!GetString(formname))
				formname = "noname";
			if (cID == " ")
				cID = "0";
			fprintf(fi, "#define %s %s\n", formname, cID);
			fprintf(f, "\n%s DIALOG DISCARDABLE %i, %i, %i, %i\n", 
				formname, 0, 0, xwidth,  height + yoffset + 18);		// include caption
			fprintf(f,"STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU\n");
			fprintf(f, "CAPTION \"%s\"\n", formname);
			fprintf(f, "BEGIN\n");
		}

		cTYPE = cNAME = cVALUE = ' ';	
		if (!ieres && pInputElement && cTAG == "INPUT")
		{	// General INPUT element
			res = pInputElement->get_type(&bs);
			if (!res && bs)
			{
				cTYPE = bs;
				sprintf(s, "\n\tTYPE= %s", cTYPE);
				strcat(gs, s);
			}
			res = pInputElement->get_name(&bs);
			if (!res && bs)
			{
				cNAME = bs;
				sprintf(s, ",NAME= %s", cNAME);
				strcat(gs, s);
			}
			res = pInputElement->get_value(&bs);
			if (!res && bs)
			{
				cVALUE = bs;
				sprintf(s, ",VALUE= %s", cVALUE);
				strcat(gs, s);
			}
			if (cVALUE == " ")
				cVALUE = cNAME;
			sprintf(s, ",%i,%i,%i,%i", xoffset, yoffset, xwidth,  height),
			strcat(gs,s);
			if (cTYPE == "text")
			{// Text edit control
				MakeID();
				fprintf(f, "\tEDITTEXT ID_%s,%i,%i,%i,%i,0\n",
					cID, xoffset, yoffset, xwidth,  height);
				fprintf(fi, "#define ID_%s  %s\n", cID, cID);
			}
			else if (cTYPE == "button" || cTYPE == "submit")
			{// Button control
				MakeID();
				fprintf(f, "\tPUSHBUTTON \"%s\",ID_%s,%i,%i,%i,%i,0\n",
					cVALUE, cID, xoffset, yoffset, xwidth,  height);
				fprintf(fi, "#define ID_%s  %s\n", cID, cID);
			}
			else if (cTYPE == "checkbox")
			{
				MakeID();
				fprintf(f, "\tCONTROL \"%s\",ID_%s,\"Button\",BS_CHECKBOX,%i,%i,%i,%i\n",
					cVALUE, cID, xoffset, yoffset, xwidth,  height);
				fprintf(fi, "#define ID_%s  %s\n", cID, cID);
			}
			else if (cTYPE == "radio")
			{
				MakeID();
				fprintf(f, "\tCONTROL \"%s\",ID_%s,\"Button\",BS_AUTORADIOBUTTON,%i,%i,%i,%i\n",
					cVALUE, cID, xoffset, yoffset, xwidth,  height);
				fprintf(fi, "#define ID_%s  %s\n", cID, cID);
			}
			else
				fprintf(flog,"Unsupported INPUT control/n");
		}
		else if (!lres && pListElement)
		{
			res = pListElement->get_type(&bs);
			if (!res && bs)
			{
				cTYPE = bs;
				sprintf(s, "\n\tTYPE= %s", cTYPE);
				strcat(gs, s);
			}
			res = pListElement->get_name(&bs);
			if (!res && bs)
			{
				cNAME = bs;
				sprintf(s, ",NAME= %s", cNAME);
				strcat(gs, s);
			}
			res = pListElement->get_value(&bs);
			if (!res && bs)
			{
				cVALUE = bs;
			}
			sprintf(s, ",%i,%i,%i,%i", xoffset, yoffset, xwidth,  height),
			strcat(gs,s);

			MakeID();
			fprintf(f, "\tCOMBOBOX ID_%s,%i,%i,%i,%i,0\n",
				cID, xoffset, yoffset, xwidth,  height);
			fprintf(fi, "#define ID_%s  %s\n", cID, cID);
		}
		else if (!tares && pTextAreaElement)
		{
			res = pTextAreaElement->get_type(&bs);
			if (!res && bs)
			{
				cTYPE = bs;
				sprintf(s, "\n\tTYPE= %s", cTYPE);
				strcat(gs, s);
			}
			res = pTextAreaElement->get_name(&bs);
			if (!res && bs)
			{
				cNAME = bs;
				sprintf(s, ",NAME= %s", cNAME);
				strcat(gs, s);
			}
			sprintf(s, ",%i,%i,%i,%i", xoffset, yoffset, xwidth,  height),
			strcat(gs,s);
			{// Text edit control
				MakeID();
				fprintf(f, "\tEDITTEXT ID_%s,%i,%i,%i,%i,0\n",
					cID, xoffset, yoffset, xwidth,  height);
				fprintf(fi, "#define ID_%s  %s\n", cID, cID);
			}
		}

		if (gs[0])
		{
			fprintf(flog, "%s\n", gs);
		}
cleanup:
		pitem->Release();
		if (pElement)
			pElement->Release();
		if (pInputTextElement)
			pInputTextElement->Release();
		if (pButtonElement)
			pButtonElement->Release();
		if (pListElement)
			pListElement->Release();
		if (pTextAreaElement)
			pTextAreaElement->Release();
	}
	if (inform)
		fprintf(f, "END\n");
}

void CAnalyzeHTMLView::OnGenerateCreatercfile()
{
	IHTMLElementCollection *p;
	LPDISPATCH	lpd = GetHtmlDocument();
	HRESULT res;	
	IHTMLDocument2* pDoc;	
  char buf[256] = "Create RC File";
	CString ct;

	if (!lpd)
	{
		MessageBox("Cannot retrieve HTML Document");
		return;
	}
	res = lpd->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
	if (res)
	{
		MessageBox("Cannot retrieve HTML Document");
		return;
	}
	res = pDoc->get_all(&p);
	if (res)
	{
		MessageBox("Cannot 'get_all'");
		return;
	}
	// Put up a standard file dialog to allow output file selection
	CFileDialog dlg(NULL, _T("rc"), NULL, OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, 
    _T("RC file (*.rc)||"));
//  dlg.m_ofn.lpstrTitle  = (LPSTR) & buf;
 	if( dlg.DoModal() != IDOK )
		return;
  ct = dlg.GetFileTitle();
	if (ct.GetLength() == 0)
		return;
	sprintf(buf, "%s.rc",ct);
	f = fopen(buf, "w");
	if (!f)
	{
		MessageBox(_T("Unable to create RC File"),buf);
	}
	sprintf(buf, "%s.log",ct);
	flog = fopen(buf, "w");
	if (!flog)
	{
		MessageBox(_T("Unable to create Log File"),buf);
	}
	sprintf(buf, "%s.h",ct);
	fi = fopen(buf, "w");
	if (!fi)
	{
		MessageBox(_T("Unable to create include File"),buf);
	}
	fprintf(flog, "Analyzing HTML File %s\n\n", ct);
	fprintf(f, "#include \"%s\"\n", buf);
	GenRC(p);
	fclose(f);
	fclose(fi);
	fclose(flog);
	p->Release();
	lpd->Release();
	pDoc->Release();
}

