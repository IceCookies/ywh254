#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <GdiPlus.h>
#include <string.h>

#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include "OpenFileDialog.h"

#include "Button.h"

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;
using namespace std;

WNDPROC   OrginProc;
HINSTANCE hApp;
HWND      hMainWindow;

HWND hTemplateImgBtn;
HWND hStartBtn;

IplImage* templateImg = NULL;
IplImage* pBackImg = NULL;

int BackWidth;
int BackHeight;

char* szClassName = "MainWindow";
int count_center_point;


struct MyPoint
{
	union 
	{
		CvPoint point;
		struct 
		{
			int x, y;
		};
	};
	double value;
};

MyPoint cen_point[1000000];


LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

char* GetNativeFile() 
{
	char* fileName;
	OpenFileDialog* openFileDialog = new OpenFileDialog();		
	openFileDialog->FilterIndex = 1;

	openFileDialog->Title = "Open Video File";

	if (openFileDialog->ShowDialog())
	{
		fileName = openFileDialog->FileName;
		if (fileName == 0)
		{
			MessageBox(NULL, "Can not Open file", "Warning", MB_OK);
			return 0;
		}				
		else 
		{
			return fileName;
		}
	}
	else
	{
		MessageBox(NULL, "Can not Open file", "Error", MB_OK);
		return 0;
	}
}

void TemplateBtnHandler()
{
	templateImg = cvLoadImage(GetNativeFile());
}

void SetWindow(char* name)
{
	HWND hVideo = (HWND)cvGetWindowHandle(name);
	HWND hVideoParent = GetParent(hVideo);
		
	if(hVideoParent != NULL)
	{
		SetWindowLong(hVideoParent,GWL_STYLE,((GetWindowLong(hVideoParent,GWL_STYLE) & ~WS_POPUP) & ~WS_OVERLAPPEDWINDOW ) | WS_CHILD );
		//SetWindowLong(hVideoParent,GWL_EXSTYLE,GetWindowLong(hVideoParent,GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE  & ~WS_EX_WINDOWEDGE  );
		SetWindowPos(hVideoParent,HWND_TOP,100,100, 200, 200, SWP_SHOWWINDOW );

		SetParent(hVideoParent,hMainWindow);

		//SetWindowPos(hVideoParent,HWND_TOP, 0, 0, 100, 100, SWP_SHOWWINDOW);
	}
}

void StartBtnHandler()
{
	CvCapture* pCapture = cvCaptureFromFile(GetNativeFile());

	IplImage* pFrame = NULL;

	int count_center_point = 0;
	
	cvNamedWindow("video", 1);
	SetWindow("video");

	while (1)
	{
		pFrame = cvQueryFrame(pCapture);
		
		if (pFrame == NULL)
			break;
		pBackImg = pFrame;
		int width = pFrame->width - templateImg->width + 1;
		int height = pFrame->height - templateImg->height + 1;

		IplImage* resultImg = cvCreateImage(cvSize(width, height), 32, 1);
		cvMatchTemplate(pFrame, templateImg, resultImg, CV_TM_CCOEFF);
		double minValue, maxValue;
		CvPoint minPoint;
		CvPoint maxPoint;

		cvMinMaxLoc(resultImg, &minValue, &maxValue, &minPoint, &maxPoint);
		
			cen_point[count_center_point].x = maxPoint.x + templateImg->width - 1;
			cen_point[count_center_point].y = maxPoint.y + templateImg->height - 1;
			cen_point[count_center_point].value = maxValue;
			count_center_point++;
		
		cvNamedWindow("result", 1);
		cvShowImage("result", resultImg);


		/*
		cvNamedWindow("video", 1);
	    HWND hVideo = (HWND)cvGetWindowHandle("video");
		HWND hVideoParent = GetParent(hVideo);
		
		if(hVideoParent != NULL)
		{
			SetWindowLong(hVideoParent,GWL_STYLE,((GetWindowLong(hVideoParent,GWL_STYLE) & ~WS_POPUP) & ~WS_OVERLAPPEDWINDOW ) | WS_CHILD | WS_THICKFRAME);
			//SetWindowLong(hVideoParent,GWL_EXSTYLE,GetWindowLong(hVideoParent,GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE  & ~WS_EX_WINDOWEDGE  );
			SetWindowPos(hVideoParent,HWND_TOP,200,100, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

			SetParent(hVideoParent,hMainWindow);

			//SetWindowPos(hVideoParent,HWND_TOP, 0, 0, 100, 100, SWP_SHOWWINDOW);
		}
		*/

		//SetWindowLong(hVideoParent,GWL_STYLE,~WS_VISIBLE );
		//ShowWindow(hVideoParent,SW_HIDE);
		//UpdateWindow(hVideoParent);
	    //ShowWindow(hMainWindow,SW_SHOW);
		//ShowWindow(hVideoParent,SW_SHOW);


		cvShowImage("video", pFrame);


		char c = cvWaitKey(1);
		if (c == 27)
			break;
	}

	IplImage* pTrackImg = cvCreateImage(cvGetSize(pBackImg), 8, 3);
	cvZero(pTrackImg);
	CvScalar color = {0, 255, 0};
	cvNamedWindow("Track", 1);
	for (int i = 0; i < count_center_point - 1; i++) 
	{
		cvLine(pTrackImg, cen_point[i].point, cen_point[i + 1].point, color);
		//cvWaitKey(100);
		cvShowImage("Track", pTrackImg);
	}
	
	
	
	
	cvWaitKey(0);
	cvReleaseCapture(&pCapture);
	cvDestroyWindow("result");
	cvDestroyWindow("Track");
	//cvReleaseImage(&pbkimg);
}


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{

	Gdiplus::GdiplusStartupInput gdiInput;
	ULONG_PTR gdiplusStartupToken;
	Gdiplus::GdiplusStartup(&gdiplusStartupToken,&gdiInput,NULL);

	memset(cen_point, 0, sizeof(cen_point));

	
	WNDCLASSEX wndclass;
	wndclass.cbSize        = sizeof(wndclass);
	wndclass.style         = CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc   = WinProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.hIconSm       = NULL;

	RegisterClassEx(&wndclass);

	HWND hwnd = ::CreateWindowEx(
		0,
		szClassName,
		"My First Window",
		WS_OVERLAPPEDWINDOW  ,
		100,
		100,
		640,
		480,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Error in Create Window", "Error", MB_OK);
		return -1;
	}

	hApp = hInstance;
	hMainWindow = hwnd;

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusStartupToken);


	return msg.wParam;	
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		{
			hTemplateImgBtn = (new Button(10, 10, 120, 30, L"SrcImg",hWnd, NULL, TemplateBtnHandler))->Create();
			hStartBtn = (new Button(150, 10, 120, 30, L"Start",hWnd, NULL, StartBtnHandler))->Create();
			return 0;
		}

	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);

			GetWindowRect(hWnd, &rect);

			EndPaint(hWnd, &ps);
			return 0;
		}



	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}



