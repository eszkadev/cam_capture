/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ui.h
 * Copyright (C) 2015 Szymon Kłos <eszkadev@gmail.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Szymon Kłos'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * cam_capture IS PROVIDED BY Szymon Kłos ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Szymon Kłos OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UI_
#define _UI_

#include <string>
#include <iostream>
#include "cam_capture.h"
#include "serial.h"

class UI
{
public:
	UI() {};
	~UI() {};

	virtual void run() = 0;
	virtual void update() = 0;

	virtual void set_status(std::string text) = 0;
	virtual void show_progress() = 0;
	virtual void hide_progress() = 0;
	virtual void pulse_progress() = 0;
	virtual void error(std::string text) = 0;

	virtual std::string get_port() = 0;
	virtual std::string get_speed() = 0;
	virtual std::string get_res() = 0;
	virtual void set_res(int x, int y) = 0;
	virtual unsigned int** get_buffer() = 0;
	virtual unsigned int get_size_x() = 0;
	virtual unsigned int get_size_y() = 0;
	virtual void enable_capture() = 0;
	virtual void disable_capture() = 0;
};

extern UI* ui;
void change_resolution();
void open_serial_port();

#ifdef __linux__
	#include "linuxui.h"
#else
	#include "windowsui.h"
#endif


#endif