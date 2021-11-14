#include <windows.h>
#include <tchar.h>
#include <corecrt_math.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

const HBRUSH RECT_BRUSH = CreateSolidBrush(RGB(50, 160, 120));

HBITMAP movableBitmap;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM             MyRegisterClass(HINSTANCE);
BOOL             InitInstance(HINSTANCE, INT);

HBITMAP          PngToBitmap(WCHAR* pngFilePath);
void             drawBmp(HDC hdc, int x, int y, HBITMAP hBitmap);
void             Jump(int *x, int *y, int rectSize, RECT clientRect, int step);

INT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);
	InitInstance(hInstance, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (INT)msg.wParam;
}

LRESULT WINAPI MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hBrush;
	HDC hdc;
	PAINTSTRUCT ps;

	static HBITMAP hBmp;
	WCHAR pic[] = L"pic.png";

	static RECT clientRect;
	
	static BOOL isPic = FALSE;
	static int rectSize = 128;
	static int x, y;
	static int step = 20;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		hBrush = RECT_BRUSH;

		GetClientRect(hWnd, &clientRect);
		x = (clientRect.right - clientRect.left - rectSize) / 2;
		y = (clientRect.bottom - clientRect.top - rectSize) / 2;
		hBmp = PngToBitmap(pic);
	}
	break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		if (!isPic)
		{
			SelectObject(hdc, hBrush);
			Rectangle(hdc, x, y, x + rectSize, y + rectSize);
		}
		else
		{
			drawBmp(hdc, x, y, hBmp);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
			isPic = !isPic;
		}
		break;
		case VK_LEFT:
		case 'A':
		{
			x -= step;
		}
		break;
		case VK_UP:
		case 'W':
		{
			y -= step;
		}
		break;
		case VK_RIGHT:
		case 'D':
		{
			x += step;
		}
		break;
		case VK_DOWN:
		case 'S':
		{
			y += step;
		}
		break;
		}

		Jump(&x, &y, rectSize, clientRect, step * 2);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			if (LOWORD(wParam) == MK_SHIFT)
			{
				x += step;
			}
			else
			{
				y -= step;
			}
		}
		else
		{
			if (LOWORD(wParam) == MK_SHIFT)
			{
				x -= step;
			}
			else
			{
				y += step;
			}
		}

		Jump(&x, &y, rectSize, clientRect, step * 2);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_MOUSEMOVE: 
	{
		x = LOWORD(lParam) - rectSize/2;
		y = HIWORD(lParam) - rectSize/2;
		if (x < clientRect.left)
		{
			x = clientRect.left + step;
		}
		if (x > clientRect.right - rectSize)
		{
			x = clientRect.right - rectSize - step;
		}
		if (y < clientRect.top)
		{
			y = clientRect.top + step;
		}
		if (y > clientRect.bottom - rectSize)
		{
			y = clientRect.bottom - rectSize - step;
		}
		Jump(&x, &y, rectSize, clientRect, step * 2);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break; 
	case WM_SIZE:
	{
		GetClientRect(hWnd, &clientRect);
		if (x < clientRect.left)
		{
			x = clientRect.left + 5;
		}
		if (y > clientRect.bottom - rectSize)
		{
			y = clientRect.bottom - rectSize - 5;
		}
		if (x > clientRect.right - rectSize)
		{
			x = clientRect.right - rectSize -5;
		}
		if (y < clientRect.top)
		{
			y = clientRect.top + 5;
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_DESTROY:
	{
		DeleteObject(hBrush);
		DeleteObject(hBmp);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}

void Jump(int *x, int *y, int rectSize, RECT clientRect, int step)
{
	if (*x < clientRect.left)
	{
		*x += step;
	}
	if (*y > clientRect.bottom - rectSize)
	{
		*y -= step;
	}
	if (*x > clientRect.right - rectSize)
	{
		*x -= step;
	}
	if (*y < clientRect.top)
	{
		*y += step;
	}
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.hInstance = hInstance;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.lpfnWndProc = MyWindowProc;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"WinAPI";

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
	HWND hWnd = CreateWindow(L"WinAPI", L"Sprite", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, 0, 720, 480, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void drawBmp(HDC hdc, int x, int y, HBITMAP hBitmap) {
	HBITMAP hNewBmp;

	HDC compDc;
	BITMAP bmp;
	int bmpWidth, bmpHeight;

	compDc = CreateCompatibleDC(hdc);

	hNewBmp = (HBITMAP)SelectObject(compDc, hBitmap);

	if (hNewBmp) {
		SetMapMode(compDc, GetMapMode(hdc));
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);

		bmpWidth = bmp.bmWidth;
		bmpHeight = bmp.bmHeight;

		POINT bmpSize;
		bmpSize.x = bmpWidth;
		bmpSize.y = bmpHeight;

		DPtoLP(hdc, &bmpSize, 1);
		
		POINT point;
		point.x = 0;
		point.y = 0;

		DPtoLP(compDc, &point, 1);
		BitBlt(hdc, x, y, bmpWidth, bmpHeight, compDc, point.x, point.y, SRCCOPY);
		SelectObject(compDc, hNewBmp);
	}

	DeleteDC(compDc);
}

HBITMAP PngToBitmap(WCHAR* pngFilePath) {
	GdiplusStartupInput test;
	ULONG_PTR token;
	GdiplusStartup(&token, &test, NULL);
	Color Back = Color(Color::MakeARGB(0, 255, 255, 255));
	HBITMAP convertedBitmap = NULL;
	Bitmap* Bitmap = Bitmap::FromFile(pngFilePath, false);
	if (Bitmap) {
		Bitmap->GetHBITMAP(Back, &convertedBitmap);
		delete Bitmap;
	}
	GdiplusShutdown(token);
	return convertedBitmap;
}