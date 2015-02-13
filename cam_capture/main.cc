/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
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

#define _WITH_GUI // To use console mode comment this line

#include <iostream>

#include "cam_capture.h"

#include "serial.h"
Serial* serial = 0;

#include "ui.h"
UI* ui = 0;


#ifdef _WIN32
#ifdef _WITH_GUI
#define _WIN32_GUI
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	serial = new WindowsSerialImpl();

	WinData data;
	data.hInstance = hInstance;
	data.nCmdShow = nCmdShow;
	ui = new WindowsUI(&data);
	ui->run();

	return 0;
}
#endif
#endif

#ifndef _WIN32_GUI
int main(int argc, char *argv[])
{
	#ifdef __linux__
		std::string serial_port = "/dev/ttyUSB0";
	#else
		std::string serial_port = "COM3";
		
	#endif

	std::string output_filename = "out.bmp";
	std::string baudrate = "115200";
	std::string resolution = "160x120";

	#ifdef __linux__
		serial = new LinuxSerialImpl();
	#else
		serial = new WindowsSerialImpl();
	#endif

	#ifdef _WITH_GUI

		#ifdef __linux__
			ui = new LinuxUI(NULL);
			std::thread* tg = new std::thread(&UI::run, std::ref(ui));
			tg->join();
		#endif

	#else
		// console mode
		unsigned int size_x, size_y;

		if(serial->is_open())
			serial->close();

		try
		{
			serial->open_port(serial_port.c_str());
		}
		catch(...) {}

		if(serial->is_open())
		{
			serial->set_baud(atoi(baudrate.c_str()));
		}
		else
		{
			std::cerr << "Cannot open this port" << std::endl;
			return 0;
		}

		if(resolution == r120)
		{
			size_x = 160;
			size_y = 120;
		}
		else if(resolution == r240)
		{
			size_x = 320;
			size_y = 240;
		}
		else
		{
			std::cerr << "Unsupported resolution" << std::endl;
			return 0;
		}

		unsigned int** img = new unsigned int*[size_x];
		for(int i = 0; i < size_x; i++)
		{
			img[i] = new unsigned int[size_y];
			for(int j = 0; j < size_y; j++)
				img[i][j] = 0;
		}

		capture_image(size_x, size_y, img);
		save_image(output_filename, size_x, size_y, img);
	#endif
	
	return 0;
}

#endif