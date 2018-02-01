
// haikang64Dlg.h : 头文件
//

#pragma once
#include "HaikangCam.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "afxcmn.h"
using namespace cv;

// Chaikang64Dlg 对话框
class Chaikang64Dlg : public CDialogEx
{
// 构造
public:
	Chaikang64Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HAIKANG64_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HaikangCam haikang;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void Init_OpenCV(const char* pic_name);
	UINT Open_HaiKang();
	UINT Close_HaiKang();
	static UINT Thread1(void* param);
	static UINT Thread2(void* param);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_width;
	CSliderCtrl m_height;
	afx_msg void OnBnClickedButtonPreprocessing();
};
