#pragma once


//typedef LRESULT (CALLBACK *BtnWndProc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef void (*BtnHandler)();


class Button
{
public:
	Button::Button();
	Button::Button(int, int, int, int, WCHAR*, HWND, HINSTANCE, BtnHandler);
	~Button(void);

	HWND Create();
	HWND test(HWND parent, HINSTANCE hinstance);
	static LRESULT CALLBACK Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK ButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	//friend LRESULT CALLBACK BtnProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	

private:
	int x;
	int y;
	int width;
	int height;
	WCHAR* content;
	//int color;
	//int bgColor;

	HWND parent;
	HINSTANCE hInstance;

	WNDPROC OrginProc;
	BtnHandler Handler;

};

