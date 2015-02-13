/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * windowsui.h
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

#ifndef _WINDOWSUI_
#define _WINDOWSUI_

#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")
#include "ui.h"

using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define _PREV_X 10
#define _PREV_Y 200

struct WinData
{
	HINSTANCE hInstance;
	int nCmdShow;
};

struct capture_data
{
    unsigned int x;
    unsigned int y;
	unsigned int** buf;
};

class WindowsUI : public UI
{
public:
	WindowsUI(void* ptr);
	~WindowsUI();

	virtual void run();
	virtual void update();

	virtual void set_status(std::string text);
	virtual void show_progress();
	virtual void hide_progress();
	virtual void pulse_progress();
	virtual void error(std::string text);

	virtual std::string get_port();
	virtual std::string get_speed();
	virtual std::string get_res();
	virtual void set_res(int x, int y);
	virtual unsigned int** get_buffer();
	virtual unsigned int get_size_x();
	virtual unsigned int get_size_y();
	virtual void enable_capture();
	virtual void disable_capture();

	static HWND window;
	static HWND capture_btn;
	static HWND open_btn;
	static HWND save_btn;
	static HWND speed_cb;
	static HWND resolution_cb;
	static HWND port_ed;
	static HWND status;
	static HWND progress;
	static unsigned int** buffer;
	static int _w;
	static int _h;

private:
	ULONG_PTR m_gdiplusToken;
};

#endif