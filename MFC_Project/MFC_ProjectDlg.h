
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
	int m_nDragIndex;   // 드래그 중인 점 인덱스 (-1이면 드래그 안 하는 상태)
	bool m_bDragging;   // 드래그 중인지 여부
	static UINT RandomMoveThread(LPVOID pParam);
	bool m_bThreadRunning;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int m_nRadius;
	int m_nThickness;
	CString m_strP1;
	CString m_strP2;
	CString m_strP3;
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnRandom();
	afx_msg LRESULT OnRandomUpdate(WPARAM wParam, LPARAM lParam);
};
