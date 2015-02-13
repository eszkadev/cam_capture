/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * windowsui.cpp
 * Copyright (C) 2015 Szymon K這s <eszkadev@gmail.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Szymon K這s'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * cam_capture IS PROVIDED BY Szymon K這s ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Szymon K這s OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "windowsui.h"

HWND WindowsUI::window = 0;
HWND WindowsUI::capture_btn = 0;
HWND WindowsUI::open_btn = 0;
HWND WindowsUI::save_btn = 0;
HWND WindowsUI::speed_cb = 0;
HWND WindowsUI::resolution_cb = 0;
HWND WindowsUI::port_ed = 0;
HWND WindowsUI::status = 0;
HWND WindowsUI::progress = 0;
unsigned int** WindowsUI::buffer = 0;
int WindowsUI::_w = 0;
int WindowsUI::_h = 0;

capture_data* pDataArray = 0;
DWORD dwThreadIdArray = 0;
HANDLE hThreadArray = 0;

WindowsUI::WindowsUI(void* ptr)
{
	HINSTANCE hInstance = ((WinData*)ptr)->hInstance;
	int nCmdShow = ((WinData*)ptr)->nCmdShow;

	WNDCLASS window_class;
	static char class_name[] = "MainWindow";

	window_class.style         = 0;
	window_class.lpfnWndProc   = WndProc;
 	window_class.cbClsExtra    = 0;
	window_class.cbWndExtra    = 0;
	window_class.hInstance     = hInstance;
	window_class.hIcon         = 0;
	window_class.hCursor       = LoadCursor(0, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	window_class.lpszMenuName  = NULL;
	window_class.lpszClassName = class_name;

    RegisterClass(&window_class);

 	WindowsUI::window = CreateWindow(class_name, "cam_capture", WS_OVERLAPPEDWINDOW | WS_BORDER| WS_VISIBLE,
						CW_USEDEFAULT, CW_USEDEFAULT, 360, 500, NULL, NULL, hInstance, NULL);

	capture_btn = CreateWindowEx(NULL, "Button", "Capture", WS_DISABLED | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                10, 115, 155, 35, window, NULL, hInstance, NULL);
	save_btn = CreateWindowEx(NULL, "Button", "Save", WS_DISABLED | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                175, 115, 155, 35, window, NULL, hInstance, NULL);
	open_btn = CreateWindowEx(NULL, "Button", "Open port", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                210, 10, 120, 45, window, NULL, hInstance, NULL);


	speed_cb = CreateWindowEx(NULL, "ComboBox", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				100, 35, 100, 100, window, NULL, hInstance, NULL);
	ComboBox_AddString(speed_cb, "9600");
	ComboBox_AddString(speed_cb, "57600");
	ComboBox_AddString(speed_cb, "115200");
	SendMessage(speed_cb, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
	CreateWindowEx(NULL, "STATIC", "Baud: ", WS_CHILD | WS_VISIBLE | SS_CENTER,
				10, 38, 80, 20, window, NULL, NULL, NULL);

	resolution_cb = CreateWindowEx(NULL, "ComboBox", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				100, 70, 230, 100, window, NULL, hInstance, NULL);
	ComboBox_AddString(resolution_cb, r120);
	ComboBox_AddString(resolution_cb, r240);
	SendMessage(resolution_cb, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateWindowEx(NULL, "STATIC", "Resolution: ", WS_CHILD | WS_VISIBLE | SS_CENTER,
				10, 73, 80, 20, window, NULL, NULL, NULL);

	port_ed = CreateWindowEx(NULL, "Edit", "COM3", WS_CHILD | WS_VISIBLE | ES_LEFT,
				100, 10, 100, 20, window, NULL, hInstance, NULL);
	CreateWindowEx(NULL, "STATIC", "Port: ", WS_CHILD | WS_VISIBLE | SS_CENTER,
				10, 10, 80, 20, window, NULL, NULL, NULL);

	status = CreateWindowEx(NULL, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_LEFT,
				10, 160, 200, 20, window, NULL, NULL, NULL);
	progress = CreateWindowEx(NULL, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE | SS_LEFT,
				230, 160, 100, 20, window, NULL, NULL, NULL);
	SendMessage(progress, PBM_SETSTEP, (WPARAM) 1, 0);

	set_res(160, 120);
}

DWORD WINAPI capture( LPVOID lpParam )
{
	capture_image(((capture_data*)lpParam)->x, ((capture_data*)lpParam)->y, ((capture_data*)lpParam)->buf);
	SendMessage(WindowsUI::progress, PBM_SETPOS, 0, 0);
	return  0;
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_COMMAND:
		{
			if(wParam == BN_CLICKED)
			{
				if((HWND)lParam == WindowsUI::capture_btn)
				{
					pDataArray = (capture_data*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(capture_data));
					pDataArray->x = ui->get_size_x();
					pDataArray->y = ui->get_size_y();
					pDataArray->buf = ui->get_buffer();

					hThreadArray = CreateThread(NULL, 0, capture, pDataArray, 0, &dwThreadIdArray);
				}
				else if((HWND)lParam == WindowsUI::save_btn)
				{
					OPENFILENAME ofn;
					char szFileName[MAX_PATH] = "";

					ZeroMemory(&ofn, sizeof(ofn));

					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFilter = (LPCSTR)"BMP Files (*.bmp)\0*.bmp\0\0";
					ofn.lpstrFile = (LPSTR)szFileName;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = (LPCSTR)"bmp";

					if(GetSaveFileName(&ofn) != 0)
					{
						std::string filename = ofn.lpstrFile;

						save_image(filename, ui->get_size_x(), ui->get_size_y(), ui->get_buffer());

						filename = "Saved as '" + filename + "'";
						MessageBox(hWnd, filename.c_str(), "Info", MB_OK);
					}
				}
				else if((HWND)lParam == WindowsUI::open_btn)
				{
					open_serial_port();
					if(serial->is_open())
					{
						Button_Enable(WindowsUI::capture_btn, true);
						Button_Enable(WindowsUI::save_btn, true);
					}
				}
			}
			else if(HIWORD(wParam) == CBN_SELCHANGE)
			{
				if((HWND)lParam == WindowsUI::resolution_cb)
					change_resolution();
			}
			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC hdc;
			int startx = _PREV_X;
			int starty = _PREV_Y;
			int width = ui->get_size_x();
			int height = ui->get_size_y();
			unsigned int** img = ui->get_buffer();

			hdc = BeginPaint(hWnd, &paint);

			// drawing image from buffer
			for(int y = starty; y < starty + height; y++)
			{
				for(int x = startx; x < startx + width; x++)
				{
					int color = img[x-startx][y-starty];
					HPEN pioro = CreatePen(PS_SOLID, 1, RGB(((color >> 11) << 3), (((color & 2016) >> 5) << 2), ((color & 31) << 3)));
					SelectObject(hdc, pioro);

					MoveToEx(hdc, x, y, NULL);
					LineTo(hdc, x+1, y);

					DeleteObject(pioro);
				}
			}

			return 0;
		}
  	
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
    
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

WindowsUI::~WindowsUI()
{
	GdiplusShutdown(m_gdiplusToken);
}

void WindowsUI::run()
{
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CloseHandle(hThreadArray);
	HeapFree(GetProcessHeap(), 0, pDataArray);
}

void WindowsUI::update()
{
	// this function is called afther the capture of every line
	int width = ui->get_size_x();
	int height = ui->get_size_y();
	static int line = 0;

	static RECT r;
	r.left = _PREV_X;
	r.right = _PREV_X + width;
	r.top = _PREV_Y + line++;
	r.bottom = _PREV_Y + line;

	InvalidateRect(WindowsUI::window, &r, FALSE);
	if(line == height)
		line = 0;
}

void WindowsUI::set_status(std::string text)
{
	Static_SetText(status, text.c_str());
}

void WindowsUI::show_progress()
{
}

void WindowsUI::hide_progress()
{
}

void WindowsUI::pulse_progress()
{
	SendMessage(progress, PBM_STEPIT, 0, 0);
}

void WindowsUI::error(std::string text)
{
	MessageBox(WindowsUI::window, text.c_str(), "Error", MB_OK);
}

std::string WindowsUI::get_port()
{
#define _BUF_SIZE 64
	char buf[_BUF_SIZE];
	memset(buf, 0, _BUF_SIZE);
	Edit_GetLine(port_ed, 0, buf, _BUF_SIZE);
	return std::string(buf);
#undef _BUF_SIZE
}

std::string WindowsUI::get_speed()
{
#define _BUF_SIZE 64
	char buf[_BUF_SIZE];
	memset(buf, 0, _BUF_SIZE);
	ComboBox_GetText(speed_cb, buf, _BUF_SIZE);
	return std::string(buf);
#undef _BUF_SIZE
}

std::string WindowsUI::get_res()
{
#define _BUF_SIZE 64
	char buf[_BUF_SIZE];
	memset(buf, 0, _BUF_SIZE);
	ComboBox_GetText(resolution_cb, buf, _BUF_SIZE);
	return std::string(buf);
#undef _BUF_SIZE
}

void WindowsUI::set_res(int x, int y)
{
	if(buffer)
		delete[] buffer;

	_w = x;
	_h = y;

	buffer = new unsigned int*[x];
	for(int i = 0; i < x; i++)
	{
		buffer[i] = new unsigned int[y];
		for(int j = 0; j < y; j++)
			buffer[i][j] = 0;
	}

	SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, _h));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	InvalidateRect(window, NULL, TRUE);
}

unsigned int** WindowsUI::get_buffer()
{
	return buffer;
}

unsigned int WindowsUI::get_size_x()
{
	return _w;
}

unsigned int WindowsUI::get_size_y()
{
	return _h;
}

void WindowsUI::enable_capture()
{
	Button_Enable(WindowsUI::capture_btn, true);
	Button_Enable(WindowsUI::save_btn, true);
	Button_Enable(WindowsUI::open_btn, true);
	ComboBox_Enable(WindowsUI::resolution_cb, true);
}

void WindowsUI::disable_capture()
{
	Button_Enable(WindowsUI::capture_btn, false);
	Button_Enable(WindowsUI::save_btn, false);
	Button_Enable(WindowsUI::open_btn, false);
	ComboBox_Enable(WindowsUI::resolution_cb, false);
}