
// FormatToolDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "FormatTool.h"
#include "FormatToolDlg.h"
#include "afxdialogex.h"

#include "Layout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFormatToolDlg 對話方塊



CFormatToolDlg::CFormatToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FORMATTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFormatToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, setMBR_ctrl);
	DDX_Control(pDX, IDC_COMBO2, fileSysType_ctrl);
	DDX_Control(pDX, IDC_EDIT1, hidSec_ctrl);
	DDX_Control(pDX, IDC_EDIT2, rsvdSec_ctrl);
	DDX_Control(pDX, IDC_EDIT3, cluSize_ctrl);
}

BEGIN_MESSAGE_MAP(CFormatToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

// CFormatToolDlg 訊息處理常式

BOOL CFormatToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定

	// initial options
	// set MBD combo box
	setMBR_ctrl.InsertString(0, _T("Yes"));
	setMBR_ctrl.InsertString(1, _T("No"));
	setMBR_ctrl.SetCurSel(0);
	SetDropDownHeight(&setMBR_ctrl, 2);

	// set file system combo box
	//fileSysType_ctrl.InsertString(1, _T("FAT16")); // optional
	fileSysType_ctrl.InsertString(0, _T("FAT32"));
	//fileSysType_ctrl.InsertString(2, _T("exFAT")); // optional
	fileSysType_ctrl.SetCurSel(0);
	SetDropDownHeight(&fileSysType_ctrl, 3);

	// set edit control default
	cluSize_ctrl.SetWindowText(_T("64"));


	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CFormatToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CFormatToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

