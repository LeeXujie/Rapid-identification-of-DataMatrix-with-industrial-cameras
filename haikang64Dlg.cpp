// haikang64Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "haikang64.h"
#include "haikang64Dlg.h"
#include "afxdialogex.h"
#include "CTimer.h"
#include "dmtx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

volatile int camera_flag;
bool dmtx_flag1, dmtx_flag2, preprocessing_flag;
int cam, pic_width, pic_height, iTime = 1000;
Rect rect;
Mat dst, dmtx_img1, dmtx_img2;
IplImage* HK_Image;
CTimer* ptime = new CTimer;
static int fps0, fps0_count, fps1, fps1_count, num_count;
CString code_info;
int rect_width = 200, rect_height = 200;
DmtxTime *m_timeout = new DmtxTime;
HANDLE hMutex;
Mutex mut;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Chaikang64Dlg 对话框



Chaikang64Dlg::Chaikang64Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Chaikang64Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Chaikang64Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_width);
	DDX_Control(pDX, IDC_SLIDER2, m_height);
}

BEGIN_MESSAGE_MAP(Chaikang64Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Chaikang64Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Chaikang64Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &Chaikang64Dlg::OnBnClickedButtonClear)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_Preprocessing, &Chaikang64Dlg::OnBnClickedButtonPreprocessing)
END_MESSAGE_MAP()


// Chaikang64Dlg 消息处理程序

BOOL Chaikang64Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	GetDlgItem(IDC_SLIDER1)->EnableWindow(false);
	GetDlgItem(IDC_SLIDER2)->EnableWindow(false);
	// TODO:  在此添加额外的初始化代码
	Init_OpenCV("res/bluesky.bmp");
	CString sstr;
	sstr.Format(_T("%d"), num_count);
	GetDlgItem(IDC_EDIT_COUNT)->SetWindowText(sstr);
	m_width.SetRange(10, 720);
	m_width.SetTicFreq(1);
	m_width.SetPos(rect_width);
	m_height.SetRange(10, 480);
	m_height.SetTicFreq(1);
	m_height.SetPos(rect_height);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Chaikang64Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Chaikang64Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Chaikang64Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Chaikang64Dlg::Init_OpenCV(const char* pic_name)
{
	namedWindow("view", WINDOW_AUTOSIZE);
	HWND hwnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hwnd);
	::SetParent(hwnd, GetDlgItem(IDC_STATIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	CRect crect;
	pWnd->GetClientRect(&crect);
	pic_width = crect.Width();
	pic_height = crect.Height();
	Mat frame = imread(pic_name);
	cv::resize(frame, dst, Size(pic_width, pic_height), 0, 0, 1);
	cv::imshow("view", dst);
}

void __stdcall ImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	Chaikang64Dlg *dlg = (Chaikang64Dlg*)AfxGetApp()->GetMainWnd();
	if (pFrameInfo)
	{
		cvSetData(HK_Image, pData, HK_Image->widthStep);
		Mat hk_img = cvarrToMat(HK_Image);
		Mat ImgColor;
		cvtColor(hk_img, ImgColor, CV_GRAY2BGR, 0);
		rectangle(ImgColor, Rect((ImgColor.cols - rect_width) / 2, (ImgColor.rows - rect_height) / 2, rect_width, rect_height), Scalar(0, 255, 0), 1); //将感兴趣区域框出来
		if (preprocessing_flag){
			Mat Preprocessing_img;
			hk_img(Rect((hk_img.cols - rect_width) / 2, (hk_img.rows - rect_height) / 2, rect_width, rect_height)).copyTo(Preprocessing_img);
			Mat binary_image;
			adaptiveThreshold(Preprocessing_img, binary_image, 255, CV_ADAPTIVE_THRESH_MEAN_C,
				CV_THRESH_BINARY_INV, 25, 10);  ///局部自适应二值化函数
			//去噪  
			Mat de_noise = binary_image.clone();
			//中值滤波
			medianBlur(binary_image, de_noise, 5);
			//膨胀
			Mat dilate_img;
			Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));//膨胀系数
			dilate(de_noise, dilate_img, element);
			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarchy;
			findContours(dilate_img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//CV_RETR_EXTERNAL只检测外部轮廓，可根据自身需求进行调整  
			Mat contoursImage(dilate_img.size(), CV_8U, Scalar(255));
			//drawContours(contoursImage, contours, -1, Scalar(0), 1, 8, hierarchy);//描绘字符的外轮廓
			if (!contours.empty()){
				int cmin = 500;
				int cmax = 1500;
				std::vector<std::vector<Point>>::const_iterator itContours = contours.begin();
				for (int index = 0; index >= 0; index = hierarchy[index][0]) {
					drawContours(contoursImage, contours, index, Scalar(0), 1, 8, hierarchy);//描绘字符的外轮廓  
					if (itContours->size() > cmin && itContours->size() < cmax){
						rect = boundingRect(Mat(contours[index]));//检测外轮廓
						if (abs(rect.width - rect.height) > 10)break;
						rectangle(contoursImage, rect, Scalar(0), 3);//对外轮廓加矩形框
						rect.height += 10; rect.width += 10;
						rect.x -= 5; rect.y -= 5;
						if ((rect.x < 0) || (rect.y < 0) || (rect.x + rect.width >= Preprocessing_img.cols) || (rect.y + rect.height >= Preprocessing_img.rows))break;
						if (!dmtx_flag1){
							//DataMatrix
							Preprocessing_img(rect).copyTo(dmtx_img1);
							dmtx_flag1 = true;
						}
						if (!dmtx_flag2){
							//DataMatrix
							Preprocessing_img(rect).copyTo(dmtx_img2);
							dmtx_flag2 = true;
						}
						rect.x += (hk_img.cols - rect_width) / 2;
						rect.y += (hk_img.rows - rect_height) / 2;
						rectangle(ImgColor, rect, Scalar(0, 0, 255), 1); //将感兴趣区域框出来
						break;
					}
					itContours++;
				}
			}
			/*cv::imshow("膨胀图", dilate_img);
			cv::imshow("轮廓图", contoursImage);
			cv::waitKey(1);*/
		}
		else{
			if (!dmtx_flag1){
				//DataMatrix
				hk_img(Rect((hk_img.cols - rect_width) / 2, (hk_img.rows - rect_height) / 2, rect_width, rect_height)).copyTo(dmtx_img1);
				dmtx_flag1 = true;
			}
			if (!dmtx_flag2){
				//DataMatrix
				hk_img(Rect((hk_img.cols - rect_width) / 2, (hk_img.rows - rect_height) / 2, rect_width, rect_height)).copyTo(dmtx_img2);
				dmtx_flag2 = true;
			}
		}

		if (CTimer::timer_flag){
			CTimer::timer_flag = false;
			fps0 = fps0_count;
			fps0_count = 0;
			fps1 = fps1_count;
			fps1_count = 0;
		}
		fps1_count++;
		char fpstring[1024];  // 用于存放帧率的字符串
		sprintf_s(fpstring, "CAM_FPS: %d  REC_FPS: %d", fps1, fps0);      // 帧率保留两位小数
		cv::putText(ImgColor,			// 图像矩阵
			fpstring,                   // string型文字内容
			cv::Point(150, 30),           // 文字坐标，以左下角为原点
			cv::FONT_HERSHEY_DUPLEX,   // 字体类型
			1, // 字体大小
			cv::Scalar(0, 255, 0));     // 字体颜色

		cv::resize(ImgColor, dst, Size(pic_width, pic_height), 0, 0, 1);
		cv::imshow("view", dst);
	}
}

void Chaikang64Dlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	if (camera_flag)
	{
		camera_flag = false;
		GetDlgItem(IDOK)->SetWindowText(L"Closing...");
		ptime->CloseTimer();
		Close_HaiKang();
		GetDlgItem(IDC_SLIDER1)->EnableWindow(false);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(false);
		GetDlgItem(IDOK)->SetWindowText(L"打开相机");
		code_info.Empty();
	}
	else
	{
		GetDlgItem(IDOK)->SetWindowText(L"Opening...");
		Open_HaiKang();
		if (camera_flag)
		{
			//m_timeout->sec = 0;
			//m_timeout->usec = 0;
			ptime->CreateTimerThread(&iTime);
			AfxBeginThread(Thread1, this);
			AfxBeginThread(Thread2, this);
			GetDlgItem(IDC_SLIDER1)->EnableWindow(true);
			GetDlgItem(IDC_SLIDER2)->EnableWindow(true);
			GetDlgItem(IDOK)->SetWindowText(L"关闭相机");
		}
		else GetDlgItem(IDOK)->SetWindowText(L"打开相机");
	}
}


void Chaikang64Dlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	if (camera_flag)OnBnClickedOk();
	cvDestroyWindow("view");
	CDialogEx::OnCancel();
}


UINT Chaikang64Dlg::Thread1(void *param)
{
	Chaikang64Dlg *dlg = (Chaikang64Dlg*)param;
	while (camera_flag){
		while (dmtx_flag1)
		{
			int width = dmtx_img1.cols;
			int height = dmtx_img1.rows;
			int channels = dmtx_img1.channels();
			//WaitForSingleObject(hMutex, INFINITE);
			DmtxImage* img1 = dmtxImageCreate(dmtx_img1.data, width, height, DmtxPack8bppK);
			if (!img1) {
				fprintf(stderr, "dmtx image create fail\n");
				dmtx_flag1 = false;
			}

			DmtxDecode* dec1 = dmtxDecodeCreate(img1, 1);
			if (!dec1) {
				fprintf(stderr, "dmtx decode create fail\n");
				dmtx_flag1 = false;
			}

			DmtxRegion* reg1 = dmtxRegionFindNext(dec1, nullptr);
			if (reg1) {
				DmtxMessage* msg1 = dmtxDecodeMatrixRegion(dec1, reg1, DmtxUndefined);
				if (!msg1)dmtx_flag1 = false;
				else{
					//WaitForSingleObject(hMutex, INFINITE);
					mut.lock();
					dlg->GetDlgItem(IDC_EDIT_TYPE)->SetWindowText(L"Data Matrix");
					std::string str(reinterpret_cast<char*>(msg1->output));
					CString sstr;
					sstr.Format(L"%S", str.c_str());
					dlg->GetDlgItem(IDC_EDIT_INFO)->SetWindowText(sstr);
					if (sstr != code_info){
						num_count++;
						CString temp;
						temp.Format(_T("%d"), num_count);
						dlg->GetDlgItem(IDC_EDIT_COUNT)->SetWindowText(temp);
					}
					code_info = sstr;
					fps0_count++;
					mut.unlock();
					//ReleaseMutex(hMutex);
				}
				dmtxMessageDestroy(&msg1);
			}
			dmtxRegionDestroy(&reg1);
			dmtxDecodeDestroy(&dec1);
			dmtxImageDestroy(&img1);
			dmtx_flag1 = false;
			//ReleaseMutex(hMutex);
		}
	}
	return 0;
}

UINT Chaikang64Dlg::Thread2(void *param)
{
	Chaikang64Dlg *dlg = (Chaikang64Dlg*)param;
	while (camera_flag){
		while (dmtx_flag2)
		{
			int width = dmtx_img2.cols;
			int height = dmtx_img2.rows;
			int channels = dmtx_img2.channels();
			//WaitForSingleObject(hMutex, INFINITE);
			DmtxImage* img2 = dmtxImageCreate(dmtx_img2.data, width, height, DmtxPack8bppK);
			if (!img2) {
				fprintf(stderr, "dmtx image create fail\n");
				dmtx_flag2 = false;
			}

			DmtxDecode* dec2 = dmtxDecodeCreate(img2, 1);
			if (!dec2) {
				fprintf(stderr, "dmtx decode create fail\n");
				dmtx_flag2 = false;
			}

			DmtxRegion* reg2 = dmtxRegionFindNext(dec2, nullptr);
			if (reg2) {
				DmtxMessage* msg2 = dmtxDecodeMatrixRegion(dec2, reg2, DmtxUndefined);
				if (!msg2)dmtx_flag2 = false;
				else{
					//WaitForSingleObject(hMutex, INFINITE);
					mut.lock();
					dlg->GetDlgItem(IDC_EDIT_TYPE)->SetWindowText(L"Data Matrix");
					std::string str(reinterpret_cast<char*>(msg2->output));
					CString sstr;
					sstr.Format(L"%S", str.c_str());
					dlg->GetDlgItem(IDC_EDIT_INFO)->SetWindowText(sstr);
					if (sstr != code_info){
						num_count++;
						CString temp;
						temp.Format(_T("%d"), num_count);
						dlg->GetDlgItem(IDC_EDIT_COUNT)->SetWindowText(temp);
					}
					code_info = sstr;
					fps0_count++;
					mut.unlock();
					//ReleaseMutex(hMutex);
				}
				dmtxMessageDestroy(&msg2);
			}
			dmtxRegionDestroy(&reg2);
			dmtxDecodeDestroy(&dec2);
			dmtxImageDestroy(&img2);
			dmtx_flag2 = false;
			//ReleaseMutex(hMutex);
		}
	}
	return 0;
}


UINT Chaikang64Dlg::Open_HaiKang()
{
	HK_Image = cvCreateImageHeader(cvSize(720, 480), IPL_DEPTH_8U, 1);
	// 枚举设备
	int Ret = haikang.EnumDevices();
	if (MV_OK != Ret)
	{
		camera_flag = false;
		AfxMessageBox(L"HaiKang EnumDevices failed!");
		return HK_FAIL;
	}
	// 打开设备
	Ret = haikang.Open(0);
	if (MV_OK != Ret)
	{
		camera_flag = false;
		AfxMessageBox(L"HaiKang Open failed!");
		return HK_FAIL;
	}
	// 注册图像数据回调
	Ret = haikang.RegisterImageCallBack(ImageCallBack, NULL);
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang RegisterImageCallBack failed!");
		return HK_FAIL;
	}
	// 设置AOI
	Ret = haikang.SetAOI(1176, 720, 784, 480);
	if (MV_OK != Ret)
	{
		Ret = haikang.SetAOI(1176, 720, 784, 480);
		if (MV_OK != Ret)
		{
			AfxMessageBox(L"HaiKang SetAOI failed!");
			return HK_FAIL;
		}
		Ret = haikang.SetAOI(1176, 720, 784, 480);
	}
	// 设置帧率
	Ret = haikang.SetFrame(100);
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang SetFrame failed!");
		return HK_FAIL;
	}
	// 关闭触发
	Ret = haikang.CloseTrigger();
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang CloseTrigger failed!");
		return HK_FAIL;
	}
	// 设置自动曝光
	Ret = haikang.SetExposure(500, 500000);
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang SetExposure failed!");
		return HK_FAIL;
	}
	// 设置连续模式
	Ret = haikang.SetAcquisitionMode();
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang SetAcquisitionMode failed!");
		return HK_FAIL;
	}
	// 开启抓图
	Ret = haikang.StartGrabbing();
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang StartGrabbing failed!");
		return HK_FAIL;
	}
	camera_flag = true;
	return 0;
}

UINT Chaikang64Dlg::Close_HaiKang()
{
	// 停止抓图
	int Ret;
	if (camera_flag){
		Ret = haikang.StopGrabbing();
		if (MV_OK != Ret)
		{
			AfxMessageBox(L"HaiKang StopGrabbing failed!");
			return HK_FAIL;
		}
	}
	// 关闭设备
	Ret = haikang.Close();
	if (MV_OK != Ret)
	{
		AfxMessageBox(L"HaiKang Close failed!");
		return HK_FAIL;
	}

	Mat mat = imread("res/bluesky.bmp");
	cv::resize(mat, dst, Size(pic_width, pic_height), 0, 0, 1);
	cv::imshow("view", dst);
	return 0;
}

void Chaikang64Dlg::OnBnClickedButtonClear()
{
	// TODO:  在此添加控件通知处理程序代码
	num_count = 0;
	code_info.Empty();
	CString sstr;
	sstr.Format(_T("%d"), num_count);
	GetDlgItem(IDC_EDIT_COUNT)->SetWindowText(sstr);
}


void Chaikang64Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	rect_width = m_width.GetPos();
	rect_height = m_height.GetPos();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void Chaikang64Dlg::OnBnClickedButtonPreprocessing()
{
	// TODO:  在此添加控件通知处理程序代码
	if (preprocessing_flag){
		preprocessing_flag = false;
		GetDlgItem(IDC_BUTTON_Preprocessing)->SetWindowText(L"启用预处理");
	}
	else{
		preprocessing_flag = true;
		GetDlgItem(IDC_BUTTON_Preprocessing)->SetWindowText(L"停用预处理");
	}
}
