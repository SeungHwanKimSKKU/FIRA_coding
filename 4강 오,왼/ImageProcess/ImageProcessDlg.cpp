
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

#define Orange			0
#define Blue			1
#define Yellow			2
#define Red				3
#define Pink			4
#define Green			5

#define Pattern_L		40
#define Pattern_U		60
//Threshold 값
#define Num_Th 50
#define A_size_x 3
#define A_size_y 30

int Robot_Info[3][3] = { 0, };

int kick1 = 0;
int kick2 = 0;
int kick3 = 0;

//Postion Imformation
/*
CvPoint B_Ave;		//Ball

int i_B_Ave;
int j_B_Ave;
int i_R1_Ave;
int j_R1_Ave;
int i_R2_Ave;
int j_R2_Ave;
int i_R3_Ave;
int j_R3_Ave;
*/

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


int State_Robot1 = 0;
int State_Robot2 = 0;
int State_Robot3 = 0;
int dummy_State = 0;

BYTE  *data_T, *data_D, *data_L, *data_B, *data_Cr, *data_Cb, *data_Pink, *data_Green;
//공의 중심을 찾데 필요한 변수//
int i_B_Sum = 0;
int j_B_Sum = 0;
int n_B_Sum = 0;
//레이블링에 필요한 변수//
int S_T = 50;
int max = 0;
int min = 255;
//정렬에 필요한 변수//
int sort[Num_Th] = { 0, };
int R = 0;
int F = 0;
//로봇의 중심을 찾는데 필요한 변수//
int i_R_Sum[Num_Th] = { 0, };
int j_R_Sum[Num_Th] = { 0, };
int n_R_Sum[Num_Th] = { 0, };
//로봇의 방향과 패턴을 찾는데 필요한 변수//
int Sum_Pink[3] = { 0, };
int Sum_green[3] = { 0, };
int i_G_Sum[3] = { 0, };
int j_G_Sum[3] = { 0, };
int P_BY_x1, P_BY_y1;
int P_BY_x2, P_BY_y2;
int P_BY_x3, P_BY_y3;
int P_Red_x1, P_Red_y1;
int P_Red_x2, P_Red_y2;
int P_Red_x3, P_Red_y3;
double Pattern[3] = { 0, };
double Angle[3] = { 0, };

int speed;
int position;
int Keeper_Sign = 0;
int KS[3] = {0,0,1};
int angle_R3toB;

//Robot Position
int R_Position[NUM_ROBOT][2] = { { 200, 300 }, { 100, 100 }, { 150, 150 } };

//Serial Com
CString PortArray[] = { "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "COM10" };
DWORD BaurateArray[] = { CBR_4800, CBR_9600, CBR_19200, CBR_115200 };
char Rx_Buff[256] = { 0, };

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 패널티킥 프리킥 변수
int Stop_P = 0;
int Stop_F = 0;









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
, m_CIndex_Robot(0)
, m_Ccommend_robot(0)
, m_RobotPosition_X(0)
, m_RobotPosition_Y(0)
, m_MIndex_Robot(0)
, m_TargetPosition_X(0)
, m_TargetPosition_Y(0)
, m_Index_RightFigure(0)
, Color(0)
, Select_Color(0)
, Size_Th(0)
, ROI(0)
, Cut_y_min(0)
, Cut_y_max(0)
, Cut_x_max(0)
, Cut_x_min(0)
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
	DDX_CBIndex(pDX, IDC_ROBOTINDEX, m_CIndex_Robot);
	DDX_Text(pDX, IDC_COMMEND, m_Ccommend_robot);
	DDX_Text(pDX, IDC_RobotPosition_X, m_RobotPosition_X);
	DDX_Text(pDX, IDC_RobotPosition_Y, m_RobotPosition_Y);
	DDX_CBIndex(pDX, IDC_ROBOTINDEX_MOVE, m_MIndex_Robot);
	DDX_Text(pDX, IDC_TargetPosition_X, m_TargetPosition_X);
	DDX_Text(pDX, IDC_TargetPosition_Y, m_TargetPosition_Y);
	DDX_CBIndex(pDX, IDC_COMBO_RightFigure, m_Index_RightFigure);
	DDX_CBIndex(pDX, IDC_Color, Color);
	DDX_CBIndex(pDX, IDC_Color_Th, Select_Color);
	DDX_Text(pDX, IDC_Size_Th, Size_Th);
	DDX_Text(pDX, IDC_ROI, ROI);
	DDX_Text(pDX, IDC_Cut_y_min, Cut_y_min);
	DDX_Text(pDX, IDC_Cut_y_max, Cut_y_max);
	DDX_Text(pDX, IDC_Cut_x_max, Cut_x_max);
	DDX_Text(pDX, IDC_Cut_x_min, Cut_x_min);
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
	ON_BN_CLICKED(IDC_SendCommend, &CImageProcessDlg::OnBnClickedSendcommend)
	ON_BN_CLICKED(IDC_Run, &CImageProcessDlg::OnBnClickedRun)
	ON_CBN_SELCHANGE(IDC_COMBO_RightFigure, &CImageProcessDlg::OnCbnSelchangeComboRightfigure)
	ON_CBN_SELCHANGE(IDC_ROBOTINDEX_MOVE, &CImageProcessDlg::OnCbnSelchangeRobotindexMove)
	ON_BN_CLICKED(IDC_Robot_Move, &CImageProcessDlg::OnBnClickedRobotMove)
	ON_BN_CLICKED(IDC_Robot_Stop, &CImageProcessDlg::OnBnClickedRobotStop)
	ON_CBN_SELCHANGE(IDC_Color_Th, &CImageProcessDlg::OnCbnSelchangeColorTh)
	ON_EN_CHANGE(IDC_ROI, &CImageProcessDlg::OnEnChangeRoi)
	ON_CBN_SELCHANGE(IDC_Color, &CImageProcessDlg::OnCbnSelchangeColor)

	ON_BN_CLICKED(IDC_RADIO6, &CImageProcessDlg::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &CImageProcessDlg::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO8, &CImageProcessDlg::OnBnClickedRadio8)
	ON_EN_CHANGE(IDC_Cut_y_min, &CImageProcessDlg::OnEnChangeCutymin)
	ON_EN_CHANGE(IDC_Cut_x_max, &CImageProcessDlg::OnEnChangeCutxmax)
	ON_EN_CHANGE(IDC_Cut_y_max, &CImageProcessDlg::OnEnChangeCutymax)
	ON_EN_CHANGE(IDC_Cut_x_min, &CImageProcessDlg::OnEnChangeCutxmin)
	ON_BN_CLICKED(IDC_BUTTON5, &CImageProcessDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON3, &CImageProcessDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CImageProcessDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CImageProcessDlg 메시지 처리기

double ratio, G_x, G_y, Kick_x = 0 , Kick_y = 0 ;
double L1 = { 0 }, L2 = { 0 };
int SS, ST = 0, G_Sign[3] = { 0 },QWERRTY=0;


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


	//경기장 크기 조정
	Cut_x_min = 90;
	Cut_x_max = 925;
	Cut_y_min = 55;
	Cut_y_max = 490;

	//Position
	P_Ball = cvPoint(0, 0);
	P_Robot1 = cvPoint(0, 0);
	P_Robot2 = cvPoint(0, 0);
	P_Robot3 = cvPoint(0, 0);
	P_Pattern1 = cvPoint(0, 0);
	P_Pattern2 = cvPoint(0, 0);
	P_Pattern3 = cvPoint(0, 0);

	//Threshold Value
	//주황색 공
	Cr_Orange_Th_L = 160;
	Cr_Orange_Th_U = 200;   
	Cb_Orange_Th_L = 0;
	Cb_Orange_Th_U = 90;
	//파란색 로봇
	Cr_Blue_Th_L = 0;
	Cr_Blue_Th_U = 110;
	Cb_Blue_Th_L = 140;
	Cb_Blue_Th_U = 255;
	//노란색 로봇
	Cr_Yellow_Th_L = 128;
	Cr_Yellow_Th_U = 138;
	Cb_Yellow_Th_L = 91;
	Cb_Yellow_Th_U = 110;
	//핑크색 패턴
	Cr_Pink_Th_L = 128;
	Cr_Pink_Th_U = 131;
	Cb_Pink_Th_L = 127;
	Cb_Pink_Th_U = 129;
	//초록색 패턴
	Cr_Green_Th_L = 120;
	Cr_Green_Th_U = 150;
	Cb_Green_Th_L = 149;
	Cb_Green_Th_U = 170;
	//빨간색 패턴
	Cr_Red_Th_L = 150;
	Cr_Red_Th_U = 255;
	Cb_Red_Th_L = 100;
	Cb_Red_Th_U = 120;
	//로봇 색 선택//
	Color = 0;
	//로봇 사이즈//
	Size_Th = 1;
	//패턴 사이즈//
	ROI = 15;

	//Robot Motion Time Init
	/*T_Array[M_LeftBlock] = T_LeftBlock;
	T_Array[M_CenterBlock] = T_CenterBlock;
	T_Array[M_RightBlock] = T_RightBlock;
	T_Array[M_Ceremony] = T_CereTony;
	T_Array[M_CenterRun50] = T_CenterRun50;
	T_Array[M_CenterRun30] = T_CenterRun30;
	T_Array[M_LeftTurn10] = T_LeftTrun10;
	T_Array[M_LeftTurn20] = T_LeftTrun20;
	T_Array[M_LeftTurn45] = T_LeftTurn45;
	T_Array[M_LeftTurn60] = T_LeftTurn60;
	T_Array[M_RightTurn10] = T_RightTrun10;
	T_Array[M_RightTurn20] = T_RightTrun20;
	T_Array[M_RightTurn45] = T_RightTrun45;
	T_Array[M_RightTurn60] = T_RightTrun60;
	T_Array[M_Right20] = T_Right20;
	T_Array[M_Right70] = T_Right70;
	T_Array[M_Left20] = T_Left20;
	T_Array[M_Left70] = T_Left70;
	T_Array[M_BackRun40] = T_BackRun40;
	T_Array[M_STOP] = T_STOP;
	T_Array[M_BackStep] = T_BackStep;
	T_Array[M_RightShoot] = T_RightShoot;
	T_Array[M_LeftShoot] = T_LeftShoot;
	T_Array[M_RightShooting] = T_RightShooting;
	T_Array[M_LeftShooting] = T_LeftShooting;
	T_Array[M_RightBackShooting] = T_RightBackShooting;
	T_Array[M_LeftBackShooting] = T_LeftBackShooting;
	T_Array[M_RightBlock2] = T_RightBlock2;
	T_Array[M_LeftBlock2] = T_LeftBlock2;
	T_Array[M_CenterStep] = T_CenterStep;*/


	//Show Threshold
	Cr_Th_Lb = Cr_Orange_Th_L;
	Cr_Th_Ub = Cr_Orange_Th_U;
	Cb_Th_Lb = Cb_Orange_Th_L;
	Cb_Th_Ub = Cb_Orange_Th_U;

	//Value -> MFC
	UpdateData(false);

	//Show Threshold
	Cr_Th_Lb = Cr_Orange_Th_L;
	Cr_Th_Ub = Cr_Orange_Th_U;
	Cb_Th_Lb = Cb_Orange_Th_L;
	Cb_Th_Ub = Cb_Orange_Th_U;

	//Value -> MFC
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
UINT MyThread1(LPVOID IpParam)
{
	CImageProcessDlg* pClass = (CImageProcessDlg*)IpParam;
	int iReturn = pClass->ThreadFunction();
	return 0L;
}
UINT MyThread2(LPVOID IpParam)
{
	//필요한 변수를 넘겨주고 일을 하는 함수가 여기 들어가면 됨
	CImageProcessDlg* pClass = (CImageProcessDlg*)IpParam;
	int iReturn = pClass->ThreadFunction2();
	return 0L;
}
void CImageProcessDlg::CreateThread1(UINT _method)
{
	if (pThread1 != NULL)
	{
		printf("Thread is running\n");
		return;
	}

	pThread1 = AfxBeginThread(MyThread1, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (pThread1 == NULL)
		printf("Fail to create camera Thread\n");

	pThread1->m_bAutoDelete = FALSE;
	pThread1->ResumeThread();
}

void CImageProcessDlg::CreateThread2(UINT _method)
{
	if (pThread2 != NULL)
	{
		printf("Thread is running\n");
		return;
	}

	pThread2 = AfxBeginThread(MyThread2, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (pThread2 == NULL)
		printf("Fail to create camera Thread\n");

	pThread2->m_bAutoDelete = FALSE;
	pThread2->ResumeThread();
}

bool CImageProcessDlg::DestroyThread1(void)
{
	if (pThread1 != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(pThread1->m_hThread, INFINITE);

		if (dwResult == WAIT_TIMEOUT)
			printf("Time Out\n");
		else if (dwResult == WAIT_OBJECT_0)
			printf("Thread END\n");

		pThread1 = NULL;
	}
	return true;
}

bool CImageProcessDlg::DestroyThread2(void)
{
	if (pThread1 != NULL)
	{
		DWORD dwResult = ::WaitForSingleObject(pThread1->m_hThread, INFINITE);

		if (dwResult == WAIT_TIMEOUT)
			printf("Time Out\n");
		else if (dwResult == WAIT_OBJECT_0)
			printf("Thread END\n");

		pThread1 = NULL;
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
	CvSize cvsize_cam, cvsize_field;

	pDC_cam = m_main.GetDC();
	m_main.GetClientRect(&rect_cam);

	//Test Window
	pDC_test = m_test.GetDC();
	m_test.GetClientRect(&rect_test);

	//Draw Field
	pDC_field = m_field.GetDC();
	m_field.GetClientRect(&rect_field);
	IplImage* img_Field_Original = cvLoadImage("Field.jpg", 1);
	//cvFlip(img_Field_Original, img_Field_Original, -1);
	Cvv_field.CopyOf(img_Field_Original, 1);
	Cvv_field.DrawToHDC(pDC_field->m_hDC, &rect_field);
	////Make Dumy Field
	cvsize_field.height = img_Field_Original->height;
	cvsize_field.width = img_Field_Original->width;
	IplImage* img_Field = cvCreateImage(cvsize_field, IPL_DEPTH_8U, 3);
	////


	//Draw Pattern
	pDC_pattern1 = m_pfield1.GetDC();
	pDC_pattern2 = m_pfield2.GetDC();
	pDC_pattern3 = m_pfield3.GetDC();
	pDC_pattern4 = m_pfield4.GetDC();


	m_pfield1.GetClientRect(&rect_pattern1);
	m_pfield2.GetClientRect(&rect_pattern2);
	m_pfield3.GetClientRect(&rect_pattern3);
	m_pfield4.GetClientRect(&rect_pattern4);

	img_Pattern1 = cvLoadImage("pattern1_blue.jpg", -1);
	img_Pattern2 = cvLoadImage("pattern2_blue.jpg", -1);
	img_Pattern3 = cvLoadImage("pattern3_blue.jpg", 1);
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
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 1024);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 576);
	img_Cam = cvRetrieveFrame(capture);



	while (Flag_Thread_State == 1)
	{
		cvGrabFrame(capture);
		img_Cam = cvRetrieveFrame(capture); // 현재 인식된 장면을 받아오고image에 넣는다.

		//cvFlip(img_Cam, img_Cam, -1);

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
			//cvFlip(img_Cam, img_Cam, 1);

			ImageProcess(cvsize_cam);
			//Draw Cam Img
			Cvv_cam.CopyOf(img_Cam, 1);
			Cvv_cam.DrawToHDC(pDC_cam->m_hDC, &rect_cam);

			Cvv_test.CopyOf(img_Show, 1);
			Cvv_test.DrawToHDC(pDC_test->m_hDC, &rect_test);
		}
		else
		{
			cvRectangle(img_Cam, cvPoint(Cut_x_min, Cut_y_min), cvPoint(Cut_x_max, Cut_y_max), CV_RGB(0, 0, 255));
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
		cvCircle(img_Field, cvPoint((int)G_x, (int)G_y), ROBOT_RADIOUS, CV_RGB(255, 255, 0));
		//cvCircle(img_Field, cvPoint(()Kick_x, Kick_y), ROBOT_RADIOUS, CV_RGB(100, 150, 190));
		cvCircle(img_Field, cvPoint(P_Ball_x, P_Ball_y), ROBOT_RADIOUS, CV_RGB(255, 0, 0));
		cvCircle(img_Field, cvPoint(N_Ball_x1, N_Ball_y1), ROBOT_RADIOUS, CV_RGB(255, 100, 100));
		cvCircle(img_Field, cvPoint(N_Ball_x2, N_Ball_y2), ROBOT_RADIOUS, CV_RGB(255, 100, 100));
		cvCircle(img_Field, cvPoint(N_Ball_x3, N_Ball_y3), ROBOT_RADIOUS, CV_RGB(255, 100, 100));
		cvCircle(img_Field, cvPoint(N_Ball_x4, N_Ball_y4), ROBOT_RADIOUS, CV_RGB(255, 100, 100));
		cvCircle(img_Field, cvPoint(N_Ball_x5, N_Ball_y5), ROBOT_RADIOUS, CV_RGB(255, 100, 100));


		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, 8);




		//Draw Robot		
		cvCircle(img_Field, cvPoint(Robot_Info[0][0], Robot_Info[0][1]), ROBOT_RADIOUS, CV_RGB(0, 255, 255));
	
		cvCircle(img_Field, cvPoint(Robot_Info[0][0], Robot_Info[0][1]), 18, CV_RGB(0, 128, 128));
		cvCircle(img_Field, cvPoint(Robot_Info[0][0], Robot_Info[0][1]), 6, CV_RGB(0, 64, 216));
		cvPutText(img_Field, "1", cvPoint(Robot_Info[0][0] + 10, Robot_Info[0][1] + 10), &font, cvScalar(0, 255, 0));
		cvCircle(img_Field, cvPoint(Robot_Info[1][0], Robot_Info[1][1]), ROBOT_RADIOUS, CV_RGB(0, 255, 255));
		cvCircle(img_Field, cvPoint(Robot_Info[1][0], Robot_Info[1][1]), 18, CV_RGB(0, 128, 128));
		cvCircle(img_Field, cvPoint(Robot_Info[1][0], Robot_Info[1][1]), 6, CV_RGB(0, 64, 216));
		cvPutText(img_Field, "2", cvPoint(Robot_Info[1][0] + 10, Robot_Info[1][1] + 10), &font, cvScalar(0, 255, 255));
		cvCircle(img_Field, cvPoint(Robot_Info[2][0], Robot_Info[2][1]), ROBOT_RADIOUS, CV_RGB(0, 255, 255));
		cvCircle(img_Field, cvPoint(Robot_Info[2][0], Robot_Info[2][1]), 18, CV_RGB(0, 128, 128));
		cvCircle(img_Field, cvPoint(Robot_Info[2][0], Robot_Info[2][1]), 6, CV_RGB(0, 64, 216));
		cvPutText(img_Field, "3", cvPoint(Robot_Info[2][0] + 10, Robot_Info[2][1] + 10), &font, cvScalar(0, 0, 255));

		//Draw Angle		
		cvCircle(img_Field, cvPoint(P_Red_x1, P_Red_y1), ROBOT_RADIOUS, CV_RGB(0, 0, 255));
		cvCircle(img_Field, cvPoint(P_Red_x2, P_Red_y2), ROBOT_RADIOUS, CV_RGB(0, 0, 255));
		cvCircle(img_Field, cvPoint(P_Red_x3, P_Red_y3), ROBOT_RADIOUS, CV_RGB(0, 0, 255));



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

int CImageProcessDlg::ThreadFunction2(void)
{
	clock_t present_second = 0;
	clock_t past_second = 0;
	long time_interval = 0;
	long end_second1 = 0;
	long end_second2 = 0;
	long end_second3 = 0;
	long end_second4 = 0;
	int flag_time1 = 0;   //0 : off, 1 : run
	int flag_time2 = 0;   //0 : off, 1 : run
	int flag_time3 = 0;   //0 : off, 1 : run
	int flag_time4 = 0;   //0 : off, 1 : run
	int i = 0;
	int time_interval1 = 400;
	int time_interval2 = 600;
	int time_interval3 = 800;
	int time_interval4 = 600;
	float ballx_velocity_ave = 0;
	float ballx_velocity_sum = 0;
	float bally_velocity_ave = 0;
	float bally_velocity_sum = 0;
	float time_moved = 0;
	float t = 5;
	//공의 next 위치를 찾기 위해 필요한 변수//
	int ballx_present = 0;
	int bally_present = 0;
	int ballx_past = 0;
	int bally_past = 0;
	int ball_angle = 0;
	int j = 0;
	int a = 0;

	float ballx_velocity[5] = { 0, };
	float bally_velocity[5] = { 0, };

	// int id_len[3] = { 4, 4 , 4 };
	// int array_Robot[A_size_x][A_size_y] = { { 20, 30, 20, 21 }, { 20, 30, 20, 21 }, { 20, 30, 20, 21 } };
	int array_Robot[A_size_x] = { 20, 20, 20 };

	past_second = clock();
	ballx_past = P_Ball_x;
	bally_past = P_Ball_y;


	while (Flag_Thread_State == 1)
	{
		//clock값 받기 위한 함수
		present_second = clock();
		time_interval = present_second - past_second;
		//두 시간의 차가 나면 되는 함수만 받기 위한 함수
		if (end_second1 > time_interval1)
		{
			flag_time1 = 1;
		}
		else
			end_second1 = end_second1 + time_interval;

		if (end_second2 > time_interval2)
		{
			flag_time2 = 1;
		}
		else
			end_second2 = end_second2 + time_interval;

		if (end_second3 > time_interval3)
		{
			flag_time3 = 1;
		}
		else
			end_second3 = end_second3 + time_interval;

		if (end_second4 > time_interval4)
		{
			flag_time4 = 1;
		}
		else
			end_second4 = end_second4 + time_interval;



		if (flag_time1 == 1) // time_interval1을 기준 간격으로 명령어 전달하는 부분 - 공 위치 예측
		{
			time_moved = (float)time_interval1 / 1000;

			ballx_present = P_Ball_x;
			bally_present = P_Ball_y;
			if (time_moved != 0)
			{
				if (j == 5)
				{
					j = 0;
					ballx_velocity[j] = (ballx_present - ballx_past) / time_moved;
					bally_velocity[j] = (bally_present - bally_past) / time_moved;
					j++;
				}
				else
				{
					ballx_velocity[j] = (ballx_present - ballx_past) / time_moved;
					bally_velocity[j] = (bally_present - bally_past) / time_moved;
					j++;
				}
			}

			for (i = 0; i < 5; i++)
			{
				ballx_velocity_sum = ballx_velocity[i];
				bally_velocity_sum = bally_velocity[i];
			}

			ballx_velocity_ave = ballx_velocity_sum / 5;
			bally_velocity_ave = bally_velocity_sum / 5;

			N_Ball_x1 = (int)(P_Ball_x + (ballx_velocity_ave * 1 * t));
			N_Ball_y1 = (int)(P_Ball_y + (bally_velocity_ave * 1 * t));
			N_Ball_x2 = (int)(P_Ball_x + (ballx_velocity_ave * 2 * t));
			N_Ball_y2 = (int)(P_Ball_y + (bally_velocity_ave * 2 * t));
			N_Ball_x3 = (int)(P_Ball_x + (ballx_velocity_ave * 3 * t));
			N_Ball_y3 = (int)(P_Ball_y + (bally_velocity_ave * 3 * t));
			N_Ball_x4 = (int)(P_Ball_x + (ballx_velocity_ave * 4 * t));
			N_Ball_y4 = (int)(P_Ball_y + (bally_velocity_ave * 4 * t));
			N_Ball_x5 = (int)(P_Ball_x + (ballx_velocity_ave * 5 * t));
			N_Ball_y5 = (int)(P_Ball_y + (bally_velocity_ave * 5 * t));


			ballx_past = ballx_present;
			bally_past = bally_present;


			end_second1 = 0;
			flag_time1 = 0;

			Speed_Check(P_Ball_x, P_Ball_y, N_Ball_x1, N_Ball_y1, 1);
			double K = 31.5;

			for (int i = 0; i < 5; i++)
			{
				G_x = P_Ball_x + K;
				ratio = P_Ball_x / K;
				if (P_Ball_y < 280)
				{
					G_y = P_Ball_y - ((double)(280 - P_Ball_y) / ratio);
				}
				else
				{
					G_y = P_Ball_y + ((double)(P_Ball_y - 280) / ratio);
				}
				if (-50 < G_y - P_Ball_y&&G_y - P_Ball_y < 50)
				{
					break;
				}
				else
				{
					K -= 5;
				}
				i++;
			}
			if (G_y < 90)
			{
				G_y = (P_Ball_y - G_y) + P_Ball_y;
			}
			else if (G_y > 460)
			{
				G_y = P_Ball_y - (G_y - P_Ball_y);
			}
			if (G_x > 880)
			{
				G_x = 2 * P_Ball_x - G_x;
			}
			//Kick_x = P_Ball_x - 20;
			//ratio = P_Ball_x / 20.0;
			//if (P_Ball_y < Robot_Info[0][1])
			//{
			//	Kick_y = P_Ball_y - ((double)(P_Ball_y - Robot_Info[0][1]) / ratio);
			//}
			//else
			//{
			//	Kick_y = P_Ball_y + ((double)(Robot_Info[0][1] - P_Ball_y) / ratio);
			//}
			//

			/*if (270 < P_Ball_x&&P_Ball_x < 330)
			{
				L1 =sqrt( (double)(abs(Robot_Info[0][0] - P_Ball_x)*abs(Robot_Info[0][0] - P_Ball_x)) + (double)(abs(Robot_Info[0][1] - P_Ball_y)*abs(Robot_Info[0][1] - P_Ball_y)) )*2.0/3.0;
				L2 = sqrt( (double)(abs(Robot_Info[2][0] - P_Ball_x)*abs(Robot_Info[2][0] - P_Ball_x)) + (double)(abs(Robot_Info[2][1] - P_Ball_y)*abs(Robot_Info[2][1] - P_Ball_y)));
				if (L1>L2)
				{
					SS = 0;
				}
				else
					SS = 1;
			}*/
		}
		/*if (abs(Robot_Info[0][0] - P_Ball_x) > 1)
		{
			printf("%d\n", abs(Robot_Info[0][0] - P_Ball_x));
		}*/
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			//																									//
			//		변수 정리																					//
			//							현재			1초 후		2초 후		3초 후		4초 후		5초 후 	//
			//		공의 위치 x		:	P_Ball_x	N_Ball_x1	N_Ball_x2	N_Ball_x3	N_Ball_x4	N_Ball_x5	//
			//		공의 위치 y		:	P_Ball_y	N_Ball_y1	N_Ball_y2	N_Ball_y3	N_Ball_y4	N_Ball_y5	//
			//																									//
			//							로봇 1				로봇 2				로봇 3							//
			//		로봇의 위치 x	:	Robot_Info[0][0]	Robot_Info[1][0]	Robot_Info[2][0]				//
			//		로봇의 위치 y	:	Robot_Info[0][1]	Robot_Info[1][1]	Robot_Info[2][1]							
			//		로봇의 방향 a	:	Robot_Info[0][2]	Robot_Info[1][2]	Robot_Info[2][2]										
			//																									//
			//////////////////////////////////////////////////////////////////////////////////////////////////////



			if (flag_time2 == 1) //time_interval2를 기준 간격으로 명령어 전달하는 부분 - Robot 함수 전달
			{
				
				//////////////////////////////////////////////////////////////////////////////////////////
				//																						//
				//		로봇 1번 동작 알고리즘															//
				//																						//
				//////////////////////////////////////////////////////////////////////////////////////////
				//																						//
				//		
				//Target_Move(0, P_Ball_x, P_Ball_y, 1);
				//if (QWERRTY == 0)
				//{
				//	delay(20000);
				//	QWERRTY++;
				//}
				if (P_Ball_x >= 90 && P_Ball_x <= 520 && P_Ball_y >= 55 && P_Ball_y <= 490 )
				{
						if (G_x - 15 < Robot_Info[0][0] && Robot_Info[0][0] <= G_x + 15 && G_y - 15 < Robot_Info[0][1] && Robot_Info[0][1] <= G_y + 15)
						{
							//G_Sign[0] = 1;
						//}
						//if (G_Sign[0] == 1)
						//{
							if (KS[0] == 0)
							{
								Target_Move(0,P_Ball_x,P_Ball_y,0);
								KS[0]++;
							}

							else if (KS[0] == 1)
							{
								
								Target_Move(0, P_Ball_x, P_Ball_y, 0);
									KS[0]++;
					
							}
							else if (KS[0] == 2)
							{
								SendData(0, 20);
								KS[0]++;
							}
							else if (KS[0] == 3)
							{
								SendData(0, 20);
								KS[0] ++;
							}
							else if (KS[0] == 4)
							{
								SendData(0, M_LeftShoot);
								KS[0] ++;
							}
							else if (KS[0] == 5)
							{
								SendData(0, M_RightShoot);
								KS[0] =0;
							}
						
							//RobotMove(0, P_Ball_x - 10, P_Ball_y);

							/*if (20 < abs(Robot_Info[0][0] - P_Ball_x) || abs(Robot_Info[0][1] - P_Ball_y) > 20)
							//{
							//	G_Sign[0] = 0;
							//*/

							//SendData(0, M_CenterStep);
							//if (abs(Robot_Info[0][2])>170)
							//SendData(0, M_RightShoot);
						}
						else if (P_Ball_x < 170)
						{
							RobotMove(0, P_Ball_x, P_Ball_y);
						}
						else
						{
							//if (Robot_Info[0][0] < P_Ball_x)
							//{
							//	if ()
							//	Target_Move(0,P_Ball_x,P_Ball_y+10)
							//}
							if (Robot_Info[0][0] < P_Ball_x)
							{
								//G_Sign[0] = 2;
							//}
							//if (G_Sign[0] == 2)
							//{
								if (Robot_Info[0][1]>=P_Ball_y)
								{
									RobotMove(0, P_Ball_x + 50, P_Ball_y + 50);
								}
								else
								{
									RobotMove(0, P_Ball_x + 30, P_Ball_y - 30);
								}
								if (Robot_Info[0][0]>=P_Ball_x+15)
								{
									G_Sign[0] = 0;
								}
							}
							else
							{
								RobotMove(0, (int)G_x, (int)G_y);
							}
							
						}

				}
				else
				{
					/*if (255 < Robot_Info[0][0] && Robot_Info[0][0] <= 285 && 220 < Robot_Info[0][1] && Robot_Info[0][1] <= 250)
					{
						Angle_Check(0, 180);
					}*/
					//else
					{
						Target_Move(0, 425, 280,1);
						if (30 > abs(Robot_Info[0][1] - 280) && abs(Robot_Info[0][0] - 420) < 30)
						{
							//time_interval4 = 900;
							Angle_Check(0, 180);
							
							SendData(0, M_STOP);
						}
					}
				}
				//
				//																						//
				//////////////////////////////////////////////////////////////////////////////////////////
				end_second2 = 0;
				flag_time2 = 0;

			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////
			//																									//
			//		변수 정리																					//
			//							현재			1초 후		2초 후		3초 후		4초 후		5초 후 		//
			//		공의 위치 x		:	P_Ball_x	N_Ball_x1	N_Ball_x2	N_Ball_x3	N_Ball_x4	N_Ball_x5	//
			//		공의 위치 y		:	P_Ball_y	N_Ball_y1	N_Ball_y2	N_Ball_y3	N_Ball_y4	N_Ball_y5	//
			//																									//
			//							로봇 1				로봇 2				로봇 3							//
			//		로봇의 위치 x	:	Robot_Info[0][0]	Robot_Info[1][0]	Robot_Info[2][0]				//
			//		로봇의 위치 y	:	Robot_Info[0][1]	Robot_Info[1][1]	Robot_Info[2][1]							
			//		로봇의 방향 a	:	Robot_Info[0][2]	Robot_Info[1][2]	Robot_Info[2][2]										
			//																									//
			//////////////////////////////////////////////////////////////////////////////////////////////////////

			if (flag_time3 == 1) //time_interval2를 기준 간격으로 명령어 전달하는 부분 - Robot 함수 전달
			{


				//////////////////////////////////////////////////////////////////////////////////////////
				//																						//
				//		로봇 2번 동작 알고리즘			//골키퍼											//
				//																						//
				//////////////////////////////////////////////////////////////////////////////////////////
				//																						//
				//	
				if (Keeper_Sign == 0)
				{
					if ((900 > N_Ball_x1 && N_Ball_x1 > 680) || (900 > N_Ball_x2 && N_Ball_x2 > 680) || (900 > N_Ball_x3 && N_Ball_x3 > 680))
					{
						SendData(1, 2);
						Keeper_Sign = 1;
					}
					else
					{
						if (P_Ball_y <= 210)
						{
							RobotMove(1, 820, 210);
						}
						else if (P_Ball_y >= 340)
						{
							RobotMove(1, 820, 340);
						}
						else
						{
							RobotMove(1, 820, P_Ball_y);
						}
					}
				}
				else if (Keeper_Sign == 1)
				{
					if ((740 < P_Ball_x && P_Ball_x < 880) && (100 < P_Ball_y&& P_Ball_y < 450))
					{
						if (Robot_Info[1][0] < P_Ball_x)
						{
						//	G_Sign[1] = 2;
						//}
						//if (G_Sign[1] == 2)
						//{
							if (Robot_Info[1][1] >= P_Ball_y)
							{
								RobotMove(1, P_Ball_x + 50, P_Ball_y + 50);
							}
							else
							{
								RobotMove(1, P_Ball_x + 30, P_Ball_y - 30);
							}
							if (Robot_Info[1][0] >= P_Ball_x + 15)
							{
								G_Sign[1] = 0;
							}
						}
						else
						{
							RobotMove(1, P_Ball_x, P_Ball_y);
						}
						if (abs(Robot_Info[0][0] - G_x)<15 && abs(Robot_Info[0][1] - G_y)<15)
						{
							SendData(1, 2);
						}
					}
					else
					{
						Keeper_Sign = 0;
					}
				}
				

				//if (P_Ball_x >= 450 && P_Ball_x <= 580){ Keeper_Sign = GoalKeeper(1); }// 이러한 포지션에서 골기퍼 동작 }
				//else if (P_Ball_x >= 580 && P_Ball_x <= 680){ Keeper_Sign = GoalKeeper(2); }
				//else if (P_Ball_x >= 680 && P_Ball_x <= 800 && P_Ball_y >= 110 && P_Ball_y <= 430){ Keeper_Sign = GoalKeeper(3); }
				//if (Keeper_Sign == 1)
				//{
				//	if (P_Ball_x >= 720 && (110 <= P_Ball_y && P_Ball_y <= 430))
				//	{
				//		RobotMove(1, P_Ball_x, P_Ball_y);
				//	}
				//	
				//	else
				//	{
				//		RobotMove(1, 840, 270);
				//		if (Robot_Info[1][0] > 830 && 260 <= Robot_Info[1][1] && Robot_Info[1][1] < 280)
				//		{
				//			Angle_Check(1, 180);
				//			Keeper_Sign = 0;
				//		}
				//	}
				//}
				//else
				//{
				//	
				//	RobotMove(1, 840, 270);

				//	if (Robot_Info[1][0] > 830 && 255 <= Robot_Info[1][1] && Robot_Info[1][1] < 285)
				//	{
				//		SendData(1, M_STOP);
				//		Angle_Check(1, 180);
				//		Keeper_Sign = 0;
				//	}
				//}

				//																						//
				//																						//
				//////////////////////////////////////////////////////////////////////////////////////////

				end_second3 = 0;
				flag_time3 = 0;
			}



			//////////////////////////////////////////////////////////////////////////////////////////////////////
			//																									//
			//		변수 정리																					//
			//							현재			1초 후		2초 후		3초 후		4초 후		5초 후 		//
			//		공의 위치 x		:	P_Ball_x	N_Ball_x1	N_Ball_x2	N_Ball_x3	N_Ball_x4	N_Ball_x5	//
			//		공의 위치 y		:	P_Ball_y	N_Ball_y1	N_Ball_y2	N_Ball_y3	N_Ball_y4	N_Ball_y5	//
			//																									//
			//							로봇 1				로봇 2				로봇 3							//
			//		로봇의 위치 x	:	Robot_Info[0][0]	Robot_Info[1][0]	Robot_Info[2][0]				//
			//		로봇의 위치 y	:	Robot_Info[0][1]	Robot_Info[1][1]	Robot_Info[2][1]							
			//		로봇의 방향 a	:	Robot_Info[0][2]	Robot_Info[1][2]	Robot_Info[2][2]										
			//																									//
			//////////////////////////////////////////////////////////////////////////////////////////////////////



			if (flag_time4 == 1) //time_interval2를 기준 간격으로 명령어 전달하는 부분 - Robot 함수 전달
			{
				//	//////////////////////////////////////////////////////////////////////////////////////////
				//	//																						//
				//	//		로봇 3번 동작 알고리즘															//
				//	//																						//
				//	//////////////////////////////////////////////////////////////////////////////////////////
				//	//																						//22222+++++++++++++++++++++++++++++++++++++++++++

				//	//
				if (P_Ball_x > 520 && P_Ball_x <= 780 /*&& P_Ball_y >= 40 && P_Ball_y <= 415*/)
				{
					//time_interval4 = 1000;
					if (G_x - 20 < Robot_Info[2][0] && Robot_Info[2][0] <= G_x + 20 && G_y - 20 < Robot_Info[2][1] && Robot_Info[2][1] <= G_y + 20)
					{
						G_Sign[2] = 1;
					}
					if (G_Sign[2] == 1)
					{
						if (KS[2] == 0)
						{
							Target_Move(2, P_Ball_x, P_Ball_y, 0);
							KS[2]++;
						}

						else if (KS[2] == 1)
						{

							Target_Move(2, P_Ball_x, P_Ball_y, 0);
							KS[2]++;

						}
						else if (KS[2] == 2)
						{
							SendData(2, M_CenterRun30);
							KS[2]++;
						}
						else if (KS[2] == 3)
						{
							SendData(2, M_STOP);
							KS[2] ++;
						}
						else if (KS[2] == 4)
						{
							if (P_Ball_y>Robot_Info[2][1])
							SendData(2, M_LeftShoot);
							else
								SendData(2, M_RightShoot);
							KS[2] =0;
						}
						
						//RobotMove(0, P_Ball_x - 10, P_Ball_y);
						if (10 < abs(Robot_Info[2][0] - P_Ball_x) || abs(Robot_Info[2][1] - P_Ball_y) > 10)
						{
							G_Sign[2] = 0;
						}
						/*RobotMove(0, P_Ball_x - 10, P_Ball_y);
						if (10 < abs(Robot_Info[0][0] - P_Ball_x) || abs(Robot_Info[0][1] - P_Ball_y) > 10)
						{
						G_Sign[0] = 0;
						}*/
						//SendData(0, M_CenterStep);
						//if (abs(Robot_Info[0][2])>170)
						//SendData(0, M_RightShoot);
					}
					else
					{
						//if (Robot_Info[0][0] < P_Ball_x)
						//{
						//	if ()
						//	Target_Move(0,P_Ball_x,P_Ball_y+10)
						//}
						if (Robot_Info[2][0] < P_Ball_x)
						{
							G_Sign[2] = 2;
						}
						if (G_Sign[2] == 2)
						{
							if (Robot_Info[2][1] >= P_Ball_y)
							{
								RobotMove(2, P_Ball_x + 30, P_Ball_y + 30);
							}
							else
							{
								RobotMove(2, P_Ball_x + 30, P_Ball_y - 30);
							}
							if (Robot_Info[2][0] >= P_Ball_x + 15)
							{
								G_Sign[2] = 0;
							}
						}
						else
						{
							RobotMove(2, (int)G_x, (int)G_y);
						}

					}

				}
					//else if (P_Ball_x < 310 && P_Ball_x >= 0 && P_Ball_y >= 40 && P_Ball_y <= 415)
					//{
					//	RobotMove(2, 440, );
					//	RobotMove(2, Robot_Info[2][0], P_Ball_y);
					//	/*if (Robot_Info[2][1] - P_Ball_y <= 30)
					//	{
					//		RobotMove(2, Robot_Info[2][0], P_Ball_y + 50);
					//	}
					//	else if (Robot_Info[2][1] - P_Ball_y >= -30)
					//	{
					//		RobotMove(2, Robot_Info[2][0], P_Ball_y - 50);
					//	}*/

					//}
				
				else
				{
					
					/*if (440 < Robot_Info[2][0] && Robot_Info[2][0] <= 470 && 220 < Robot_Info[2][1] && Robot_Info[2][1] <= 250)
					{
						Angle_Check(2, 180);
						time_interval4 = 900;
					}*/
		
					
						//time_interval4 = 500;
						RobotMove(2, 650, P_Ball_y); 
						if (30 > abs(Robot_Info[2][1] - P_Ball_y) && abs(Robot_Info[2][0] - 650) < 30)
						{
							//time_interval4 = 900;
							SendData(2, M_STOP);
						}
						
					
				}
				//	SendData(2, array_Robot[2]);


	

				end_second4 = 0;
				flag_time4 = 0;
			}



			past_second = present_second;
		}

		return 0;


	}


	/************************************************************************************************************************************/
	void CImageProcessDlg::Init_ImageProcess(CvSize img_size)
	{
		//Create Image Frae
		img_Process = cvCreateImage(img_size, IPL_DEPTH_8U, 3);
		img_Threshold = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Depth = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Labling = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_YCbCr = cvCreateImage(img_size, IPL_DEPTH_8U, 3);
		img_Cb = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Cr = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Capture = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Ball = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Pink = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_Green = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
		img_GrayBall = cvCreateImage(cvSize(img_Pattern4->width, img_Pattern4->height), IPL_DEPTH_8U, 1);
		img_Match = cvCreateImage(cvSize(img_size.width - img_Pattern4->width + 1, img_size.height - img_Pattern4->height + 1), IPL_DEPTH_32F, 1);

		cvCvtColor(img_Pattern4, img_GrayBall, CV_BGR2GRAY);
	}

	void CImageProcessDlg::ImageProcess(CvSize img_size)
	{
		int i, j, k;
		int width = img_size.width;
		int height = img_size.height;

		/*공의 중심을 찾데 필요한 변수*/
		i_B_Sum = 0;
		j_B_Sum = 0;
		n_B_Sum = 0;
		/*레이블링에 필요한 변수*/
		S_T = 50;
		max = 0;
		min = 255;
		/*정렬에 필요한 변수*/
		for (i = 0; i < Num_Th; i++)
		{
			sort[i] = { 0, };
		}
		R = 0;
		F = 0;
		/*로봇의 중심을 찾는데 필요한 변수*/
		for (i = 0; i < Num_Th; i++)
		{
			i_R_Sum[i] = 0;
			j_R_Sum[i] = 0;
			n_R_Sum[i] = 0;
		}
		/*로봇의 방향과 패턴을 찾는데 필요한 변수*/
		for (i = 0; i < 3; i++)
		{
			Sum_Pink[i] = 0;
			Sum_green[i] = 0;
			i_G_Sum[i] = 0;
			j_G_Sum[i] = 0;
			Pattern[i] = 0;
			Angle[i] = 0;
		}
		P_Red_x1 = 0;
		P_Red_x2 = 0;
		P_Red_x3 = 0;
		P_Red_y1 = 0;
		P_Red_y2 = 0;
		P_Red_y3 = 0;
		P_Robot1_ID = 0;
		P_Robot2_ID = 0;
		P_Robot3_ID = 0;

		//로봇의 현재 위치를 실시간으로 표현하기 위한 변수
		CString str_num;

		//Copy Cam Image
		cvCopy(img_Cam, img_Process);

		//YCbCr
		cvCvtColor(img_Process, img_YCbCr, CV_BGR2YCrCb);
		cvCvtPixToPlane(img_YCbCr, NULL, img_Cr, img_Cb, NULL);

		//cvGetImageRawData : 각 픽셀의 데이터 값 읽어오기
		cvGetImageRawData(img_Threshold, &data_T, NULL, NULL);
		cvGetImageRawData(img_Depth, &data_D, NULL, NULL);
		cvGetImageRawData(img_Labling, &data_L, NULL, NULL);
		cvGetImageRawData(img_Ball, &data_B, NULL, NULL);
		cvGetImageRawData(img_Cr, &data_Cr, NULL, NULL);
		cvGetImageRawData(img_Cb, &data_Cb, NULL, NULL);
		cvGetImageRawData(img_Pink, &data_Pink, NULL, NULL);
		cvGetImageRawData(img_Green, &data_Green, NULL, NULL);


		//Thresholding 알고리즘 : Color는 0일때 파랑색 로봇을 기준으로, 1일때 노란색 로봇을 기준으로 영상처리에 다루기 좋은 이미지 제작

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//		Threshold 설정																								//
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//		주황색 공 :																									//
		//																													//
		//		1. 주황색에 해당하는 영역을 설정하여 주황색인 부분은 255, 아닌 부분은 0으로 결정							//
		//		2. 주황색 영역의 픽셀들의 평균을 내서 주황색 공의 중심값을 결정												//
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (i = Cut_y_min; i < Cut_y_max; i++)
		{
			for (j = Cut_x_min; j < Cut_x_max; j++)
			{
				data_L[i*width + j] = 0; // 이 값은 나중에 레이블링한 결과를 보기 쉽게 한 것이므로 실전에서는 지워도 됨
				data_B[i*width + j] = 0; // 초기화 과정
				if ((Cr_Orange_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Orange_Th_U) && (Cb_Orange_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Orange_Th_U))
				{
					data_B[i*width + j] = 255;
					i_B_Sum = i_B_Sum + i;
					j_B_Sum = j_B_Sum + j;
					n_B_Sum++;
				}
			}
		}
		// i와 j의 평균을 계산해 공위 중심을 찾음
		if (n_B_Sum != 0)
		{
			P_Ball_x = j_B_Sum / n_B_Sum;
			P_Ball_y = i_B_Sum / n_B_Sum;
		}
		else
		{
			P_Ball_x = P_Ball_x;
			P_Ball_y = P_Ball_y;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//		파란색, 노란색 로봇 :																						//
		//																													//
		//		1. Color == 0 이면 파란색 이미지를 받고, Color != 0 이면 노란색 이미지를 받음								//
		//		2. 레이블링에 쓰일 이미지는 255로 저장하고, 깊이 이미지에 쓰일 이미지는 1로 저장							//
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (Color == 0) // 파란색 로봇
		{
			for (i = Cut_y_min; i < Cut_y_max; i++)
			{
				for (j = Cut_x_min; j < Cut_x_max; j++)
				{
					data_T[i*width + j] = 0; // 초기화 과정
					data_D[i*width + j] = 0;
					if ((Cr_Blue_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Blue_Th_U) && (Cb_Blue_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Blue_Th_U))
					{
						data_T[i*width + j] = 255;
						data_D[i*width + j] = 1;
					}
				}
			}
		}
		else			// 노란색 로봇
		{
			for (i = Cut_y_min; i < Cut_y_max; i++)
			{
				for (j = Cut_x_min; j < Cut_x_max; j++)
				{
					data_T[i*width + j] = 0; // 초기화 과정
					data_D[i*width + j] = 0;
					if ((Cr_Yellow_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Yellow_Th_U) && (Cb_Yellow_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Yellow_Th_U))
					{
						data_T[i*width + j] = 255;
						data_D[i*width + j] = 1;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//		초록색, 분홍색 패턴																							//
		//									눈에 보기 쉽게 만든 것이므로 색 범위를  찾고 나서 실전에서는 지워도 됨			//
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (i = Cut_y_min; i < Cut_y_max; i++)
		{
			for (j = Cut_x_min; j < Cut_x_max; j++)
			{
				data_Pink[i*width + j] = 0;
				data_Green[i*width + j] = 0;
				if ((Cr_Pink_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Pink_Th_U) && (Cb_Pink_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Pink_Th_U))
				{
					data_Pink[i*width + j] = 255;
				}
				if ((Cr_Green_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Green_Th_U) && (Cb_Green_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Green_Th_U))
				{
					data_Green[i*width + j] = 255;
				}
			}
		}

		//Run Recognition Algorithm
		if (Flag_Run == 1)
		{
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		레이블링 알고리즘 및 깊이 이미지																			//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		순방향 레이블링 및 깊이 이미지 알고리즘 :																	//
			//																													//
			//		[ 1 2 3 ]	레이블링 알고리즘은 1번, 깊이 이미지 알고리즘은 2번 설명										//
			//		[ 4 T 6 ]	1. (1),(2),(3),(4)의 값 중 최댓값으로 (T)를 결정, 모두 0이면 새로운 값으로 (T)를 결정			//
			//		[ 7 8 9 ]	2. (1),(2),(3),(4)의 값에서 1을 더한 값 중 최소값으로 (T)를 결정								//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			for (i = Cut_y_min + 1; i < Cut_y_max - 1; i++)
			{
				for (j = Cut_x_min + 1; j < Cut_x_max - 1; j++)
				{
					if (data_T[i*width + j] == 255)
					{
						// 순방향 레이블링 알고리즘
						// (1),(2),(3),(4)의 값 중 최댓값 구하기
						if (max < data_T[(i - 1)*width + (j - 1)])	max = data_T[(i - 1)*width + (j - 1)];
						if (max < data_T[(i - 1)*width + (j + 0)])	max = data_T[(i - 1)*width + (j + 0)];
						if (max < data_T[(i - 1)*width + (j + 1)])	max = data_T[(i - 1)*width + (j + 1)];
						if (max < data_T[(i + 0)*width + (j - 1)])	max = data_T[(i + 0)*width + (j - 1)];
						// (1),(2),(3),(4)모두 0이라는 것은 연결된 부분이 아니기에 새로 레이블링 값을 넣어줌
						if (max == 0)
						{
							data_T[i*width + j] = S_T;
							S_T = S_T + 10;
						}
						// (1),(2),(3),(4)가 0이 아니면 연결된 부위이므로 최댓값을 넣어줌
						else
							data_T[i*width + j] = max;
					}
					// 순방향 깊이 이미지 알고리즘
					if (data_D[i*width + j] != 0)
					{
						// (1),(2),(3),(4)에 1을 더한 값 중 최솟값 구하고 그 값을 넣어줌
						if (min > 1 + data_D[(i - 1)*width + (j - 1)])	min = 1 + data_D[(i - 1)*width + (j - 1)];
						if (min > 1 + data_D[(i - 1)*width + (j + 0)])	min = 1 + data_D[(i - 1)*width + (j + 0)];
						if (min > 1 + data_D[(i - 1)*width + (j + 1)])	min = 1 + data_D[(i - 1)*width + (j + 1)];
						if (min > 1 + data_D[(i + 0)*width + (j - 1)])	min = 1 + data_D[(i + 0)*width + (j - 1)];
						data_D[i*width + j] = min;
					}
					// 초기화 과정
					max = 0;
					min = 255;
				}
			}



			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		역방향 레이블링 및 깊이 이미지 알고리즘 :																	//
			//																													//
			//		[ 1 2 3 ]	레이블링 알고리즘은 1번, 깊이 이미지 알고리즘은 2번 설명										//
			//		[ 4 T 6 ]	1. 자기자신 (T)값과 (6),(7),(8),(9)의 값 중 최댓값으로 (T)를 결정								//
			//		[ 7 8 9 ]	2. 자기자신 (T)값과 (6),(7),(8),(9)의 값에서 1을 더한 값 중 최소값으로 (T)를 결정				//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			for (i = Cut_y_max - 1; i > Cut_y_min + 1; i--)
			{
				for (j = Cut_x_max - 1; j > Cut_x_min + 1; j--)
				{
					// 역방향 레이블링 알고리즘
					if (data_T[i*width + j] != 0)
					{
						// 자기자신 (T)값과 (6),(7),(8),(9)의 값 중 최댓값으로 (T)를 결정
						if (max < data_T[(i + 1)*width + (j + 1)])		max = data_T[(i + 1)*width + (j + 1)];
						if (max < data_T[(i + 1)*width + (j + 0)])		max = data_T[(i + 1)*width + (j + 0)];
						if (max < data_T[(i + 1)*width + (j - 1)])		max = data_T[(i + 1)*width + (j - 1)];
						if (max < data_T[(i + 0)*width + (j + 1)])		max = data_T[(i + 0)*width + (j + 1)];
						if (max < data_T[(i + 0)*width + (j + 0)])		max = data_T[(i + 0)*width + (j + 0)];
						data_T[i*width + j] = max;
					}
					// 역방향 깊이 이미지 알고리즘
					if (data_D[i*width + j] != 0)
					{
						// 자기자신 (T)값과 (6),(7),(8),(9)의 값에서 1을 더한 값 중 최소값으로 (T)를 결정	
						if (min > 0 + data_D[(i + 0)*width + (j + 0)])	min = 0 + data_D[(i + 0)*width + (j + 0)];
						if (min > 1 + data_D[(i + 1)*width + (j + 1)])	min = 1 + data_D[(i + 1)*width + (j + 1)];
						if (min > 1 + data_D[(i + 1)*width + (j + 0)])	min = 1 + data_D[(i + 1)*width + (j + 0)];
						if (min > 1 + data_D[(i + 1)*width + (j - 1)])	min = 1 + data_D[(i + 1)*width + (j - 1)];
						if (min > 1 + data_D[(i + 0)*width + (j + 1)])	min = 1 + data_D[(i + 0)*width + (j + 1)];
						data_D[i*width + j] = min;
					}
					// 초기화 과정
					max = 0;
					min = 255;
				}
			}


			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		레이블링 한 뒤죽박죽인 결과를 순서대로 나열하는 알고리즘 :													//
			//		1. 사람이 들어가는 순간 레이블링 결과가 많아질 수 있도록 적당한 레이블링 한계치 (Num_Th) 를 설정			//
			//		2. 노이즈와 구분하기 위해 적당한 로봇 사이즈 (Size_Th) 이상인 것들만 선택하도록 함							//
			//		3. sort[k]에 각각의 레이블링한 결과 값들을 넣고, 개수를 중복되서 세는 것을 막기 위해 sort[k]와 비교			//
			//		4. 로봇이 발견되는 갯수만큼 sort[k]에 차례대로 정렬															//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			for (i = Cut_y_min; i < Cut_y_max; i++)
			{
				for (j = Cut_x_min; j < Cut_x_max; j++)
				{
					if (data_D[i*width + j] > Size_Th)	// 깊이 이미지를 이용해 로봇 사이즈 이상인 값들만 선별
					{
						for (k = 0; k < Num_Th; k++)
						{
							if (data_T[i*width + j] != sort[k] && (data_T[i*width + j] != 255)) // 각 픽셀을 모든 sort[k]와 비교해서 같은지 다른지 비교
							{
								F++; // sort[k]와 다를때마다 F를 증가
							}
						}
						// F == Num_Th 일때 모든 sort[k]와 비교했을 때 다 같지 않다는 것이고, 처음으로 나온 레이블링 값이므로 그 값을 저장
						if (F == Num_Th)
						{
							sort[R] = (int)(data_T[i*width + j]);
							R++;
						}
						F = 0; // F값 초기화
					}
				}
			}
			// sort[k]에 로봇이 순서대로 저장되어 있으나 화면에 보기 쉽게 밝기를 입힘			// /* 실전에서는 이 부분부터

			for (i = Cut_y_min; i < Cut_y_max; i++)
			{
				for (j = Cut_x_min; j < Cut_x_max; j++)
				{
					for (k = 0; k < Num_Th; k++)
					{
						if (data_T[i*width + j] == sort[k] && sort[k] != 0)
						{
							data_L[i*width + j] = 50 * (k + 1);
							break;
						}
					}
				}
			}																					// */ 이 부분까지 지워도 됨

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		각 로봇의 파란색 영역의 중심을 찾는 알고리즘 :																//
			//		1. 각각의 분류된 파란색 영역 안에서 i와 j의 최솟값과 최댓값을 각각 구한다.									//
			//		2. i와 j의 최솟갑과 최댓값의 중간값을 파란색 영역의 중심값으로 설정한다.									//
			//																													//
			//		로봇 1의 파란색 중심값 : ( P_BY_x1, P_BY_y1 )																//
			//		로봇 2의 파란색 중심값 : ( P_BY_x2, P_BY_y2 )																//
			//		로봇 3의 파란색 중심값 : ( P_BY_x3, P_BY_y3 )																//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			int i_R_Min[Num_Th] = { 0, };	// 파란색 i값의 최솟값 초기화
			int i_R_Max[Num_Th] = { 0, };		// 파란색 i값의 최댓값 초기화
			int j_R_Min[Num_Th] = { 0, };	// 파란색 j값의 최솟값 초기화
			int j_R_Max[Num_Th] = { 0, };		// 파란색 j값의 최댓값 초기화

			for (i = 0; i < Num_Th; i++)
			{
				if (sort[i] != 0)
				{
					i_R_Min[i] = height;	// 파란색 i값의 최솟값 초기화
					j_R_Min[i] = width;		// 파란색 j값의 최솟값 초기화
				}
			}


			// 이미지 전체 영역에서 탐색하다가 레이블링 한 값끼리의 각 i와 j의 최댓값, 최솟값 구하기
			for (i = Cut_y_min; i < Cut_y_max; i++)
			{
				for (j = Cut_x_min; j < Cut_x_max; j++)
				{
					for (k = 0; k < Num_Th; k++)
					{
						if (data_T[i*width + j] == sort[k] && sort[k] != 0)
						{
							if (i > i_R_Max[k])
								i_R_Max[k] = i;
							if (i < i_R_Min[k])
								i_R_Min[k] = i;
							if (j > j_R_Max[k])
								j_R_Max[k] = j;
							if (j < j_R_Min[k])
								j_R_Min[k] = j;
							break;
						}
					}
				}
			}

			// 파란색의 중심은 각각의 최댓갑과 최솟값의 중간값으로 설정
			P_BY_x1 = (j_R_Max[0] + j_R_Min[0]) / 2;
			P_BY_y1 = (i_R_Max[0] + i_R_Min[0]) / 2;
			P_BY_x2 = (j_R_Max[1] + j_R_Min[1]) / 2;
			P_BY_y2 = (i_R_Max[1] + i_R_Min[1]) / 2;
			P_BY_x3 = (j_R_Max[2] + j_R_Min[2]) / 2;
			P_BY_y3 = (i_R_Max[2] + i_R_Min[2]) / 2;

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		1번 로봇 데이터 구하기 _ 중심위치 : ( P_Robot1_x, P_Robot1_y ) _ 각도 : P_Robot1_a _ 아이디 : P_Robot1_ID	//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (sort[0] != 0)
			{
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//		각도 구하는 알고리즘 :																						//
				//		1. 파란색 영역의 중심 근처의 적당한 영역(ROI)에 있는 빨간색 영역을 찾는다.									//
				//		2. 빨간색 영역의 i와 j의 최댓값과 최솟값을 각각 찾고 그 중간값을 빨간색 영역의 중심으로 설정한다.			//
				//		3. 파란색 영역의 중심과 빨간색 영역의 중심사이의 거리 차를 arctan를 이용해 각도를 구한다.					//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				int i_Red1_Min = height;	// 빨간색 i값의 최솟값 초기화
				int i_Red1_Max = 0;			// 빨간색 i값의 최댓값 초기화
				int j_Red1_Min = width;		// 빨간색 j값의 최솟값 초기화
				int j_Red1_Max = 0;			// 빨간색 j값의 최댓값 초기화

				for (i = P_BY_y1 - ROI; i < P_BY_y1 + ROI; i++)			// 파란색 영역의 중심에서 ROI만큼 떨어진 i값의 범위안에서 탐색
				{
					for (j = P_BY_x1 - ROI; j < P_BY_x1 + ROI; j++)		// 파란색 영역의 중심에서 ROI만큼 떨어진 j값의 범위안에서 탐색
					{
						if (0 < i && i < height && 0 < j &&j < width)	// i나 j가 (-)값이나 이미지 크기를 벗아난 값을 가질 때 나타나는 에러를 피하기 위해 제외
						{
							// Cr이미지와 Cb이미지에서 빨간색이 나타내는 영역 안에 존재하는 픽셀 찾기
							if ((Cr_Red_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Red_Th_U) && (Cb_Red_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Red_Th_U))
							{
								// 빨간색의 중심값을 찾기 위해 각각의 최댓값과 최소값 찾기
								if (i > i_Red1_Max)
									i_Red1_Max = i;
								if (i < i_Red1_Min)
									i_Red1_Min = i;
								if (j > j_Red1_Max)
									j_Red1_Max = j;
								if (j < j_Red1_Min)
									j_Red1_Min = j;
							}
						}
					}
				}
				// 빨간색의 중심은 각각의 최댓갑과 최솟값의 중간값으로 설정
				P_Red_x1 = (j_Red1_Max + j_Red1_Min) / 2;
				P_Red_y1 = (i_Red1_Max + i_Red1_Min) / 2;
				// 파란색에서 빨간색으로 향하는 벡터의 각도 값은 arctan를 이용
				P_Robot1_a = cvFastArctan(-(P_Red_y1 - P_BY_y1), (P_Red_x1 - P_BY_x1));
				// 각도 값 일반화 및 양자화
				P_Robot1_a = P_Robot1_a % 360;
				P_Robot1_a = 15 * (int)(((int)(P_Robot1_a / 7.5) + 1) / 2);
				if (P_Robot1_a > 180)	P_Robot1_a = P_Robot1_a - 360;
				// 로봇 전체의 중심값은 빨간색의 중심값과 파란색의 중심값의 중간값
				P_Robot1_x = (P_Red_x1 + P_BY_x1) / 2;
				P_Robot1_y = (P_Red_y1 + P_BY_y1) / 2;





				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//		패턴 구하는 알고리즘 :																						//
				//		1. 로봇의 중심 근처의 적당한 영역(ROI*2/3)에 있는 빨간색 영역을 찾는다.										//
				//		2. 분홍색과 초록색의 개수를 센다.																			//
				//		3. 분홍색과 초록색사이의 개수 비를 이용하여 패턴을 구분한다.												//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				for (i = P_Robot1_y - 20; i < P_Robot1_y + 20; i++)		// 로봇의 중심에서 (ROI*2/3)만큼 떨어진 i값의 범위안에서 탐색
				{
					for (j = P_Robot1_x - 20; j < P_Robot1_x + 20; j++) // 로봇의 중심에서 (ROI*2/3)만큼 떨어진 j값의 범위안에서 탐색
					{
						if (0 < i && i < height && 0 < j &&j < width) // i나 j가 (-)값이나 이미지 크기를 벗아난 값을 가질 때 나타나는 에러를 피하기 위해 제외
						{
							//if ((Cr_Pink_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Pink_Th_U) && (Cb_Pink_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Pink_Th_U))
							if (data_Pink[i*width + j] == 255)
							{
								//핑크색의 픽셀 수 세기
								Sum_Pink[0]++;
								//printf("no\n");
							}
							if ((Cr_Green_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Green_Th_U) && (Cb_Green_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Green_Th_U))
							{
								//초록색의 픽셀 수 세기 
								Sum_green[0]++;
							}
						}
					}
				}
				// 패턴을 이용해 ID정보 구하기
				//if (Sum_Pink[0] < Pattern_L && Sum_green[0] > Pattern_U)
				if (Sum_green[0] > Pattern_U)
				{
					P_Robot1_ID = 1;
				}
				//else if (Sum_Pink[0] > Pattern_L && Sum_Pink[0] < Pattern_U && Sum_green[0]>Pattern_L && Sum_green[0] < Pattern_U)
				else if (Sum_green[0] > Pattern_L && Sum_green[0] < Pattern_U)
				{
					P_Robot1_ID = 2;
				}
				//else if (Sum_Pink[0] > Pattern_U && Sum_green[0] < Pattern_L)
				else if (Sum_green[0] < Pattern_L)
				{
					P_Robot1_ID = 3;
				}
				else
				{
					printf("Fail to fine Pattern\n");
				}

				// 로봇 1에 대한 정보를 화면에 출력

			}
			// 카메라 비율 조정
			P_Robot1_x = (512 + 8 * P_Robot1_x) / 9;
			P_Robot1_y = (273 + 8 * P_Robot1_y) / 9;
			P_Red_x1 = (512 + 8 * P_Red_x1) / 9;
			P_Red_y1 = (273 + 8 * P_Red_y1) / 9;
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		2번 로봇 데이터 구하기 _ 중심위치 : ( P_Robot2_x, P_Robot2_y ) _ 각도 : P_Robot2_a _ 아이디 : P_Robot2_ID	//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (sort[1] != 0)
			{
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//		각도 구하는 알고리즘																						//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				int i_Red2_Min = height;
				int i_Red2_Max = 0;
				int j_Red2_Min = width;
				int j_Red2_Max = 0;
				for (i = P_BY_y2 - ROI; i < P_BY_y2 + ROI; i++)
				{
					for (j = P_BY_x2 - ROI; j < P_BY_x2 + ROI; j++)
					{
						if (0 < i && i < height && 0 < j &&j < width)
						{
							if ((Cr_Red_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Red_Th_U) && (Cb_Red_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Red_Th_U))
							{
								if (i > i_Red2_Max)
									i_Red2_Max = i;
								if (i < i_Red2_Min)
									i_Red2_Min = i;
								if (j > j_Red2_Max)
									j_Red2_Max = j;
								if (j < j_Red2_Min)
									j_Red2_Min = j;
							}
						}
					}
				}
				P_Red_x2 = (j_Red2_Max + j_Red2_Min) / 2;
				P_Red_y2 = (i_Red2_Max + i_Red2_Min) / 2;
				P_Robot2_a = cvFastArctan(-(P_Red_y2 - P_BY_y2), (P_Red_x2 - P_BY_x2));
				P_Robot2_a = P_Robot2_a % 360;
				P_Robot2_a = 15 * (int)(((int)(P_Robot2_a / 7.5) + 1) / 2);
				if (P_Robot2_a > 180)	P_Robot2_a = P_Robot2_a - 360;
				P_Robot2_x = (P_Red_x2 + P_BY_x2) / 2;
				P_Robot2_y = (P_Red_y2 + P_BY_y2) / 2;

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//		패턴 구하는 알고리즘 																						//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				for (i = P_Robot2_y - (int)(ROI * 2 / 3); i < P_Robot2_y + (int)(ROI * 2 / 3); i++)
				{
					for (j = P_Robot2_x - (int)(ROI * 2 / 3); j < P_Robot2_x + (int)(ROI * 2 / 3); j++)
					{
						if (0 < i && i < height && 0 < j &&j < width)
						{
							if ((Cr_Pink_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Pink_Th_U) && (Cb_Pink_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Pink_Th_U))
							{
								Sum_Pink[1]++;
							}
							if ((Cr_Green_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Green_Th_U) && (Cb_Green_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Green_Th_U))
							{
								Sum_green[1]++;
							}
						}
					}
				}

				//if (Sum_Pink[1] < Pattern_L && Sum_green[1] > Pattern_U)
				if (Sum_green[1] > Pattern_U)
				{
					P_Robot2_ID = 1;
				}
				//else if (Sum_Pink[1] > Pattern_L && Sum_Pink[1] < Pattern_U && Sum_green[1]>Pattern_L && Sum_green[1] < Pattern_U)
				else if (Sum_green[1] > Pattern_L && Sum_green[1] < Pattern_U)
				{
					P_Robot2_ID = 2;
				}
				//else if (Sum_Pink[1] > Pattern_U && Sum_green[1] < Pattern_L)
				else if (Sum_green[1] < Pattern_L)
				{
					P_Robot2_ID = 3;
				}
				else
				{
					printf("Fail to fine Pattern\n");
				}
			}

			P_Robot2_x = (512 + 8 * P_Robot2_x) / 9;
			P_Robot2_y = (273 + 8 * P_Robot2_y) / 9;
			P_Red_x2 = (512 + 8 * P_Red_x2) / 9;
			P_Red_y2 = (273 + 8 * P_Red_y2) / 9;

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		3번 로봇 데이터 구하기 _ 중심위치 : ( P_Robot3_x, P_Robot3_y ) _ 각도 : P_Robot3_a _ 아이디 : P_Robot3_ID	//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (sort[2] != 0)
			{
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//		각도 구하는 알고리즘																						//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				int i_Red3_Min = height;
				int i_Red3_Max = 0;
				int j_Red3_Min = width;
				int j_Red3_Max = 0;

				for (i = P_BY_y3 - ROI; i < P_BY_y3 + ROI; i++)
				{
					for (j = P_BY_x3 - ROI; j < P_BY_x3 + ROI; j++)
					{
						if (0 < i && i < height && 0 < j &&j < width)
						{
							if ((Cr_Red_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Red_Th_U) && (Cb_Red_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Red_Th_U))
							{
								if (i > i_Red3_Max)
									i_Red3_Max = i;
								if (i < i_Red3_Min)
									i_Red3_Min = i;
								if (j > j_Red3_Max)
									j_Red3_Max = j;
								if (j < j_Red3_Min)
									j_Red3_Min = j;
							}
						}
					}
				}
				P_Red_x3 = (j_Red3_Max + j_Red3_Min) / 2;
				P_Red_y3 = (i_Red3_Max + i_Red3_Min) / 2;
				P_Robot3_a = cvFastArctan(-(P_Red_y3 - P_BY_y3), (P_Red_x3 - P_BY_x3));
				P_Robot3_a = P_Robot3_a % 360;
				P_Robot3_a = 15 * (int)(((int)(P_Robot3_a / 7.5) + 1) / 2);
				if (P_Robot3_a > 180)	P_Robot3_a = P_Robot3_a - 360;
				P_Robot3_x = (P_Red_x3 + P_BY_x3) / 2;
				P_Robot3_y = (P_Red_y3 + P_BY_y3) / 2;

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//		패턴 구하는 알고리즘 																						//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				for (i = P_Robot3_y - (int)(ROI * 2 / 3); i < P_Robot3_y + (int)(ROI * 2 / 3); i++)
				{
					for (j = P_Robot3_x - (int)(ROI * 2 / 3); j < P_Robot3_x + (int)(ROI * 2 / 3); j++)
					{
						if (0 < i && i < height && 0 < j &&j < width)
						{
							if ((Cr_Pink_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Pink_Th_U) && (Cb_Pink_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Pink_Th_U))
							{
								Sum_Pink[2]++;
							}
							if ((Cr_Green_Th_L < data_Cr[i*width + j] && data_Cr[i*width + j] < Cr_Green_Th_U) && (Cb_Green_Th_L < data_Cb[i*width + j] && data_Cb[i*width + j] < Cb_Green_Th_U))
							{
								Sum_green[2]++;
							}
						}
					}
				}

				//if (Sum_Pink[2] < Pattern_L && Sum_green[2] > Pattern_U)
				if (Sum_green[2] > Pattern_U)
				{
					P_Robot3_ID = 1;
				}
				//else if (Sum_Pink[2] > Pattern_L && Sum_Pink[2] < Pattern_U && Sum_green[2]>Pattern_L && Sum_green[2] < Pattern_U)
				else if (Sum_green[2] > Pattern_L && Sum_green[2] < Pattern_U)
				{
					P_Robot3_ID = 2;
				}
				//else if (Sum_Pink[2] > Pattern_U && Sum_green[2] < Pattern_L)
				else if (Sum_green[2] < Pattern_L)
				{
					P_Robot3_ID = 3;
				}
				else
				{
					printf("Fail to fine Pattern\n");
				}

			}

			P_Robot3_x = (512 + 8 * P_Robot3_x) / 9;
			P_Robot3_y = (273 + 8 * P_Robot3_y) / 9;
			P_Red_x3 = (512 + 8 * P_Red_x3) / 9;
			P_Red_y3 = (273 + 8 * P_Red_y3) / 9;



			printf("////////////////////////////////////////////////////////\n");
			printf("Robot1 x:%d, y:%d\n", P_Robot1_x, P_Robot1_y);
			printf("Pattern angle:%d, ID:%d\n", P_Robot1_a, P_Robot1_ID);
			printf("Robot2 x:%d, y:%d\n", P_Robot2_x, P_Robot2_y);
			printf("Pattern angle:%d, ID:%d\n", P_Robot2_a, P_Robot2_ID);
			printf("Robot3 x:%d, y:%d\n", P_Robot3_x, P_Robot3_y);
			printf("Pattern angle:%d, ID:%d\n", P_Robot3_a, P_Robot3_ID);
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//		패턴으로 결정한 ID를 실제 로봇의 ID와 매칭																	//
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			/*
			Robot_Info[0][0] = 0;
			Robot_Info[0][1] = 0;
			Robot_Info[0][2] = 0;

			Robot_Info[1][0] = 0;
			Robot_Info[1][1] = 0;
			Robot_Info[1][2] = 0;

			Robot_Info[2][0] = 0;
			Robot_Info[2][1] = 0;
			Robot_Info[2][2] = 0;
			*/
			Robot_Info[P_Robot1_ID - 1][0] = P_Robot1_x;
			Robot_Info[P_Robot1_ID - 1][1] = P_Robot1_y;
			Robot_Info[P_Robot1_ID - 1][2] = P_Robot1_a;

			Robot_Info[P_Robot2_ID - 1][0] = P_Robot2_x;
			Robot_Info[P_Robot2_ID - 1][1] = P_Robot2_y;
			Robot_Info[P_Robot2_ID - 1][2] = P_Robot2_a;

			Robot_Info[P_Robot3_ID - 1][0] = P_Robot3_x;
			Robot_Info[P_Robot3_ID - 1][1] = P_Robot3_y;
			Robot_Info[P_Robot3_ID - 1][2] = P_Robot3_a;

			////Show Present Robot Position
			int present_robot_x, present_robot_y, present_robot_a;
			switch (m_MIndex_Robot)
			{
			case 0:
				present_robot_x = P_Robot1_x;
				present_robot_y = P_Robot1_y;
				present_robot_a = P_Robot1_a;
				str_num.Format("%d", P_Robot1_x);
				this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
				str_num.Format("%d", P_Robot1_y);
				this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
				break;
			case 1:
				present_robot_x = P_Robot2_x;
				present_robot_y = P_Robot2_y;
				present_robot_a = P_Robot2_a;
				str_num.Format("%d", P_Robot2_x);
				this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
				str_num.Format("%d", P_Robot2_y);
				this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
				break;
			case 2:
				present_robot_x = P_Robot3_x;
				present_robot_y = P_Robot3_y;
				present_robot_a = P_Robot3_a;
				str_num.Format("%d", P_Robot3_x);
				this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
				str_num.Format("%d", P_Robot3_y);
				this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
				break;
			default:
				present_robot_x = P_Robot1_x;
				present_robot_y = P_Robot1_y;
				present_robot_a = P_Robot1_a;
				str_num.Format("%d", P_Robot1_x);
				this->GetDlgItem(IDC_RobotPosition_X)->SetWindowTextA(str_num);
				str_num.Format("%d", P_Robot1_y);
				this->GetDlgItem(IDC_RobotPosition_Y)->SetWindowTextA(str_num);
				break;
			}



		}
		// Show Right Figure
		//// Process;Gray;Threshold;Depth;Labeling;Cb;Cr;Ball;Edge;Robot
		switch (m_Index_RightFigure)
		{
		case 0://원본
			img_Show = img_Process;
			break;
		case 1://공
			img_Show = img_Ball;
			break;
		case 2://로봇
			img_Show = img_Labling;
			break;
		case 3://Cb
			img_Show = img_Cb;
			break;
		case 4://Cr
			img_Show = img_Cr;
			break;
		case 5://Cr
			img_Show = img_Pink;
			break;
		case 6://Cr
			img_Show = img_Green;
			break;
		default:
			img_Show = img_Process;
			break;
		}
	}
	bool CImageProcessDlg::SendData(int index, int commend)
	{
		//	printf("index : %d    commend : %d \n", index+1, commend);
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
		//	printf("ReceiveData\n");

		int i;
		int nLen = wParam;
		char *data = (char *)m_Comm.abIn;
		BYTE buff[256];

		memcpy(buff, data, nLen);
		//	printf("Data Size = %d\n", nLen);

		for (i = 0; i < nLen; i++)
		{
			//		printf("%d\n", (int)buff[i]);
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
			CreateThread1(0);
			CreateThread2(0);
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
			DestroyThread1();
			DestroyThread2();
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
			DestroyThread1();
			DestroyThread2();

			//Release Memory
			if (Flag_ImageProcess_Init == 1)
			{
				cvReleaseImage(&img_Process);
				cvReleaseImage(&img_Capture);
				cvReleaseImage(&img_Ball);
				cvReleaseImage(&img_GrayBall);
				cvReleaseImage(&img_Match);
			}
			//cvReleaseImage(&img_Cam); //already realse
			//cvReleaseImage(&img_Show);//Just address
			cvReleaseImage(&img_Pattern1);
			cvReleaseImage(&img_Pattern2);
			cvReleaseImage(&img_Pattern3);
			cvReleaseImage(&img_Pattern4);
			delete pThread1;
			delete pThread2;
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
	void CImageProcessDlg::OnCbnSelchangeColorTh()
	{
		UpdateData(TRUE);
		switch (Select_Color)
		{
		case Orange:
			Cr_Th_Lb = Cr_Orange_Th_L;
			Cr_Th_Ub = Cr_Orange_Th_U;
			Cb_Th_Lb = Cb_Orange_Th_L;
			Cb_Th_Ub = Cb_Orange_Th_U;
			break;
		case Blue:
			Cr_Th_Lb = Cr_Blue_Th_L;
			Cr_Th_Ub = Cr_Blue_Th_U;
			Cb_Th_Lb = Cb_Blue_Th_L;
			Cb_Th_Ub = Cb_Blue_Th_U;
			break;
		case Yellow:
			Cr_Th_Lb = Cr_Yellow_Th_L;
			Cr_Th_Ub = Cr_Yellow_Th_U;
			Cb_Th_Lb = Cb_Yellow_Th_L;
			Cb_Th_Ub = Cb_Yellow_Th_U;
			break;
		case Pink:
			Cr_Th_Lb = Cr_Pink_Th_L;
			Cr_Th_Ub = Cr_Pink_Th_U;
			Cb_Th_Lb = Cb_Pink_Th_L;
			Cb_Th_Ub = Cb_Pink_Th_U;
			break;
		case Green:
			Cr_Th_Lb = Cr_Green_Th_L;
			Cr_Th_Ub = Cr_Green_Th_U;
			Cb_Th_Lb = Cb_Green_Th_L;
			Cb_Th_Ub = Cb_Green_Th_U;
			break;
		case Red:
			Cr_Th_Lb = Cr_Red_Th_L;
			Cr_Th_Ub = Cr_Red_Th_U;
			Cb_Th_Lb = Cb_Red_Th_L;
			Cb_Th_Ub = Cb_Red_Th_U;
			break;
		default:
			Cr_Th_Lb = Cr_Orange_Th_L;
			Cr_Th_Ub = Cr_Orange_Th_U;
			Cb_Th_Lb = Cb_Orange_Th_L;
			Cb_Th_Ub = Cb_Orange_Th_U;
			break;
		}
		UpdateData(FALSE);
	}
	void CImageProcessDlg::OnEnChangeCrThLb()
	{
		UpdateData(true);
		switch (Select_Color)
		{
		case Orange:
			Cr_Orange_Th_L = Cr_Th_Lb;
			break;
		case Blue:
			Cr_Blue_Th_L = Cr_Th_Lb;
			break;
		case Pink:
			Cr_Pink_Th_L = Cr_Th_Lb;
			break;
		case Yellow:
			Cr_Yellow_Th_L = Cr_Th_Lb;
			break;
		case Green:
			Cr_Green_Th_L = Cr_Th_Lb;
			break;
		case Red:
			Cr_Red_Th_L = Cr_Th_Lb;
			break;
		default:
			Cr_Orange_Th_L = Cr_Th_Lb;
			break;
		}
	}
	void CImageProcessDlg::OnEnChangeCrThUb()
	{
		UpdateData(true);
		switch (Select_Color)
		{
		case Orange:
			Cr_Orange_Th_U = Cr_Th_Ub;
			break;
		case Blue:
			Cr_Blue_Th_U = Cr_Th_Ub;
			break;
		case Pink:
			Cr_Pink_Th_U = Cr_Th_Ub;
			break;
		case Yellow:
			Cr_Yellow_Th_U = Cr_Th_Ub;
			break;
		case Green:
			Cr_Green_Th_U = Cr_Th_Ub;
			break;
		case Red:
			Cr_Red_Th_U = Cr_Th_Ub;
			break;
		default:
			Cr_Orange_Th_U = Cr_Th_Ub;
			break;
		}
	}
	void CImageProcessDlg::OnEnChangeCbThLb()
	{
		UpdateData(true);
		switch (Select_Color)
		{
		case Orange:
			Cb_Orange_Th_L = Cb_Th_Lb;
			break;
		case Blue:
			Cb_Blue_Th_L = Cb_Th_Lb;
			break;
		case Pink:
			Cb_Pink_Th_L = Cb_Th_Lb;
			break;
		case Yellow:
			Cb_Yellow_Th_L = Cb_Th_Lb;
			break;
		case Green:
			Cb_Green_Th_L = Cb_Th_Lb;
			break;
		case Red:
			Cb_Red_Th_L = Cb_Th_Lb;
			break;
		default:
			Cb_Orange_Th_L = Cb_Th_Lb;
			break;
		}
	}
	void CImageProcessDlg::OnEnChangeCbThUb()
	{
		UpdateData(true);
		switch (Select_Color)
		{
		case Orange:
			Cb_Orange_Th_U = Cb_Th_Ub;
			break;
		case Blue:
			Cb_Blue_Th_U = Cb_Th_Ub;
			break;
		case Pink:
			Cb_Pink_Th_U = Cb_Th_Ub;
			break;
		case Yellow:
			Cb_Yellow_Th_U = Cb_Th_Ub;
			break;
		case Green:
			Cb_Green_Th_U = Cb_Th_Ub;
			break;
		case Red:
			Cb_Red_Th_U = Cb_Th_Ub;
			break;
		default:
			Cb_Orange_Th_U = Cb_Th_Ub;
			break;
		}
	}
	void CImageProcessDlg::OnEnChangeSizeTh()
	{
		UpdateData(true);
	}
	void CImageProcessDlg::OnEnChangeRoi()
	{
		UpdateData(true);
	}
	void CImageProcessDlg::OnCbnSelchangeColor()
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
			x_robot = P_Robot1_x;
			y_robot = P_Robot1_y;
			break;
		case 1:
			x_robot = P_Robot2_x;
			y_robot = P_Robot2_y;
			break;
		case 2:
			x_robot = P_Robot3_x;
			y_robot = P_Robot3_y;
			break;
		default:
			x_robot = P_Robot1_x;
			y_robot = P_Robot1_y;
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

	void CImageProcessDlg::RobotMove(int num, int Target_x, int Target_y)
	{
		//Send Information
		/**********************************************************************************************************************************/
		//SendData(m_MIndex_Robot, GO);
		int *R_Data[3][3] = { { &Robot_Info[0][0], &Robot_Info[0][1], &Robot_Info[0][2] }, { &Robot_Info[1][0], &Robot_Info[1][1], &Robot_Info[1][2] }, { &Robot_Info[2][0], &Robot_Info[2][1], &Robot_Info[2][2] } };


		//printf("%d    %d\n", *Robot_num[0][0], *Robot_num[0][1]);
		//printf("%d    %d\n", *Robot_num[1][0], *Robot_num[1][1]);
		//printf("%d    %d\n", *Robot_num[2][0], *Robot_num[2][1]);
		if (abs(*R_Data[num][2]) < 170)
		{
			Angle_Check(num, 180);
		}


		if (Target_y - 10 <= *R_Data[num][1] && *R_Data[num][1] <= Target_y + 10)
		{
			//SendData(num, M_STOP);
			if (Target_x > *R_Data[num][0])
			{
				SendData(num, M_BackRun40);
			}
			else
			{
				SendData(num, M_CenterRun30);
			}

		}
		else
		{

			SendData(num, M_STOP);
			LR_CHECK(num, Target_x, Target_y);
		}






		/**********************************************************************************************************************************/
		//Check
		/*int distance = (Present_x - Target_x)*(Present_x - Target_x) + (Present_y - Target_y)*(Present_y - Target_y);
		printf("%d\n", distance);
		if (distance < 2*CHECK_NEAR*CHECK_NEAR)
		{
		Flag_MoveRobot = 0;
		}*/
	}

	void CImageProcessDlg::OnBnClickedRobotStop()
	{
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		Flag_MoveRobot = 0;
	}


	void CImageProcessDlg::LR_CHECK(int num, int Target_x, int Target_y)
	{
		int *R_Data[3][3] = { { &Robot_Info[0][0], &Robot_Info[0][1], &Robot_Info[0][2] }, { &Robot_Info[1][0], &Robot_Info[1][1], &Robot_Info[1][2] }, { &Robot_Info[2][0], &Robot_Info[2][1], &Robot_Info[2][2] } };
		if (abs(Target_y - *R_Data[num][1]) <= 10)
		{
			SendData(num, M_STOP);
		}
		else if (Target_y < *R_Data[num][1])
		{
			if (*R_Data[num][1] - Target_y> 30)
			{
				if (num != 1)
				SendData(num, M_Right70);
				else
					SendData(num, M_Right20);
			}
			else
				SendData(num, M_Right20);

		}
		else if (Target_y > *R_Data[num][1])
		{
			if (Target_y - *R_Data[num][1] > 30)
			{
				if (num != 1)
				SendData(num, M_Left70);
				else
				SendData(num, M_Left20);
			}
			else
				SendData(num, M_Left20);
		}
	}

	void CImageProcessDlg::OnCbnSelchangeCombo1()
	{
		UpdateData(true);
	}



	void CImageProcessDlg::OnBnClickedRadio6()
	{
		UpdateData(true);
		m_MIndex_Robot = 0;
	}


	void CImageProcessDlg::OnBnClickedRadio7()
	{
		UpdateData(true);
		m_MIndex_Robot = 1;
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	}


	void CImageProcessDlg::OnBnClickedRadio8()
	{
		UpdateData(true);
		m_MIndex_Robot = 2;
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	}


	void CImageProcessDlg::Target_Move(int num, int Target_x, int Target_y, int Type)
	{
		double C_Angle = 0, P_x, P_y, R_A;
		int *R_Data[3][3] = { { &Robot_Info[0][0], &Robot_Info[0][1], &Robot_Info[0][2] }, { &Robot_Info[1][0], &Robot_Info[1][1], &Robot_Info[1][2] }, { &Robot_Info[2][0], &Robot_Info[2][1], &Robot_Info[2][2] } };
		R_A = *R_Data[num][2]; // 로봇의 뱡향 각도

		if (R_A < 0) // 로봇의 방향 각도를 0~360도로 변환
		{
			R_A = R_A + 360;
		}
		P_x = Target_x - *R_Data[num][0]; // 현재의 공좌표와 로봇 의 거리 (X,Y)값
		P_y = Target_y - *R_Data[num][1];
		//로봇을 기점으로 공이 있는 좌표 방향 각도를 0~360도로 표현
		if (P_y < 0)
		{
			P_y *= -1;
			if (P_x < 0)
			{
				P_x *= -1; 
				C_Angle = 90 + (atan(P_x / P_y) * (180.0 / 3.14));
				//C_Angle = 10 * (int)(((int)(C_Angle / 5) + 1) / 2);
			}
			else
			{
				C_Angle = atan(P_y / P_x) * (180.0 / 3.14);
				//	C_Angle = 10 * (int)(((int)(C_Angle / 5) + 1) / 2);
			}
		}
		else
		{
			if (P_x < 0)
			{

				P_x *= -1;
				C_Angle = 180 + (atan(P_y / P_x) * (180.0 / 3.14));
				//C_Angle = 10 * (int)(((int)(C_Angle / 5) + 1) / 2);

			}
			else
			{
				C_Angle = 270 + (atan(P_x / P_y) * (180.0 / 3.14));
				//	C_Angle = 10 * (int)(((int)(C_Angle / 5) + 1) / 2);

			}
		}
		//printf("%lf     ,     %lf\n", R_A, C_Angle);
		C_Angle = C_Angle - R_A; // 공이있는 방향 각도 값에서 로봇이 바라보는 방향 각도값을 빼서 차이를 확인
		//절대값 180이 넘으면 각도를 변환해 더 가까운 방향을 찾음
		if (C_Angle < -180)
		{
			C_Angle = C_Angle + 360;
		}
		else if (C_Angle > 180)
		{
			C_Angle = C_Angle - 360;
		}
		// 공방향과 로봇이바라보는 각도의 차이가 0보다 크면 왼쪽, 작으면 오른쪽으로 턴을 함 -10 ~ 10 범위에서는 전진
		if (C_Angle > 0)
		{
			if (C_Angle >= 60)
			{
				SendData(num, M_LeftTurn45);
			}
			else if (C_Angle >= 45)
			{
				SendData(num, M_LeftTurn20);
			}
			else if (C_Angle >= 20)
			{
				if (abs(P_Ball_x - *R_Data[num][0]) < 40 && abs(P_Ball_y - *R_Data[num][1]) < 40)
					SendData(num, M_LeftTurn10);
				else
					goto mygoto1;
			}
			/*else if (C_Angle >= 10)
			{
				SendData(num, M_LeftTurn10);
			}*/
			else
			{
				mygoto1:
				if (Type == 1)
				{
					//SendData(num, M_STOP);
					SendData(num, M_CenterRun30);
				}
				else if (Type == 2)
				{
					
					G_Sign[0] = 0;
					if (ST == 0)
					{
						SendData(num, M_CenterRun50);
						ST++;
					}
					else if (ST == 1)
					{
						SendData(num, M_RightShoot);
						ST++;
					}
					else if (ST == 2)
					{
						SendData(num, M_LeftShoot);
						ST++;
					}
					else
					{
						G_Sign[0] = 0;
						ST = 0;
					}
				}
				else
				{
					SendData(num, M_STOP);
				}
			}
		}
		else
		{
			if (C_Angle <= -60)
			{
				SendData(num, M_RightTurn45);
			}
			else if (C_Angle <= -45)
			{
				SendData(num, M_RightTurn20);
			}
			else if (C_Angle <= -20)
			{
				SendData(num, M_RightTurn10);
				if (abs(P_Ball_x - *R_Data[num][0]) < 40 && abs(P_Ball_y - *R_Data[num][1]) < 40)
					SendData(num, M_LeftTurn10);
				else
					goto mygoto2;
			}
			/*else if (C_Angle <= -10)
			{
				SendData(num, M_RightTurn10);
			}*/
			else
			{
				mygoto2:
				if (Type == 1)
				{
					//SendData(num, M_STOP);
					SendData(num, M_CenterRun30);
				}
				else if (Type == 2)
				{
					
					G_Sign[0] = 0;
					if (ST == 0)
					{
						SendData(num, M_CenterRun50);
						ST++;
					}
					else if (ST == 1)
					{
						SendData(num, M_RightShoot);
						ST++;
					}
					else if (ST == 2)
					{
						SendData(num, M_LeftShoot);
						ST++;
					}
					else
					{
						G_Sign[0] = 0;
						ST = 0;
					}
					

				}
				else
					SendData(num, M_STOP);
			}
		}
	}

	void CImageProcessDlg::OnEnChangeCutymin()
	{
		UpdateData(true);
	}


	void CImageProcessDlg::OnEnChangeCutxmax()
	{
		UpdateData(true);
	}


	void CImageProcessDlg::OnEnChangeCutymax()
	{
		UpdateData(true);
	}


	void CImageProcessDlg::OnEnChangeCutxmin()
	{
		UpdateData(true);
	}


	void CImageProcessDlg::Speed_Check(int x, int y, int x_1, int y_1, int t) // 속도체크 
	{
		int distance = (int)abs(sqrt((x_1 - x) - (y_1 - y)));
		int time = t;
		speed = distance / time;

		std::cout << "distance : " << distance << " time : " << t << " speed : " << speed << "m/s" << std::endl;
	}

	int CImageProcessDlg::GoalKeeper(int position)
	{
		switch (position)
		{
		case 1:
			if ((N_Ball_y1 < 240 && N_Ball_y1 > 100) || (N_Ball_y1 < 240 && N_Ball_y1 > 100))
			{
				if (speed >= 3)
				{
					SendData(1, M_RightBlock);
					return 1;
				}

			}

			else if ((N_Ball_y1 > P_Robot2_y && N_Ball_y1 < 400) || (N_Ball_y2 < 400 && N_Ball_y2 >  P_Robot2_y))
			{
				if (speed >= 3)
				{
					SendData(1, M_LeftBlock);
					return 1;
				}

			}

			break;

		case 2:
			if ((N_Ball_y1 <  P_Robot2_y && N_Ball_y1 > 100) || (P_Ball_y <  P_Robot2_y && P_Ball_y > 100))
			{
				if (speed >= 2 && P_Ball_x < N_Ball_x1)
				{
					SendData(1, M_RightBlock);
					return 1;
				}
			}
			else if ((N_Ball_y1 >  P_Robot2_y && N_Ball_y1 < 400) || (P_Ball_y < 400 && P_Ball_y >  P_Robot2_y))
			{
				if (speed >= 2 && P_Ball_x < N_Ball_x1)
				{
					SendData(1, M_LeftBlock);
					return 1;
				}

			}
			break;

		case 3:
			if ((N_Ball_y1 > 140 && N_Ball_y1 < 240) && (N_Ball_x1 > 510 && N_Ball_x1 < 635))
			{
				if (speed >= 1)
					SendData(1, M_RightBlock);
				return 1;
			}
			else if ((N_Ball_y1 >= 240 && N_Ball_y1 <= 340) && (N_Ball_x1 >= 510 && N_Ball_x1 <= 635))
			{
				if (speed >= 1)
					SendData(1, M_LeftBlock);
				return 1;
			}
		
		default:
			break;
		}
		return Keeper_Sign;
	}
	void CImageProcessDlg::Angle_Check(int num, int Angle)
	{
		int *R_Data[3][3] = { { &Robot_Info[0][0], &Robot_Info[0][1], &Robot_Info[0][2] }, { &Robot_Info[1][0], &Robot_Info[1][1], &Robot_Info[1][2] }, { &Robot_Info[2][0], &Robot_Info[2][1], &Robot_Info[2][2] } };

		int P_Angle = *R_Data[num][2] - Angle;

		if (P_Angle > 180)
		{
			P_Angle -= 360;
		}
		else if (P_Angle < -180)
		{
			P_Angle += 360;
		}

		if (P_Angle > 0)
		{

			if (P_Angle >= 45)
			{
				SendData(num, M_RightTurn45);
			}
			else if (P_Angle >= 20)
			{
				SendData(num, M_RightTurn20);
			}
			else if (P_Angle >= 10)
			{
				SendData(num, M_RightTurn10);
			}
			else
			{
				SendData(num, M_STOP);
			}
		}
		else
		{

			if (P_Angle <= -45)
			{
				SendData(num, M_LeftTurn45);
			}
			else if (P_Angle <= -20)
			{
				SendData(num, M_LeftTurn20);
			}
			else if (P_Angle <= -10)
			{
				SendData(num, M_LeftTurn10);
			}
			else
			{
				SendData(num, M_STOP);
			}
		}
	}

	void CImageProcessDlg::Kick(int num)
	{

		if (kick1 == 0)
		{
			if ((-60 <= (P_Ball_x - Robot_Info[num][0]) && (P_Ball_x - Robot_Info[num][0]) < 60) && (-60 <= (P_Ball_y - Robot_Info[num][1]) && (P_Ball_y - Robot_Info[num][1]) < 60))
			{
				Target_Move(num, P_Ball_x, P_Ball_y, 0);
				kick1 = 1;
			}
			else
			{
				Target_Move(num, P_Ball_x, P_Ball_y, 1);
			}
		}
		else if (kick1 == 1)
		{
			if ((P_Ball_x - Robot_Info[num][0]) *(P_Ball_x - Robot_Info[num][0]) + (P_Ball_y - Robot_Info[num][1])*(P_Ball_y - Robot_Info[num][1]) < 20 * 20)
			{
				Target_Move(num, P_Ball_x, P_Ball_y, 0);
				kick1 = 2;
			}
			else
			{
				if (((-60 > (P_Ball_x - Robot_Info[num][0]) || (P_Ball_x - Robot_Info[num][0]) > 60)) || ((-60 > (P_Ball_y - Robot_Info[num][1]) && (P_Ball_y - Robot_Info[num][1]) > 60)))
					kick1 = 0;
				else
					Target_Move(num, P_Ball_x, P_Ball_y, 2);
			}
		}

		else if (kick1 == 2)
		{
			Target_Move(num, P_Ball_x, P_Ball_y, 0);
			kick1 = 3;
		}
		else if (kick1 == 3)
		{
			Target_Move(num, P_Ball_x, P_Ball_y, 0);
			kick1 = 4;
		}
		else if (kick1 == 4)
		{
			Target_Move(num, P_Ball_x, P_Ball_y, 0);
			kick1 = 5;
		}
		else if (kick1 == 5)
		{
			SendData(num, M_LeftShoot);
			SendData(num, M_RightShoot);
			kick1 = 0;
		}

	}


	void CImageProcessDlg::OnBnClickedButton5()
	{

		Stop_F = 1;
		printf("프리킥\n");
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	}


	void CImageProcessDlg::OnBnClickedButton3()
	{
		Stop_P = 1;
		printf("패널티킥\n");
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	}


	void CImageProcessDlg::OnBnClickedButton6()
	{
		Stop_F = 0;
		Stop_P = 0;
		printf("취소\n");
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	}
