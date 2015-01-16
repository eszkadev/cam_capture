/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * serial_port.cpp
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
 * 3. Neither the name ``Szymon K??os'' nor the name of any other
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

#include "serial_port.h"

extern Gtk::Window* main_win;
extern Gtk::ProgressBar* progress;
extern Gtk::Label* status;
extern Gtk::Spinner* spinner;
extern Preview* preview;

int open_port(const char* device)
{
	int port;

	port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (port == -1)
	{
		char error[256];
		snprintf(error, 256, "Unable to open device: '%s' - ", device);
		perror(error);
	}else
	{
		fcntl(port, F_SETFL, 0);
	}

	return port;
}

int setup_port(int port, int baud)
{
	struct termios specs;
	tcgetattr(port, &specs);
	specs.c_cflag = (CLOCAL | CREAD );
	specs.c_oflag = (OPOST | CR3);

	switch(baud)
	{
		case 9600:
			cfsetospeed(&specs,B9600);
			break;

		case 57600:
			cfsetospeed(&specs,B57600);
			break;

		case 115200:
			cfsetospeed(&specs,B115200);
			break;

		case 460800:
			cfsetospeed(&specs,B460800);
			break;

		default:
			cfsetospeed(&specs,B9600);
			break;
	}

	tcsetattr(port,TCSANOW,&specs);

	return 1;
}

int capture_image(int port, int size_x, int size_y, unsigned int** image)
{
	char buf;
	char start[] = "START";
	int i = 0;
	int n, x, y;
	char tmp[128];

	status->set_text("Waiting for VSYNC...");
	
	spinner->show();
	spinner->start();
	
	do
	{
		n = read(port, &buf, 1);
		if(start[i] == buf)
			i++;
		else
			i = 0;
	}
	while(n && i < strlen(start));

	status->set_text("Loading data...");

	spinner->stop();
	spinner->hide();
	progress->show();

	for(y = 0; y < size_y; ++y)
	{
		for(x = 0; x < size_x; ++x)
		{
			n = read(port, &buf, 1);
			image[x][y] = buf << 8;
			n = read(port, &buf, 1);
			image[x][y] += buf;
		}

		progress->pulse();
		main_win->queue_draw();
		
		std::stringstream ss;
		ss << y*100/size_y << "%";
		
		status->set_text(ss.str());
	}

	status->set_text("");
	progress->hide();

	return 1;
}
