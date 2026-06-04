
// MFC_ProjectDlg.h: 헤더 파일
//

#pragma once


// CMFCProjectDlg 대화 상자
class CMFCProjectDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCProjectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_PROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CPoint m_points[3];
	int m_nPointCount;
	double m_cx, m_cy, m_radius;  // 외접원 중심, 반지름
	void DrawCircle(CDC* pDC, int cx, int cy, int radius);
	bool CalcCircumCircle(CPoint p1, CPoint p2, CPoint p3, double& cx, double& cy, double& r);

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int m_nRadius;
	int m_nThickness;
	CString m_strP1;
	CString m_strP2;
	CString m_strP3;
};
