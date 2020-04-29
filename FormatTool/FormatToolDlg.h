
// FormatToolDlg.h: 標頭檔
//

#pragma once


// CFormatToolDlg 對話方塊
class CFormatToolDlg : public CDialogEx
{
// 建構
public:
	CFormatToolDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMATTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox setMBR_ctrl;
	CComboBox fileSysType_ctrl;
	CEdit hidSec_ctrl;
	CEdit rsvdSec_ctrl;
	CComboBox cluSize_ctrl;
	CComboBox device_ctrl;
	afx_msg void OnBnClickedFormatbtn();
	afx_msg void OnCbnSelchangeCombo1();
	void OnOK(); //override
	afx_msg void OnCbnDropdownCombo4();
};
