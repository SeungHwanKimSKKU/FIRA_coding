
// ImageProcessDlg.h : ???? ????
//

#pragma once

#include "CvvImage.h"
#include "highgui.h"
#include "cv.h"
#include "Comm.h"

//Motion Define
#define M_LeftBlock            1
#define M_CenterBlock         2
#define M_RightBlock         3
#define M_Ceremony            4
#define M_CenterRun50         5
#define M_CenterRun30         6
#define M_LeftTurn10         7
#define M_LeftTurn20         8
#define M_LeftTurn45         9
#define M_LeftTurn60         10
#define M_RightTurn10         11
#define M_RightTurn20         12
#define M_RightTurn45         13
#define M_RightTurn60         14
#define M_Right20            15
#define M_Right70            16
#define M_Left20            17
#define M_Left70            18
#define M_BackRun40            19
#define M_LittleStep           20
#define M_BackStep            21
#define M_RightShoot         22
#define M_LeftShoot            23
#define M_RightShooting         24
#define M_LeftShooting         25
#define M_RightBackShooting      26
#define M_LeftBackShooting      27
#define M_RightBlock2         28
#define M_LeftBlock2         29
#define M_CenterStep         30
#define M_MAXARRAY            31
#define M_STOP               32

//Time Define
#define T_LeftBlock   1000
#define T_CenterBlock   1000
#define T_RightBlock   1000
#define T_CereTony   1000
#define T_CenterRun50   1000
#define T_CenterRun30   1000
#define T_LeftTrun10   1000
#define T_LeftTrun20   1000
#define T_LeftTurn45   1000
#define T_LeftTurn60   1000
#define T_RightTrun10   1000
#define T_RightTrun20   1000
#define T_RightTrun45   1000
#define T_RightTrun60   1000
#define T_Right20   1000
#define T_Right70   1000
#define T_Left20   1000
#define T_Left70   1000
#define T_BackRun40   1000
#define T_STOP   500
#define T_BackStep   1000
#define T_RightShoot   1000
#define T_LeftShoot   1000
#define T_RightShooting   1000
#define T_LeftShooting   1000
#define T_RightBackShooting   1000
#define T_LeftBackShooting   1000
#define T_RightBlock2   1000
#define T_LeftBlock2   1000
#define T_CenterStep   1000


// CImageProcessDlg ??ȭ ????
class CImageProcessDlg : public CDialogEx
{
// ?????Դϴ?.
public:
	CImageProcessDlg(CWnd* pParent = NULL);	// ǥ?? ???????Դϴ?.

	//Thread	
	void CreateThread1(UINT _method);
	bool DestroyThread1(void);
	void CreateThread2(UINT _method);
	bool DestroyThread2(void);
	int ThreadFunction(void);
	int ThreadFunction2(void);
	

	// Serial Comm
	TTYSTRUCT Int2TTY();		//Init Com fucntion
	TTYSTRUCT LoadSettings();	//Load Previous setting
	void SaveSettings();

	//Image Process
	void Init_ImageProcess(CvSize img_size);
	void ImageProcess(CvSize img_size);

	//Moving Process
	void RobotMove(int num, int Target_x, int Target_y, int pixel_gap);
	void FrontMove(int num, int Target_x, int Target_y, int F_A);
	void BackMove(int num, int Target_x, int Target_y, int pixel_gap_x, int pixel_gap_l_y, int pixel_gap_r_y);
	void ShootPosition(int num, int Target_x, int Target_y, int pixel_gap_x, int pixel_gap_l_y, int pixel_gap_r_y);
	void Pre_RobotMove(int num, int Target_x, int Target_y);
	void Pre_LR_CHECK(int num, int Target_x, int Target_y);
		

// ??ȭ ???? ???????Դϴ?.
	enum { IDD = IDD_IMAGEPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ?????Դϴ?.	


// ?????Դϴ?.
protected:
	HICON m_hIcon;
	CComm m_Comm;													//Serial Comm Struct

	//Image Matrix
	IplImage* img_Cam;
	IplImage* img_Process;
	IplImage* img_Threshold;
	IplImage* img_Depth;
	IplImage* img_YCbCr;
	IplImage* img_Cb;
	IplImage* img_Cr;
	IplImage* img_Labling;
	IplImage* img_Capture = 0;
	//Postion
	IplImage* img_Pink;
	IplImage* img_Green;
	IplImage* img_Ball = 0;
	IplImage* img_Labeling = 0;
	IplImage* img_Show = 0;
	IplImage* img_Pattern1 = 0;
	IplImage* img_Pattern2 = 0;
	IplImage* img_Pattern3 = 0;
	IplImage* img_Pattern4 = 0;
	IplImage* img_GrayBall = 0;
	IplImage* img_Match = 0;
	
	//Position Information
	int P_Ball_x = 0;
	int P_Ball_y = 0;
	int N_Ball_x1 = 0;
	int N_Ball_y1 = 0;
	int N_Ball_x2 = 0;
	int N_Ball_y2 = 0;
	int N_Ball_x3 = 0;
	int N_Ball_y3 = 0;
	int N_Ball_x4 = 0;
	int N_Ball_y4 = 0;
	int N_Ball_x5 = 0;
	int N_Ball_y5 = 0;

	int P_Robot1_ID = 0;	
	int P_Robot1_x = 0;
	int P_Robot1_y = 0;
	int P_Robot1_a = 0;
	
	int P_Robot2_ID = 0;
	int P_Robot2_x = 0;
	int P_Robot2_y = 0;
	int P_Robot2_a = 0;

	int P_Robot3_ID = 0;
	int P_Robot3_x = 0;
	int P_Robot3_y = 0;
	int P_Robot3_a = 0;

	
	CvPoint P_Ball;								//Ball
	CvPoint N_Ball1;
	CvPoint N_Ball2;   
	CvPoint N_Ball3;   
	CvPoint N_Ball4;   
	CvPoint N_Ball5;                             //Ballnext
	CvPoint P_Robot1, P_Robot2, P_Robot3;		//Robot
	CvPoint P_Pattern1, P_Pattern2, P_Pattern3;	//Pattern
	
	// ?????? ?޽??? ?? ?Լ?
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LONG OnReceiveData(UINT wParam, LPARAM lParam);			//Serial Comm Rx
	afx_msg bool SendData(int index, int commend);					//Serial Comm Tx
	afx_msg int CImageProcessDlg::Send_MotionRemoteControl_Write_NoResponse(int Robot_Id, int addr, int motion_code, int speed_override);

	DECLARE_MESSAGE_MAP()
public:

	//Variable
	CWinThread* pThread1 = NULL;
	CWinThread* pThread2 = NULL;
	static volatile bool isThreadRunning;

	//Serial Com
	int m_nSettingBaudrate;
	int m_nSettingPort;
	int m_nSettingData;
	int m_nSettingParity;
	int m_nSettingFlow;
	int m_nSettingStop;
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedOk();

	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	int m_Index_RightFigure;
	afx_msg void OnBnClickedGo();
	afx_msg void OnBnClickedStop();	
	
	//Color Threshold
	int Cr_Th_Lb, Cr_Th_Ub;
	int Cb_Th_Lb, Cb_Th_Ub;
	//Orange
	int Cr_Orange_Th_L, Cr_Orange_Th_U;
	int Cb_Orange_Th_L, Cb_Orange_Th_U;
	//Blue
	int Cr_Blue_Th_L, Cr_Blue_Th_U;
	int Cb_Blue_Th_L, Cb_Blue_Th_U;
	//Yellow
	int Cr_Yellow_Th_L, Cr_Yellow_Th_U;
	int Cb_Yellow_Th_L, Cb_Yellow_Th_U;
	//Red
	int Cr_Red_Th_L, Cr_Red_Th_U;
	int Cb_Red_Th_L, Cb_Red_Th_U;
	//Pink
	int Cr_Pink_Th_L, Cr_Pink_Th_U;
	int Cb_Pink_Th_L, Cb_Pink_Th_U;
	//Green
	int Cr_Green_Th_L, Cr_Green_Th_U;
	int Cb_Green_Th_L, Cb_Green_Th_U;
	afx_msg void OnEnChangeCrThLb();
	afx_msg void OnEnChangeCrThUb();
	afx_msg void OnEnChangeCbThLb();
	afx_msg void OnEnChangeCbThUb();
	afx_msg void OnEnChangeSizeTh();

	//Option
	int Color;
	int Select_Color;
	int Size_Th;
	int ROI;
	afx_msg void OnCbnSelchangeColorTh();
	afx_msg void OnEnChangeRoi();
	afx_msg void OnCbnSelchangeColor();
		
	int m_CIndex_Robot;
	int m_Ccommend_robot;
	afx_msg void OnBnClickedSendcommend();

	int m_RobotPosition_X=0;
	int m_RobotPosition_Y=0;
	int m_MIndex_Robot;
	int m_TargetPosition_X = 0;
	int m_TargetPosition_Y = 0;
	afx_msg void OnBnClickedRun();	
	afx_msg void OnCbnSelchangeComboRightfigure();
	afx_msg void OnCbnSelchangeRobotindexMove();
	afx_msg void OnBnClickedRobotMove();
	afx_msg void OnBnClickedRobotStop();
	afx_msg void LR_CHECK(int num, int Target_x, int Target_y, int pixel_gap_l_y, int pixel_gap_r_y);
	afx_msg void GO_Move(int num, int Target_x, int Target_y, int Type);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void Target_Move( int num, int Target_x, int Target_y, int Type);
	afx_msg void CImageProcessDlg::Speed_Check(int x, int y, int x_1, int y_1, int t);
	int CImageProcessDlg::GoalKeeper(int position);
	afx_msg void Angle_Check(int num, int Angle);
	afx_msg void CImageProcessDlg::Kick(int num);
	afx_msg void Keeper_Motion();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio8();

	//int T_Array[M_MAXARRAY];
	int Cut_y_min;
	int Cut_y_max;
	int Cut_x_max;
	int Cut_x_min;
	afx_msg void OnEnChangeCutymin();
	afx_msg void OnEnChangeCutxmax();
	afx_msg void OnEnChangeCutymax();
	afx_msg void OnEnChangeCutxmin();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnEnChangeRobotpositionX();
	afx_msg void OnStnClickedMain();
};
