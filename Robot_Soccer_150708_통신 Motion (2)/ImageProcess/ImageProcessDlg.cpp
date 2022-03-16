
// ImageProcessDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "ImageProcessDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"

#include "highgui.h"
#include "cv.h"

#define THRESHOLD_SUBTRACTION 30

#define NUM_ROBOT		3
#define ROBOT_RADIOUS	10
#define CHECK_NEAR		10

//Threshold 값
#define Num_Th 50


int i_B_Ave;
int j_B_Ave;
int i_R1_Ave;
int j_R1_Ave;
int i_R2_Ave;
int j_R2_Ave; 
int i_R3_Ave;
int j_R3_Ave;

//Image Matrix
IplImage* img_Cam = 0;


//출력 창 이름
CStatic m_main;								//CAM 출력 창
CStatic m_test;								//CAM 영상처리 확인 창
CStatic m_field;							//FIELD 출력 창
CStatic m_pfield1;							//pattern1 출력 창
CStatic m_pfield2;							//pattern2 출력 창
CStatic m_pfield3;							//pattern3 출력 창
CStatic m_pfield4;							//pattern4 출력 창


//Flag Parameter
int Flag_Thread_State = 0;			//0: Off, 1: 0n
int Flag_ImageProcess_Start = 0;	//0: Off, 1: 0n
int Flag_ImageProcess_Init = 0;		//0: Off, 1: 0n
int Flag_Run = 0;					//0: Input Image, 1: Output Image
int Flag_MoveRobot = 0;				//0: Stop, 1: Go
////Flag Serial Com
int Flag_Connect_Serial = 0;		//0: DisConnect, 1: Connect
int Flag_Direct_Close = 1;			//0: No, 1: Yes

//Robot Position
int R_Position[NUM_ROBOT][2] = { { 200, 300 }, { 100, 100 }, { 150, 150 } };

//Serial Com
CString PortArray[] = { "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "COM10" };
DWORD BaurateArray[] = { CBR_4800, CBR_9600, CBR_19200, CBR_115200 };
char Rx_Buff[256] = { 0, };


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// CImageProcessDlg 대화 상자

CImageProcessDlg::CImageProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageProcessDlg::IDD, pParent)
	, m_nSettingBaudrate(0)
	, m_nSettingPort(0)
	, Cr_Th_Lb(0)
	, Cr_Th_Ub(0)
	, Cb_Th_Lb(0)
	, Cb_Th_Ub(0)
	, Gr_Th(0)
	, Size_Th(0)
	, Gr_Th2(0)
	, Cr_Th_Lb2(0)
	, Cb_Th_Ub2(0)
	, Cb_Th_Lb2(0)
	, Cr_Th_Ub2(0)
	, m_CIndex_Robot(0)
	, m_Ccommend_robot(0)
	, m_RobotPosition_X(0)
	, m_RobotPosition_Y(0)
	, m_MIndex_Robot(0)
	, m_TargetPosition_X(0)
	, m_TargetPosition_Y(0)
	, m_Index_RightFigure(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CImageProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN, m_main);
	DDX_Control(pDX, IDC_Test, m_test);
	DDX_Control(pDX, IDC_FIELD, m_field);
	DDX_Control(pDX, IDC_Template1, m_pfield1);
	DDX_Control(pDX, IDC_template2, m_pfield2);
	DDX_Control(pDX, IDC_template3, m_pfield3);
	DDX_Control(pDX, IDC_template4, m_pfield4);
	DDX_CBIndex(pDX, IDC_Baudrate, m_nSettingBaudrate);
	DDX_CBIndex(pDX, IDC_COMPORT, m_nSettingPort);
	DDX_Text(pDX, IDC_Cr_Th_Lb, Cr_Th_Lb);
	DDX_Text(pDX, IDC_Cr_Th_Ub, Cr_Th_Ub);
	DDX_Text(pDX, IDC_Cb_Th_Lb, Cb_Th_Lb);
	DDX_Text(pDX, IDC_Cb_Th_Ub, Cb_Th_Ub);
	DDX_Text(pDX, IDC_Gr_Th, Gr_Th);
	DDX_Text(pDX, IDC_Size_Th, Size_Th);
	DDX_Text(pDX, IDC_Gr_Th2, Gr_Th2);
	DDX_Text(pDX, IDC_Cr_Th_Lb2, Cr_Th_Lb2);
	DDX_Text(pDX, IDC_Cb_Th_Ub2, Cb_Th_Ub2);
	DDX_Text(pDX, IDC_Cb_Th_Lb2, Cb_Th_Lb2);
	DDX_Text(pDX, IDC_Cr_Th_Ub2, Cr_Th_Ub2);
	DDX_CBIndex(pDX, IDC_ROBOTINDEX, m_CIndex_Robot);
	DDX_Text(pDX, IDC_COMMEND, m_Ccommend_robot);
	DDX_Text(pDX, IDC_RobotPosition_X, m_RobotPosition_X);
	DDX_Text(pDX, IDC_RobotPosition_Y, m_RobotPosition_Y);
	DDX_CBIndex(pDX, IDC_ROBOTINDEX_MOVE, m_MIndex_Robot);
	DDX_Text(pDX, IDC_TargetPosition_X, m_TargetPosition_X);
	DDX_Text(pDX, IDC_TargetPosition_Y, m_TargetPosition_Y);
	DDX_CBIndex(pDX, IDC_COMBO_RightFigure, m_Index_RightFigure);
}
BEGIN_MESSAGE_MAP(CImageProcessDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CImageProcessDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageProcessDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CImageProcessDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_Go, &CImageProcessDlg::OnBnClickedGo)
	ON_BN_CLICKED(IDC_Stop, &CImageProcessDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_Connect, &CImageProcessDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_Disconnect, &CImageProcessDlg::OnBnClickedDisconnect)
	ON_EN_CHANGE(IDC_Cr_Th_Lb, &CImageProcessDlg::OnEnChangeCrThLb)
	ON_EN_CHANGE(IDC_Cr_Th_Ub, &CImageProcessDlg::OnEnChangeCrThUb)
	ON_EN_CHANGE(IDC_Cb_Th_Lb, &CImageProcessDlg::OnEnChangeCbThLb)
	ON_EN_CHANGE(IDC_Cb_Th_Ub, &CImageProcessDlg::OnEnChangeCbThUb)
	ON_EN_CHANGE(IDC_Gr_Th, &CImageProcessDlg::OnEnChangeGrTh)
	ON_EN_CHANGE(IDC_Size_Th, &CImageProcessDlg::OnEnChangeSizeTh)
	ON_EN_CHANGE(IDC_Gr_Th2, &CImageProcessDlg::OnEnChangeGrTh2)
	ON_EN_CHANGE(IDC_Cr_Th_Lb2, &CImageProcessDlg::OnEnChangeCrThLb2)
	ON_EN_CHANGE(IDC_Cr_Th_Ub2, &CImageProcessDlg::OnEnChangeCrThUb2)
	ON_EN_CHANGE(IDC_Cb_Th_Lb2, &CImageProcessDlg::OnEnChangeCbThLb2)
	ON_EN_CHANGE(IDC_Cb_Th_Ub2, &CImageProcessDlg::OnEnChangeCbThUb2)
	ON_BN_CLICKED(IDC_SendCommend, &CImageProcessDlg::OnBnClickedSendcommend)
	ON_BN_CLICKED(IDC_Run, &CImageProcessDlg::OnBnClickedRun)
	ON_CBN_SELCHANGE(IDC_COMBO_RightFigure, &CImageProcessDlg::OnCbnSelchangeComboRightfigure)
	ON_CBN_SELCHANGE(IDC_ROBOTINDEX_MOVE, &CImageProcessDlg::OnCbnSelchangeRobotindexMove)
	ON_BN_CLICKED(IDC_Robot_Move, &CImageProcessDlg::OnBnClickedRobotMove)
	ON_BN_CLICKED(IDC_Robot_Stop, &CImageProcessDlg::OnBnClickedRobotStop)
	ON_EN_CHANGE(IDC_RobotPosition_X, &CImageProcessDlg::OnEnChangeRobotpositionX)
	ON_EN_CHANGE(IDC_RobotPosition_Y, &CImageProcessDlg::OnEnChangeRobotpositionY)
	ON_EN_CHANGE(IDC_TargetPosition_X, &CImageProcessDlg::OnEnChangeTargetpositionX)
	ON_EN_CHANGE(IDC_COMMEND, &CImageProcessDlg::OnEnChangeCommend)
END_MESSAGE_MAP()


// CImageProcessDlg 메시지 처리기

BOOL CImageProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	LoadSettings();

	//////////////////////////////////
	//								//
	//			초기값 설정			//
	//								//
	//////////////////////////////////
	ball_x = 0;
	ball_y = 0;

	Cr_Th_Lb = 0;
	Cr_Th_Ub = 255;
	Cb_Th_Lb = 70;
	Cb_Th_Ub = 90;
	Cr_Th_Lb2 = 0;
	Cr_Th_Ub2 = 255;
	Cb_Th_Lb2 = 140;
	Cb_Th_Ub2 = 255;
	Gr_Th = 128;
	Gr_Th2 = 256;
	Size_Th = 2;
	
	UpdateData(false);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
void CImageProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void CImageProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
HCURSOR CImageProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void delay(clock_t n)
{
	clock_t start = clock();
	while (clock() - start < n);
}
UINT MyThread(LPVOID IpParam)
{
	CImageProcessDlg* pClass = (CImageProcessDlg*)IpParam;
	int iReturn = pClass->ThreadFunction();
	return 0L;
}
void CImageProcessDlg::CreateThread(UINT _method)
{
	if (pThread != NULL)
	{
		printf("Thread is running\n");
		return;
	}

	pThread = AfxBeginThread(MyThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (pThread == NULL)
		printf("Fail to create camera Thread\n");

	pThread->m_bAutoDelete = FALSE;
	pThread->ResumeThread();
}
bool CImageProcessDlg::DestoryThread(void)
{
	if (pThread != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(pThread->m_hThread, INFINITE);

		if (dwResult == WAIT_TIMEOUT)
			printf("Time Out\n");
		else if (dwResult == WAIT_OBJECT_0)
			printf("Thread END\n");

		pThread = NULL;
	}
	return true;
}
TTYSTRUCT CImageProcessDlg::Int2TTY()
{
	TTYSTRUCT tty;
	ZERO_MEMORY(tty);

	tty.byCommPort = (BYTE)m_nSettingPort + 1;
	tty.byXonXoff = FALSE;
	tty.byByteSize = (BYTE)_nDataValues[m_nSettingData];
	tty.byFlowCtrl = (BYTE)_nFlow[m_nSettingFlow];
	tty.byParity = (BYTE)m_nSettingParity;
	tty.byStopBits = (BYTE)_nStopBits[m_nSettingStop];
	tty.dwBaudRate = (DWORD)_nBaudRates[m_nSettingBaudrate];

	return tty;
}
TTYSTRUCT CImageProcessDlg::LoadSettings()
{
	CWinApp *pApp = AfxGetApp();

	m_nSettingPort = pApp->GetProfileIntA(CS_REGKEY_SETTINGS, CS_REGENTRY_PORT, 1);
	m_nSettingParity = 0;
	m_nSettingBaudrate = pApp->GetProfileIntA(CS_REGKEY_SETTINGS, CS_REGENTRY_BAUD, 1);
	m_nSettingData = 1;
	m_nSettingStop = 0;
	m_nSettingFlow = 0;

	return Int2TTY();
}
void CImageProcessDlg::SaveSettings()
{
	CWinApp *pApp = AfxGetApp();
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_PORT, m_nSettingPort);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_PARITY, m_nSettingParity);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_BAUD, m_nSettingBaudrate);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_DATABITS, m_nSettingData);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_STOPBITS, m_nSettingStop);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_FLOW, m_nSettingFlow);
}
int CImageProcessDlg::ThreadFunction(void)
{
	//Image Show
	CRect rect_cam, rect_test, rect_field, rect_pattern1, rect_pattern2, rect_pattern3, rect_pattern4;
	CvvImage Cvv_cam, Cvv_test, Cvv_field, Cvv_pattern;
	CDC *pDC_cam, *pDC_test, *pDC_field, *pDC_pattern1, *pDC_pattern2, *pDC_pattern3, *pDC_pattern4;
	CvSize cvsize_cam, cvsize_field, cvsize_pattern;
	
	pDC_cam = m_main.GetDC();
	m_main.GetClientRect(&rect_cam);

	//Test Window
	pDC_test = m_test.GetDC();
	m_test.GetClientRect(&rect_test);
		
	//Draw Field
	pDC_field = m_field.GetDC();
	m_field.GetClientRect(&rect_field);
	IplImage* img_Field_Original = cvLoadImage("Field.jpg", 1);
	Cvv_field.CopyOf(img_Field_Original, 1);
	Cvv_field.DrawToHDC(pDC_field->m_hDC, &rect_field);
	////Make Dumy Field
	cvsize_field.height = img_Field_Original->height;
	cvsize_field.width = img_Field_Original->width;
	IplImage* img_Field = cvCreateImage(cvsize_field, IPL_DEPTH_8U, 3);
	////
	int i, x, y;


	//Draw Pattern
	pDC_pattern1 = m_pfield1.GetDC();	
	pDC_pattern2 = m_pfield2.GetDC();
	pDC_pattern3 = m_pfield3.GetDC();
	pDC_pattern4 = m_pfield4.GetDC();

	
	m_pfield1.GetClientRect(&rect_pattern1);
	m_pfield2.GetClientRect(&rect_pattern2);
	m_pfield3.GetClientRect(&rect_pattern3);
	m_pfield4.GetClientRect(&rect_pattern4);

	img_Pattern1 = cvLoadImage("pattern1.jpg", -1);
	img_Pattern2 = cvLoadImage("pattern2.jpg", -1);
	img_Pattern3 = cvLoadImage("pattern3.jpg", 1);
	img_Pattern4 = cvLoadImage("pattern4.jpg", 1);
	
	Cvv_pattern.CopyOf(img_Pattern1, 1);
	Cvv_pattern.DrawToHDC(pDC_pattern1->m_hDC, &rect_pattern1);
	Cvv_pattern.CopyOf(img_Pattern2, 1);
	Cvv_pattern.DrawToHDC(pDC_pattern2->m_hDC, &rect_pattern2);
	Cvv_pattern.CopyOf(img_Pattern3, 1);
	Cvv_pattern.DrawToHDC(pDC_pattern3->m_hDC, &rect_pattern3);
	Cvv_pattern.CopyOf(img_Pattern4, 1);
	Cvv_pattern.DrawToHDC(pDC_pattern4->m_hDC, &rect_pattern4);

	
	//Cam Setting
	CvCapture* capture = cvCaptureFromCAM(0); //현재 인식된 웹캠을 찾고,
	cvGrabFrame(capture);
	img_Cam = cvRetrieveFrame(capture);


	while (Flag_Thread_State == 1)
	{
		cvGrabFrame(capture);
		img_Cam = cvRetrieveFrame(capture); // 현재 인식된 장면을 받아오고image에 넣는다.
		
		if (Flag_ImageProcess_Start == 1)
		{
			if (Flag_ImageProcess_Init == 0)
			{
				//Set ImageProcess Size
				cvsize_cam.height = img_Cam->height;
				cvsize_cam.width = img_Cam->width;

				Init_ImageProcess(cvsize_cam);

				Flag_ImageProcess_Init = 1;
			}

			ImageProcess(cvsize_cam);
			//Draw Cam Img
			Cvv_cam.CopyOf(img_Cam, 1);
			Cvv_cam.DrawToHDC(pDC_cam->m_hDC, &rect_cam);

			Cvv_test.CopyOf(img_Show, 1);
			Cvv_test.DrawToHDC(pDC_test->m_hDC, &rect_test);

		}
		else
		{
			Cvv_cam.CopyOf(img_Cam, 1);
			Cvv_cam.DrawToHDC(pDC_cam->m_hDC, &rect_cam);
		}

		//Draw Field Image
		////Clear
		cvCopy(img_Field_Original, img_Field);
		////Check Robot Position
		/*
		for (i = 0; i < NUM_ROBOT; i++)
		{
			x = R_Position[i][0];
			y = R_Position[i][1];

			//Draw Circle
			cvCircle(img_Field, cvPoint(x, y), ROBOT_RADIOUS, CV_RGB(255, 255, 0));
		}
		*/
		//Draw Ball
		cvCircle(img_Field, cvPoint(j_B_Ave, i_B_Ave), ROBOT_RADIOUS, CV_RGB(255, 0, 0));

		//Draw Robot		
		cvCircle(img_Field, cvPoint(j_R1_Ave, i_R1_Ave), ROBOT_RADIOUS, CV_RGB(255, 255, 0));
		cvCircle(img_Field, cvPoint(j_R2_Ave, i_R2_Ave), ROBOT_RADIOUS, CV_RGB(255, 255, 0));
		cvCircle(img_Field, cvPoint(j_R3_Ave, i_R3_Ave), ROBOT_RADIOUS, CV_RGB(255, 255, 0));
		
		//Draw
		Cvv_field.CopyOf(img_Field, 1);
		Cvv_field.DrawToHDC(pDC_field->m_hDC, &rect_field);		

	}
	cvReleaseCapture(&capture); // 할당받았던 웹캠을 해제하고,
	
	cvReleaseImage(&img_Field_Original);
	cvReleaseImage(&img_Field);
		
	m_main.ReleaseDC(pDC_cam);
	m_test.ReleaseDC(pDC_test);
	m_field.ReleaseDC(pDC_field);
	m_pfield1.ReleaseDC(pDC_pattern1);
	m_pfield2.ReleaseDC(pDC_pattern2);
	m_pfield3.ReleaseDC(pDC_pattern3);
	
	return 0;	
}

/************************************************************************************************************************************/
void CImageProcessDlg::Init_ImageProcess(CvSize img_size)
{
	//Create Image Frae
	img_Process = cvCreateImage(img_size, IPL_DEPTH_8U, 3);
	img_Gray = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Threshold = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Depth = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Labling = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_YCbCr = cvCreateImage(img_size, IPL_DEPTH_8U, 3);
	img_Cb = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Cr = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Capture = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Ball = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Edge = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_Robot = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	img_GrayBall = cvCreateImage(cvSize(img_Pattern4->width, img_Pattern4->height), IPL_DEPTH_8U, 1);
	img_Match = cvCreateImage(cvSize(img_size.width-img_Pattern4->width+1,img_size.height-img_Pattern4->height+1), IPL_DEPTH_32F, 1);

	cvCvtColor(img_Pattern4, img_GrayBall, CV_BGR2GRAY);
}

void CImageProcessDlg::ImageProcess(CvSize img_size)
{
	int i, j, k;
	int width = img_size.width;
	int height = img_size.height;
	BYTE  *data_G, *data_T, *data_D, *data_L, *data_B, *data_R, *data_E, *data_Cr, *data_Cb;
	CString str_num;
	//Copy Cam Image
	cvCopy(img_Cam, img_Process);

	//Gray Image
	cvCvtColor(img_Process, img_Gray, CV_BGR2GRAY);

	//Smoothing
	cvSmooth(img_Gray, img_Gray, CV_MEDIAN, 9, 9);

	//YCbCr
	cvCvtColor(img_Process, img_YCbCr, CV_BGR2YCrCb);
	cvCvtPixToPlane(img_YCbCr, NULL, img_Cr, img_Cb, NULL);

	//Make Image Matrix
	cvGetImageRawData(img_Gray, &data_G, NULL, NULL);
	cvGetImageRawData(img_Threshold, &data_T, NULL, NULL);
	cvGetImageRawData(img_Depth, &data_D, NULL, NULL);
	cvGetImageRawData(img_Labling, &data_L, NULL, NULL);
	cvGetImageRawData(img_Ball, &data_B, NULL, NULL);
	cvGetImageRawData(img_Robot, &data_R, NULL, NULL);
	cvGetImageRawData(img_Edge, &data_E, NULL, NULL);
	cvGetImageRawData(img_Cr, &data_Cr, NULL, NULL);
	cvGetImageRawData(img_Cb, &data_Cb, NULL, NULL);

	//Cr, Cb Threshold
	////Init value
	int i_B_Sum = 0;
	int j_B_Sum = 0;
	int n_B_Sum = 0;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if ((Cr_Th_Lb < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Th_Ub) && (Cb_Th_Lb < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Th_Ub))
			{
				data_B[i*width + j] = 255;
				data_R[i*width + j] = 0;
				data_T[i*width + j] = 0;
				data_D[i*width + j] = 0;
				data_L[i*width + j] = 0;
				i_B_Sum = i_B_Sum + i;
				j_B_Sum = j_B_Sum + j;
				n_B_Sum++;
			}
			else if ((Cr_Th_Lb2 < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Th_Ub2) && (Cb_Th_Lb2 < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Th_Ub2))
			{
				data_B[i*width + j] = 0;
				data_R[i*width + j] = 255;
				data_T[i*width + j] = 255;
				data_D[i*width + j] = 1;
				data_L[i*width + j] = 0;
			}
			else
			{
				data_B[i*width + j] = 0;
				data_R[i*width + j] = 0;
				data_T[i*width + j] = 0;
				data_D[i*width + j] = 0;
				data_L[i*width + j] = 0;
			}
		}
	}
	if (n_B_Sum != 0)
	{
		i_B_Ave = i_B_Sum / n_B_Sum;
		j_B_Ave = j_B_Sum / n_B_Sum;
	}

	//Run Recognition Algorithm
	if (Flag_Run == 1)
	{
		//Labling
		////Forward
		int S_T = 50;
		int max_TF = 0;
		int min_DF = 255;
		for (i = 1; i < height - 1; i++)
		{
			for (j = 1; j < width - 1; j++)
			{
				if (data_T[i*width + j] == 255)
				{
					if (max_TF < data_T[(i - 1)*width + (j - 1)])	max_TF = data_T[(i - 1)*width + (j - 1)];
					if (max_TF < data_T[(i - 1)*width + (j + 0)])	max_TF = data_T[(i - 1)*width + (j + 0)];
					if (max_TF < data_T[(i - 1)*width + (j + 1)])	max_TF = data_T[(i - 1)*width + (j + 1)];
					if (max_TF < data_T[(i + 0)*width + (j - 1)])	max_TF = data_T[(i + 0)*width + (j - 1)];
					if (max_TF == 0)
					{
						data_T[i*width + j] = S_T;
						S_T = S_T + 50;
					}
					else
						data_T[i*width + j] = max_TF;
				}
				if (data_D[i*width + j] != 0)
				{
					if (min_DF > 1 + data_D[(i - 1)*width + (j - 1)])	min_DF = 1 + data_D[(i - 1)*width + (j - 1)];
					if (min_DF > 1 + data_D[(i - 1)*width + (j + 0)])	min_DF = 1 + data_D[(i - 1)*width + (j + 0)];
					if (min_DF > 1 + data_D[(i - 1)*width + (j + 1)])	min_DF = 1 + data_D[(i - 1)*width + (j + 1)];
					if (min_DF > 1 + data_D[(i + 0)*width + (j - 1)])	min_DF = 1 + data_D[(i + 0)*width + (j - 1)];
					data_D[i*width + j] = min_DF;
				}
				//Init
				max_TF = 0;
				min_DF = 255;
			}
		}

		////Backward
		int max_TB = 0;
		int min_DB = 255;
		for (i = height - 1; i > 1; i--)
		{
			for (j = width - 1; j > 1; j--)
			{
				if (data_T[i*width + j] != 0)
				{
					if (max_TB < data_T[(i + 1)*width + (j + 1)])		max_TB = data_T[(i + 1)*width + (j + 1)];
					if (max_TB < data_T[(i + 1)*width + (j + 0)])		max_TB = data_T[(i + 1)*width + (j + 0)];
					if (max_TB < data_T[(i + 1)*width + (j - 1)])		max_TB = data_T[(i + 1)*width + (j - 1)];
					if (max_TB < data_T[(i + 0)*width + (j + 1)])		max_TB = data_T[(i + 0)*width + (j + 1)];
					if (max_TB < data_T[(i + 0)*width + (j + 0)])		max_TB = data_T[(i + 0)*width + (j + 0)];

					data_T[i*width + j] = max_TB;
				}
				if (data_D[i*width + j] != 0)
				{

					if (min_DB > 0 + data_D[(i + 0)*width + (j + 0)])	min_DB = 0 + data_D[(i + 0)*width + (j + 0)];
					if (min_DB > 1 + data_D[(i + 1)*width + (j + 1)])	min_DB = 1 + data_D[(i + 1)*width + (j + 1)];
					if (min_DB > 1 + data_D[(i + 1)*width + (j + 0)])	min_DB = 1 + data_D[(i + 1)*width + (j + 0)];
					if (min_DB > 1 + data_D[(i + 1)*width + (j - 1)])	min_DB = 1 + data_D[(i + 1)*width + (j - 1)];
					if (min_DB > 1 + data_D[(i + 0)*width + (j + 1)])	min_DB = 1 + data_D[(i + 0)*width + (j + 1)];
					data_D[i*width + j] = min_DB;
				}
				//Init
				max_TB = 0;
				min_DB = 255;
			}
		}

		////Sort
		int sort[Num_Th] = { 0, };
		int R = 0;
		int num = 0;
		int F = 0;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (data_D[i*width + j] > Size_Th)
				{
					for (k = 0; k < Num_Th; k++)
					{
						if (data_T[i*width + j] != sort[k])
						{
							F++;
						}
					}
					if (F == Num_Th)
					{
						sort[R] = (int)(data_T[i*width + j]);
						R++;
						num++;
					}
					//Init
					F = 0;
				}
			}
		}
		int i_R_Sum[Num_Th] = { 0, };
		int j_R_Sum[Num_Th] = { 0, };
		int n_R_Sum[Num_Th] = { 0, };

		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (data_T[i*width + j] == sort[0] && sort[0] != 0)
				{
					data_L[i*width + j] = 255; 
				}
				for (k = 0; k < Num_Th; k++)
				{
					if (data_T[i*width + j] == sort[k] && sort[k] != 0)
					{
						data_L[i*width + j] = (200 / num)*k;
						i_R_Sum[k] = i_R_Sum[k] + i;
						j_R_Sum[k] = j_R_Sum[k] + j;
						n_R_Sum[k]++;
						break;
					}
				}
			}
		}
		////Fine Center Point of Green
		if (n_R_Sum[0] != 0)
		{
			j_R1_Ave = j_R_Sum[0] / n_R_Sum[0];
			i_R1_Ave = i_R_Sum[0] / n_R_Sum[0];
		}

		if (n_R_Sum[1] != 0)
		{
			j_R2_Ave = j_R_Sum[1] / n_R_Sum[1];
			i_R2_Ave = i_R_Sum[1] / n_R_Sum[1];
		}
		if (n_R_Sum[2] != 0)
		{
			j_R3_Ave = j_R_Sum[2] / n_R_Sum[2];
			i_R3_Ave = i_R_Sum[2] / n_R_Sum[2];
		}
		////Save Robot Information


		////Show Present Robot Position
		int present_robot_x, present_robot_y;
		switch (m_MIndex_Robot)
		{
		case 0:
			present_robot_x = j_R1_Ave;
			present_robot_y = i_R1_Ave;
			str_num.Format("%d", j_R1_Ave);
			this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
			str_num.Format("%d", i_R1_Ave);
			this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
			break;
		case 1:
			present_robot_x = j_R2_Ave;
			present_robot_y = i_R2_Ave;
			str_num.Format("%d", j_R2_Ave);
			this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
			str_num.Format("%d", i_R2_Ave);
			this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
			break;
		case 2:
			present_robot_x = j_R3_Ave;
			present_robot_y = i_R3_Ave;
			str_num.Format("%d", j_R3_Ave);
			this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
			str_num.Format("%d", i_R3_Ave);
			this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
			break;
		default:
			present_robot_x = j_R1_Ave;
			present_robot_y = i_R1_Ave;
			str_num.Format("%d", j_R1_Ave);
			this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
			str_num.Format("%d", i_R1_Ave);
			this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
			break;			
		}

		//Robot Motion Planning
		if (Flag_MoveRobot == 1)
			RobotMove(present_robot_x, present_robot_y, 0, m_TargetPosition_X, m_TargetPosition_Y); //////////////////////////////////////////////////////////////////////

	}
	// Show Right Figure
	//// Process;Gray;Threshold;Depth;Labeling;Cb;Cr;Ball;Edge;Robot
	switch (m_Index_RightFigure)
	{
	case 0:
		img_Show = img_Process;
		break;
	case 1:
		img_Show = img_Gray;
		break;
	case 2:
		img_Show = img_Threshold;
		break;
	case 3:
		img_Show = img_Depth;
		break;
	case 4:
		img_Show = img_Labeling;
		break;
	case 5:
		img_Show = img_Cb;
		break;
	case 6:
		img_Show = img_Cr;
		break;
	case 7:
		img_Show = img_Ball;
		break;
	case 8:
		img_Show = img_Edge;
		break;
	case 9:
		img_Show = img_Robot;
		break;
	default:
		img_Show = img_Process;
		break;
	}	
}
bool CImageProcessDlg::SendData(int index, int commend)
{
	printf("index : %d    commend : %d \n", index+1, commend);
	if (Flag_Connect_Serial == 1)
	{
		m_Comm.WriteCommend(index, commend);
		return true;
	}
	else
	{
		printf("Serial Disconnect\n");
		return false;
	}
}
LONG CImageProcessDlg::OnReceiveData(UINT wParam, LPARAM lParam)
{
	printf("ReceiveData\n");

	int i, j, adc1, adc2;
	int nLen = wParam;
	float encoder;
	char *data = (char *)m_Comm.abIn;
	BYTE buff[256];

	memcpy(buff, data, nLen);
	printf("Data Size = %d\n", nLen);

	for (i = 0; i < nLen; i++)
	{
		printf("%d\n", (int)buff[i]);
	}

	UpdateData(FALSE);
	return 0;
}
/**********************************************************************************************************************************/
void CImageProcessDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Flag_Thread_State == 0)
	{
		printf("Thread Start\n");
		Flag_Thread_State = 1;
		CreateThread(0);
	}
	else
	{
		printf("Thread Running\n");
	}
}
void CImageProcessDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Flag_Thread_State == 1)
	{
		printf("Thread End\n");
		Flag_Thread_State = 0;
		Flag_ImageProcess_Start = 0;
		Flag_ImageProcess_Init = 0;

		//Wait Thread End
		DestoryThread();
	}
	else
	{
		printf("No Thread\n");
	}
}
void CImageProcessDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Flag_Thread_State == 1)
	{
		printf("Thread End and EXIT\n");
		Flag_MoveRobot = 0;
		delay(1000);
		Flag_Run = 0;
		delay(1000);
		Flag_ImageProcess_Start = 0;
		delay(1000);
		Flag_Thread_State = 0;

		//Wait Thread End
		DestoryThread();

		//Release Memory
		if (Flag_ImageProcess_Init == 1)
		{
			cvReleaseImage(&img_Process);
			cvReleaseImage(&img_Gray);
			cvReleaseImage(&img_Capture);
			cvReleaseImage(&img_Ball);
			cvReleaseImage(&img_Robot);
			cvReleaseImage(&img_GrayBall);
			cvReleaseImage(&img_Match);
		}
		//cvReleaseImage(&img_Cam); //already realse
		//cvReleaseImage(&img_Show);//Just address
		cvReleaseImage(&img_Pattern1);
		cvReleaseImage(&img_Pattern2);
		cvReleaseImage(&img_Pattern3);
		cvReleaseImage(&img_Pattern4);
		delete pThread;
	}
	
	//Com End
	if (Flag_Direct_Close == 1)
		OnBnClickedConnect();
	if (Flag_Connect_Serial == 1)
		OnBnClickedDisconnect();

	//Save Com Information
	SaveSettings();


	delay(1000);

	
	CDialogEx::OnOK();
}
void CImageProcessDlg::OnBnClickedGo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Flag_ImageProcess_Start = 1;
}
void CImageProcessDlg::OnBnClickedStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Flag_ImageProcess_Start = 0;
}
void CImageProcessDlg::OnBnClickedConnect()
{
	Flag_Direct_Close = 0;
	UpdateData(TRUE);
	printf("Start Connect\n");
	if (Flag_Connect_Serial == 0)
	{
		if (m_Comm.OpenCommPort(&Int2TTY()) == TRUE)
		{
			printf("Connect Success\n");
			m_Comm.SetHwnd(this->m_hWnd);
			Flag_Connect_Serial = 1;
		}
		else
		{
			printf("Connect Fail\n");
		}
	}
	else
	{
		printf("Already Connected\n");
	}
}
void CImageProcessDlg::OnBnClickedDisconnect()
{
	if (Flag_Connect_Serial == 1)
	{
		m_Comm.CloseConnection();
		printf("Disconnect Success\n");
		Flag_Connect_Serial = 0;
	}
	else
	{
		printf("Aleady Disconnected\n");
	}
}
void CImageProcessDlg::OnBnClickedRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Flag_Run == 0) Flag_Run = 1;
	else Flag_Run = 0;
}
void CImageProcessDlg::OnEnChangeCrThLb()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCrThUb()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCbThLb()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCbThUb()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeGrTh()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeSizeTh()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeGrTh2()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCrThLb2()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCrThUb2()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCbThLb2()
{
	UpdateData(true);
}
void CImageProcessDlg::OnEnChangeCbThUb2()
{
	UpdateData(true);
}
void CImageProcessDlg::OnBnClickedSendcommend()
{
	UpdateData(true);
	if (m_Ccommend_robot < 33)
	{
		SendData(m_CIndex_Robot, m_Ccommend_robot);
	}
	else
	{
		printf("Wrong commend num");
	}	
}
void CImageProcessDlg::OnCbnSelchangeComboRightfigure()
{
	UpdateData(true);
}
void CImageProcessDlg::OnCbnSelchangeRobotindexMove()
{
	UpdateData(true);
}


void CImageProcessDlg::OnBnClickedRobotMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int x_robot, y_robot;
	int x_target, y_target;

	//Get Data
	////Present Robot Position
	switch (m_MIndex_Robot)
	{
	case 0:
		x_robot = j_R1_Ave;
		y_robot = i_R1_Ave;
		break;
	case 1:
		x_robot = j_R2_Ave;
		y_robot = i_R2_Ave;
		break;
	case 2:
		x_robot = j_R3_Ave;
		y_robot = i_R3_Ave;
		break;
	default:
		x_robot = j_R1_Ave;
		y_robot = i_R1_Ave;
		break;
	}
	////Target Position
	UpdateData(TRUE);
	x_target = m_TargetPosition_X;
	y_target = m_TargetPosition_Y;
	printf("Robot ID : %d\n", m_MIndex_Robot + 1);
	printf("Present Postion X : %d  Y : %d \nTarget Position X : %d   Y : %d\n\n", x_robot, y_robot, x_target, y_target);

	//Function of Move Running
	Flag_MoveRobot = 1;
}

void CImageProcessDlg::RobotMove(int Present_x, int Present_y, int angle, int Target_x, int Target_y)
{
	//Send Information
	/**********************************************************************************************************************************/
	//SendData(m_MIndex_Robot, GO);
	
			//printf("%d\n", Present_x);
	//Target_x = ball_x;
	//Target_y = ball_y;
	printf("%d,%d\n", Target_x, Target_y);
	if (Target_y - 10 <= Present_y && Present_y <= Target_y + 10)
	{
		//SendData(m_MIndex_Robot, 32);
		if (Target_x > Present_x)
		SendData(m_MIndex_Robot, 19);
		else
		SendData(m_MIndex_Robot, 6);
	}
	else
	{
		SendData(m_MIndex_Robot, 32);
		LR_CHECK(Present_x, Present_y, Target_x, Target_y);
	}


	/**********************************************************************************************************************************/
	//Check
	//int distance = (Present_x - Target_x)*(Present_x - Target_x) + (Present_y - Target_y)*(Present_y - Target_y);
	//printf("%d\n", distance);
	//if (distance < 2*CHECK_NEAR*CHECK_NEAR)
	//{
		//Flag_MoveRobot = 0;
	//}
	if ((Target_y - 10 <= Present_y && Present_y <= Target_y + 10) && (Target_x - 10 <= Present_x && Present_x <= Target_x + 10))
	{
		Flag_MoveRobot = 0;
		SendData(m_MIndex_Robot, 23);
	}
}

void CImageProcessDlg::OnBnClickedRobotStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Flag_MoveRobot = 0;
}


void CImageProcessDlg::OnEnChangeRobotpositionX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CImageProcessDlg::OnEnChangeRobotpositionY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CImageProcessDlg::OnEnChangeTargetpositionX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CImageProcessDlg::OnEnChangeCommend()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
void CImageProcessDlg::LR_CHECK(int Present_x, int Present_y, int Target_x, int Target_y)
{
	if (Target_y < Present_y)
	{
		SendData(m_MIndex_Robot, 15); 

	}
	else if (Target_y > Present_y)
	{
		SendData(m_MIndex_Robot, 17);

	}
}
