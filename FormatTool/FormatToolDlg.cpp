
// FormatToolDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "FormatTool.h"
#include "FormatToolDlg.h"
#include "afxdialogex.h"

#include "Layout.h"
#include "format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFormatToolDlg 對話方塊

int enum_usb_disk(char usb_paths[], DWORD usb_capacity[], int cnt)
{
	int usb_disk_cnt = 0;

	char disk_path[5] = { 0 };
	DWORD all_disk = GetLogicalDrives();

	int i = 0;
	DWORD bytes_returned = 0;
	while (all_disk && usb_disk_cnt < cnt)
	{
		if ((all_disk & 0x1) == 1)
		{
			sprintf_s(disk_path, "%c:", 'A' + i);


			if (GetDriveTypeA(disk_path) == DRIVE_REMOVABLE)
			{
				// get device capacity
				HANDLE hDevice = getHandle('A' + i);
				if (hDevice == INVALID_HANDLE_VALUE) {
					TRACE("Open %s failed.", disk_path);
					CloseHandle(hDevice);
					return -1;
				}
				DWORD capacity_sec = getCapacity(hDevice);
				if (capacity_sec == 0) {
					CloseHandle(hDevice);
					continue; // skip invalid device (include card reader)
				}

				usb_paths[usb_disk_cnt] = 'A' + i;
				usb_capacity[usb_disk_cnt++] = capacity_sec;

				CloseHandle(hDevice);
			}
		}
		all_disk = all_disk >> 1;
		i++;
	}

	return usb_disk_cnt;
}

char* cstr2str(CString cstr) {
	const size_t newsizew = (cstr.GetLength() + 1) * 2;
	char* nstringw = new char[newsizew];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, nstringw, newsizew, cstr, _TRUNCATE);

	return nstringw;
}

CFormatToolDlg::CFormatToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FORMATTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFormatToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO4, device_ctrl);
	DDX_Control(pDX, IDC_COMBO1, setMBR_ctrl);
	DDX_Control(pDX, IDC_COMBO2, fileSysType_ctrl);
	DDX_Control(pDX, IDC_EDIT1, hidSec_ctrl);
	DDX_Control(pDX, IDC_EDIT2, rsvdSec_ctrl);
	DDX_Control(pDX, IDC_COMBO3, cluSize_ctrl);
}

BEGIN_MESSAGE_MAP(CFormatToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_Format_btn, &CFormatToolDlg::OnBnClickedFormatbtn)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CFormatToolDlg::OnCbnSelchangeCombo1)
	ON_CBN_DROPDOWN(IDC_COMBO4, &CFormatToolDlg::OnCbnDropdownCombo4)
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
	fileSysType_ctrl.InsertString(0, _T("FAT16")); // optional
	fileSysType_ctrl.InsertString(1, _T("FAT32"));
	fileSysType_ctrl.InsertString(2, _T("exFAT")); // optional
	fileSysType_ctrl.SetCurSel(1);
	SetDropDownHeight(&fileSysType_ctrl, 3);

	// set edit control default
	cluSize_ctrl.InsertString(0, _T("512 Bytes"));
	cluSize_ctrl.InsertString(1, _T("1 KB"));
	cluSize_ctrl.InsertString(2, _T("2 KB"));
	cluSize_ctrl.InsertString(3, _T("4 KB"));
	cluSize_ctrl.InsertString(4, _T("8 KB"));
	cluSize_ctrl.InsertString(5, _T("16 KB"));
	cluSize_ctrl.InsertString(6, _T("32 KB"));
	cluSize_ctrl.InsertString(7, _T("64 KB"));
	cluSize_ctrl.SetCurSel(7);
	SetDropDownHeight(&cluSize_ctrl, 5);

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



void CFormatToolDlg::OnBnClickedFormatbtn()
{
	// get value
	CString text;
	DWORD hidSec_num, rsvdSec_num, cluSize_val;
	char device_name;
	bool setMBR_val;

	// get selected device
	int device_idx = device_ctrl.GetCurSel();
	if (device_idx == CB_ERR) {
		MessageBox(_T("Must select device."), _T("Error"), MB_ICONERROR);
		return;
	}
	else {
		char* device_name_w_capacity;
		device_ctrl.GetLBText(device_idx, text);
		device_name_w_capacity = cstr2str(text);
		device_name = device_name_w_capacity[0];
		delete[] device_name_w_capacity;
	}

	// get MBR setup
	int setMBR_idx = setMBR_ctrl.GetCurSel();
	setMBR_val = setMBR_idx == 0 ? TRUE : FALSE;

	// get hidden sector number
	hidSec_ctrl.GetWindowText(text);
	if (text.IsEmpty()) {
		MessageBox(_T("Number of hidden sectors can't be empty."), _T("Error"), MB_ICONERROR);
		return;
	}
	else {
		hidSec_num = _ttoi(text);

		if (hidSec_num < 0) {
			MessageBox(_T("Number of hidden sectors can't be negative."), _T("Error"), MB_ICONERROR);
			return;
		}
		else if (setMBR_val && hidSec_num < 1) {
			MessageBox(_T("There must be at least 1 sector with MBR."), _T("Error"), MB_ICONERROR);
			return;
		}
	}

	// get reserved sector number
	rsvdSec_ctrl.GetWindowText(text);
	if (text.IsEmpty()) {
		MessageBox(_T("Number of reserved sectors can't be empty."), _T("Error"), MB_ICONERROR);
		return;
	}
	else {
		rsvdSec_num = _ttoi(text);

		if (rsvdSec_num < 0) {
			MessageBox(_T("Number of reserved sectors can't be negative."), _T("Error"), MB_ICONERROR);
			return;
		}
		else if (rsvdSec_num < 1) {
			MessageBox(_T("There must be at least 1 sector with DBR."), _T("Error"), MB_ICONERROR);
			return;
		}
		else if (rsvdSec_num > 0xFFFF) {
			// Hidden sector is only 2Bytes
			MessageBox(_T("Hidden sector is too large."), _T("Error"), MB_ICONERROR);
			return;
		}
	}

	// get cluster size
	int cluSize_idx = cluSize_ctrl.GetCurSel();
	switch (cluSize_idx)
	{
	case 0:
		cluSize_val = 512;
		break;
	case 1:
		cluSize_val = (1 << 10);
		break;
	case 2:
		cluSize_val = 2 * (1 << 10);
		break;
	case 3:
		cluSize_val = 4 * (1 << 10);
		break;
	case 4:
		cluSize_val = 8 * (1 << 10);
		break;
	case 5:
		cluSize_val = 16 * (1 << 10);
		break;
	case 6:
		cluSize_val = 32 * (1 << 10);
		break;
	case 7:
		cluSize_val = 64 * (1 << 10);
		break;
	default:
		cluSize_val = 64 * (1 << 10);
		break;
	}

	TRACE("\n[MSG] Selected device: %c:\n", device_name);
	TRACE(_T("\n[MSG] If set MBR: %s\n"), setMBR_val ? _T("Yes") : _T("No"));
	TRACE(_T("\n[MSG] Hidden sector number: %u\n"), hidSec_num);
	TRACE(_T("\n[MSG] Reserved sector number: %u\n"), rsvdSec_num);
	TRACE(_T("\n[MSG] Cluster size: %u\n"), cluSize_val);

	// initial device handle
	HANDLE hDevice = getHandle(device_name);
	if (hDevice == INVALID_HANDLE_VALUE) {
		MessageBox(_T("Open device failed."), _T("Error"), MB_ICONERROR);
		CloseHandle(hDevice);
		return;
	}

	// check upper limitation of hidden + reserved
	DWORD capacity_sec = getCapacity(hDevice);
	DWORD secPerClu = cluSize_val / PHYSICAL_SECTOR_SIZE;
	DWORD must_rsvd_sec = 2 + 6 * secPerClu; // 2 sector for FAT and 6 cluster for system files
	if (hidSec_num + rsvdSec_num + must_rsvd_sec > capacity_sec) {
		CString msg;
		msg.Format(_T("Upper bound number of hidden sector and rsvd sector: %u"), capacity_sec - must_rsvd_sec);
		MessageBox(msg, _T("Error"), MB_ICONERROR);
		CloseHandle(hDevice);
		return;
	}

	if (capacity_sec == 0) {
		MessageBox(_T("Get capacity failed."), _T("Error"), MB_ICONERROR);
		CloseHandle(hDevice);
		return;
	}

	bool ret = format(hDevice, capacity_sec, setMBR_val, hidSec_num, rsvdSec_num, cluSize_val);
	CloseHandle(hDevice);

	if(ret){
		MessageBox(_T("Format succeeded."), _T("Information"), MB_ICONINFORMATION);
	}
	else {
		MessageBox(_T("Format failed."), _T("Error"), MB_ICONERROR);
	}
}

void CFormatToolDlg::OnOK()
{
	OnBnClickedFormatbtn();
}

void CFormatToolDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	int setMBR_idx = setMBR_ctrl.GetCurSel();

	if (setMBR_idx == 0) {
		hidSec_ctrl.EnableWindow(TRUE);
	}
	else {
		hidSec_ctrl.SetWindowText(_T("0"));
		hidSec_ctrl.EnableWindow(FALSE);
	}
}

void CFormatToolDlg::OnCbnDropdownCombo4()
{
	// empty options
	device_ctrl.ResetContent();
	
	// set device combo box
	char usb_volume[8] = { 0 };
	DWORD usb_capacity_sec[8];
	int usb_cnt = enum_usb_disk(usb_volume, usb_capacity_sec, 8);
	if (usb_cnt == -1) {
		MessageBox(_T("Enumerate usb disk failed."), _T("Error"), MB_ICONERROR);
	}
	else {
		for (int i = 0; i < usb_cnt; i++) {
			CString text;
			DWORD capacity_MB = (usb_capacity_sec[i] >> 20) * PHYSICAL_SECTOR_SIZE; // MB
			if (capacity_MB > 1024) {
				double capacity_GB = (double)capacity_MB / 1024;
				text.Format(_T("%c: (%.1f GB)"), usb_volume[i], capacity_GB);
			}
			else {
				text.Format(_T("%c: (%u MB)"), usb_volume[i], capacity_MB);
			}
			device_ctrl.InsertString(i, text);
		}
	}
	SetDropDownHeight(&device_ctrl, usb_cnt);
}
