
// ImageProcessDlg.h : 헤더 파일
//

#pragma once

#include "CvvImage.h"
#include "highgui.h"
#include "cv.h"
#include "Comm.h"

//Motion Define
#define GO			2
#define STOP		32


// CImageProcessDlg 대화 상자
class CImageProcessDlg : public CDialogEx
{
// 생성입니다.
public:
	CImageProcessDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	//Thread	
	void CreateThread(UINT _method);
	bool DestoryThread(void);
	int ThreadFunction(void);
	

	// Serial Comm
	TTYSTRUCT Int2TTY();		//Init Com fucntion
	TTYSTRUCT LoadSettings();	//Load Previous setting
	void SaveSettings();

	//Image Process
	void Init_ImageProcess(CvSize img_size);
	void ImageProcess(CvSize img_size);

	//Moving Process
	void RobotMove(int Present_x, int Present_y, int angle, int Target_x, int Target_y);
		

// 대화 상자 데이터입니다.
	enum { IDD = IDD_IMAGEPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.	


// 구현입니다.
protected:
	HICON m_hIcon;
	CComm m_Comm;													//Serial Comm Struct

	//Image Matrix
	IplImage* img_Cam = 0;
	IplImage* img_Process = 0;
	IplImage* img_Gray = 0;
	IplImage* img_Threshold = 0;
	IplImage* img_Depth = 0;
	IplImage* img_YCbCr;
	IplImage* img_Cb;
	IplImage* img_Cr;
	IplImage* img_Labling = 0;
	IplImage* img_Capture = 0;
	//Postion
	IplImage* img_Ball = 0;
	IplImage* img_Edge = 0;
	IplImage* img_Robot = 0;	

	IplImage* img_Labeling = 0;
	IplImage* img_Show = 0;

	IplImage* img_Pattern1 = 0;
	IplImage* img_Pattern2 = 0;
	IplImage* img_Pattern3 = 0;
	IplImage* img_Pattern4 = 0;

	IplImage* img_GrayBall = 0;

	IplImage* img_Match = 0;

	int ball_x;
	int ball_y;

	//IplImage temp0;
	//IplImage image;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LONG OnReceiveData(UINT wParam, LPARAM lParam);			//Serial Comm Rx
	afx_msg bool SendData(int index, int commend);					//Serial Comm Tx

	DECLARE_MESSAGE_MAP()
public:

	//Variable
	CWinThread* pThread = NULL;
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
	afx_msg void OnBnClickedGo();
	afx_msg void OnBnClickedStop();	
	
	int Cr_Th_Lb;
	int Cr_Th_Ub;
	int Cb_Th_Lb;
	int Cb_Th_Ub;
	int Gr_Th;
	int Size_Th;
	afx_msg void OnEnChangeCrThLb();
	afx_msg void OnEnChangeCrThUb();
	afx_msg void OnEnChangeCbThLb();
	afx_msg void OnEnChangeCbThUb();
	afx_msg void OnEnChangeGrTh();
	afx_msg void OnEnChangeSizeTh();
	afx_msg void OnEnChangeGrTh2();
	int Gr_Th2;
	int Cr_Th_Lb2;
	int Cb_Th_Ub2;
	int Cb_Th_Lb2;
	int Cr_Th_Ub2;
	afx_msg void OnEnChangeCrThLb2();
	afx_msg void OnEnChangeCrThUb2();
	afx_msg void OnEnChangeCbThLb2();
	afx_msg void OnEnChangeCbThUb2();
	int m_CIndex_Robot;
	int m_Ccommend_robot;
	afx_msg void OnBnClickedSendcommend();

	int m_RobotPosition_X=0;
	int m_RobotPosition_Y=0;
	int m_MIndex_Robot;
	int m_TargetPosition_X = 0;
	int m_TargetPosition_Y = 0;
	afx_msg void OnBnClickedRun();
	int m_Index_RightFigure;
	afx_msg void OnCbnSelchangeComboRightfigure();
	afx_msg void OnCbnSelchangeRobotindexMove();
	afx_msg void OnBnClickedRobotMove();
	afx_msg void OnBnClickedRobotStop();
	afx_msg void OnEnChangeRobotpositionX();
	afx_msg void OnEnChangeRobotpositionY();
	afx_msg void OnEnChangeTargetpositionX();
	afx_msg void OnEnChangeCommend();
	afx_msg void LR_CHECK(int Present_x, int Present_y, int Target_x, int Target_y);
};
