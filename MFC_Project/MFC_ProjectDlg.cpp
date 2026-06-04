
// MFC_ProjectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFC_Project.h"
#include "MFC_ProjectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCProjectDlg 대화 상자



CMFCProjectDlg::CMFCProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_PROJECT_DIALOG, pParent)
	, m_nRadius(0)
	, m_nThickness(0)
	, m_strP1(_T(""))
	, m_strP2(_T(""))
	, m_strP3(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_nRadius);
	DDX_Text(pDX, IDC_EDIT_THICKNESS, m_nThickness);
	DDX_Text(pDX, IDC_STATIC_P1, m_strP1);
	DDX_Text(pDX, IDC_STATIC_P2, m_strP2);
	DDX_Text(pDX, IDC_STATIC_P3, m_strP3);
}

BEGIN_MESSAGE_MAP(CMFCProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()

	ON_BN_CLICKED(IDC_BTN_RESET, &CMFCProjectDlg::OnBnClickedBtnReset)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CMFCProjectDlg::OnBnClickedBtnRandom)
	ON_MESSAGE(WM_USER + 1, &CMFCProjectDlg::OnRandomUpdate)
END_MESSAGE_MAP()


// CMFCProjectDlg 메시지 처리기

BOOL CMFCProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.\

	m_nPointCount = 0;
	m_cx = m_cy = m_radius = 0.0;
	m_nRadius = 10;
	m_nThickness = 1;
	m_strP1 = _T("P1: (-, -)");
	m_strP2 = _T("P2: (-, -)");
	m_strP3 = _T("P3: (-, -)");
	m_nDragIndex = -1;
	m_bDragging = false;
	UpdateData(FALSE);  // 변수 → 화면 반영
	m_bThreadRunning = false;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCProjectDlg::DrawCircle(CDC* pDC, int cx, int cy, int radius)
{
	const int steps = 360;
	double angleStep = 2.0 * 3.14159265 / steps;

	int prevX = cx + (int)(radius * cos(0));
	int prevY = cy + (int)(radius * sin(0));

	for (int i = 1; i <= steps; i++)
	{
		double angle = i * angleStep;
		int x = cx + (int)(radius * cos(angle));
		int y = cy + (int)(radius * sin(angle));

		pDC->MoveTo(prevX, prevY);
		pDC->LineTo(x, y);

		prevX = x;
		prevY = y;
	}
}

void CMFCProjectDlg::OnPaint()
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
		CPaintDC dc(this);

		// 클릭 지점 원 그리기 (3개까지)
		for (int i = 0; i < m_nPointCount; i++)
		{
			DrawCircle(&dc, m_points[i].x, m_points[i].y, m_nRadius);
		}

		// 3점 찍혔을 때 외접원 그리기
		if (m_nPointCount == 3)
		{
			if (CalcCircumCircle(m_points[0], m_points[1], m_points[2], m_cx, m_cy, m_radius))
			{
				CPen pen(PS_SOLID, m_nThickness, RGB(0, 0, 0));
				CPen* pOldPen = dc.SelectObject(&pen);
				DrawCircle(&dc, (int)m_cx, (int)m_cy, (int)m_radius);
				dc.SelectObject(pOldPen);
			}
		}
	}
}
bool CMFCProjectDlg::CalcCircumCircle(CPoint p1, CPoint p2, CPoint p3, double& cx, double& cy, double& r)
{
	double ax = p2.x - p1.x, ay = p2.y - p1.y;
	double bx = p3.x - p1.x, by = p3.y - p1.y;

	double D = 2.0 * (ax * by - ay * bx);
	if (fabs(D) < 1e-10)
		return false;  // 세 점이 일직선

	double ux = (by * (ax * ax + ay * ay) - ay * (bx * bx + by * by)) / D;
	double uy = (ax * (bx * bx + by * by) - bx * (ax * ax + ay * ay)) / D;

	cx = p1.x + ux;
	cy = p1.y + uy;
	r = sqrt(ux * ux + uy * uy);

	return true;
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCProjectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	UpdateData(TRUE);

	// 히트테스트 - 3점 다 찍힌 상태에서 기존 점 클릭했는지 확인
	if (m_nPointCount == 3)
	{
		for (int i = 0; i < 3; i++)
		{
			int dx = point.x - m_points[i].x;
			int dy = point.y - m_points[i].y;
			if (sqrt((double)(dx * dx + dy * dy)) <= m_nRadius)
			{
				m_bDragging = true;
				m_nDragIndex = i;
				SetCapture();
				return;
			}
		}
	}

	// 새 점 찍기 (3개 미만일 때만)
	if (m_nPointCount < 3)
	{
		m_points[m_nPointCount] = point;
		m_nPointCount++;

		CString str;
		str.Format(_T("P%d: (%d, %d)"), m_nPointCount, point.x, point.y);
		if (m_nPointCount == 1) m_strP1 = str;
		else if (m_nPointCount == 2) m_strP2 = str;
		else if (m_nPointCount == 3) m_strP3 = str;

		UpdateData(FALSE);
		Invalidate();
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMFCProjectDlg::OnBnClickedBtnReset()
{
	m_nPointCount = 0;
	m_cx = m_cy = m_radius = 0.0;
	m_nRadius = 10;
	m_nThickness = 1;
	m_strP1 = _T("P1: (-, -)");
	m_strP2 = _T("P2: (-, -)");
	m_strP3 = _T("P3: (-, -)");
	UpdateData(FALSE);
	Invalidate();
}

void CMFCProjectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		m_nDragIndex = -1;
		ReleaseCapture();  // 마우스 캡처 해제
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CMFCProjectDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging && m_nDragIndex >= 0)
	{
		m_points[m_nDragIndex] = point;

		// 좌표 표시 업데이트
		CString str;
		str.Format(_T("P%d: (%d, %d)"), m_nDragIndex + 1, point.x, point.y);
		if (m_nDragIndex == 0) m_strP1 = str;
		else if (m_nDragIndex == 1) m_strP2 = str;
		else if (m_nDragIndex == 2) m_strP3 = str;

		UpdateData(FALSE);
		Invalidate();
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMFCProjectDlg::OnBnClickedBtnRandom()
{
	if (m_nPointCount != 3 || m_bThreadRunning)
		return;

	AfxBeginThread(RandomMoveThread, this);
}

LRESULT CMFCProjectDlg::OnRandomUpdate(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	Invalidate();
	return 0;
}

UINT CMFCProjectDlg::RandomMoveThread(LPVOID pParam)
{
	CMFCProjectDlg* pDlg = (CMFCProjectDlg*)pParam;
	pDlg->m_bThreadRunning = true;

	CRect rect;
	pDlg->GetClientRect(&rect);

	srand((unsigned int)time(NULL));

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pDlg->m_points[j].x = rand() % rect.Width();
			pDlg->m_points[j].y = rand() % rect.Height();
		}

		// 좌표 표시 업데이트
		for (int j = 0; j < 3; j++)
		{
			CString str;
			str.Format(_T("P%d: (%d, %d)"), j + 1, pDlg->m_points[j].x, pDlg->m_points[j].y);
			if (j == 0) pDlg->m_strP1 = str;
			else if (j == 1) pDlg->m_strP2 = str;
			else if (j == 2) pDlg->m_strP3 = str;
		}

		pDlg->PostMessage(WM_USER + 1, 0, 0);
		Sleep(500);
	}

	pDlg->m_bThreadRunning = false;
	return 0;
}
