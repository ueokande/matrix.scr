#include <windows.h> 
#include <scrnsave.h>
#include <time.h>
#include "resource.h"

#pragma comment(lib, "scrnsavw.lib")
#pragma comment(lib, "comctl32.lib")

#define ID_TIMER1 100
#define ID_TIMER2 101

typedef struct _tagPlace{
	BOOL bStart;
	INT nX;
	INT nY;
	INT nPict1;
	INT nPict2;
}PLACE;

const INT MAX_NUM = 100;

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HDC hdc_wnd;
	PAINTSTRUCT ps;
	RECT rc;
	static INT w, h, ch;
	static HDC hdc_main, hdc_mem, hdc_char[36][3];
	static HBITMAP hBmp_main, hBmp_mem, hBmp_char[36][3];
	static PLACE Place[MAX_NUM];
	HINSTANCE hInst;
	INT n = 0, m = 0;

	switch(msg) {
	case WM_CREATE:
		GetWindowRect(hWnd, &rc);
		w = rc.right;
		h = rc.bottom;
		ch = (INT)(h / 15);

		srand((unsigned)time(NULL));
		hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		hBmp_mem = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONT));
		hdc_mem = CreateCompatibleDC(NULL);
		SelectObject(hdc_mem, hBmp_mem);

		hdc_wnd = GetDC(hWnd);
		hBmp_main = CreateCompatibleBitmap(hdc_wnd, w, h);
		hdc_main = CreateCompatibleDC(NULL);
		SelectObject(hdc_main, hBmp_main);
		ReleaseDC(hWnd, hdc_wnd);

		for(n = 0; n < 36; n++) {
			for(m = 0; m < 3; m++) {
				hBmp_char[n][m] = CreateCompatibleBitmap(hdc_main, 10, 15);
				hdc_char[n][m] = CreateCompatibleDC(NULL);
				SelectObject(hdc_char[n][m], hBmp_char[n][m]);
				BitBlt(hdc_char[n][m], 0, 0, 10, 15, hdc_mem, n*10, m*15, SRCCOPY);
		
			}
		}
		DeleteObject(hBmp_mem);
		DeleteDC(hdc_mem);

		SetTimer(hWnd, ID_TIMER1, 70, NULL);
		SetTimer(hWnd, ID_TIMER2, 35, NULL);
		break;

	case WM_TIMER:
		switch(wp){
		case ID_TIMER1:
			PatBlt(hdc_main, 0, 0, w, h, BLACKNESS);
			for(n = 0; n < MAX_NUM; n ++) {
				if(Place[n].bStart) {	
					Place[n].nY += 15;
					for(m = 0; m < 3; m++) {
						BitBlt(hdc_main, Place[n].nX, Place[n].nY + m * 15, 10, 15,
							hdc_char[rand()%36][0], 0, 0, SRCCOPY);
					}
					for(m = 3; m < 16; m++) {
						BitBlt(hdc_main, Place[n].nX, Place[n].nY + m * 15, 10, 15,
							hdc_char[rand()%36][1], 0, 0, SRCCOPY);
					}
					BitBlt(hdc_main, Place[n].nX, Place[n].nY + 16 * 15, 10, 15,
						hdc_char[rand()%36][2], 0, 0, SRCCOPY);
					if(Place[n].nY > h) {
						Place[n].bStart = FALSE;
					}
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
			
		case ID_TIMER2:
			while(Place[n].bStart) {
				n++;
				if(n > MAX_NUM) {
					break;
				}
			}
			if(n >= MAX_NUM) {
				break;
			}
			Place[n].bStart = TRUE;
			Place[n].nX = (rand() % ch) * 20;
			Place[n].nY = -15 * 16;
			break;
		}
		break;

	case WM_PAINT:
		hdc_wnd = BeginPaint(hWnd, &ps);
		BitBlt(hdc_wnd, 0, 0, w, h, hdc_main, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER1);
		KillTimer(hWnd, ID_TIMER2);

		DeleteObject(hBmp_main);
		DeleteDC(hdc_main);
		for(n = 0; n < 3; n++){
			for(m = 0; m < 36; m++){
				DeleteObject(hBmp_char[n][m]);
				DeleteDC(hdc_char[n][m]);

			}
		}
		PostQuitMessage(0);
        break;
    
	default:
        break;
	}
	return DefScreenSaverProc(hWnd, msg, wp, lp);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND, UINT, WPARAM, LPARAM)
{
	return TRUE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE)
{
    return TRUE;
}