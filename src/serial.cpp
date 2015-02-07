/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * serial.cpp
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

#include "serial.h"

WindowsSerialImpl::WindowsSerialImpl()
{
}

WindowsSerialImpl::~WindowsSerialImpl()
{
}

void WindowsSerialImpl::open_port(const char* port_name)
{
}

void WindowsSerialImpl::set_baud(int baud)
{
}

unsigned char WindowsSerialImpl::read_byte()
{
	unsigned char ret = 0;
	return ret;
}

bool WindowsSerialImpl::is_open()
{
	return false;
}

void WindowsSerialImpl::close()
{
}

void WindowsSerialImpl::flush()
{
}

#ifdef __linux__
LinuxSerialImpl::LinuxSerialImpl()
{
}

LinuxSerialImpl::~LinuxSerialImpl()
{
}

void LinuxSerialImpl::open_port(const char* port_name)
{
	_descriptor = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (_descriptor == -1)
	{
		// error
	}else
	{
		_port_name = (const char*) malloc(sizeof(const char) * (strlen(port_name) + 1));
		_port_name = port_name;
		fcntl(_descriptor, F_SETFL, 0);
	}
}

void LinuxSerialImpl::set_baud(int baud)
{
	_baud = baud;

	struct termios specs;
	tcgetattr(_descriptor, &specs);
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
			_baud = 9600;
			cfsetospeed(&specs,B9600);
			break;
	}

	tcsetattr(_descriptor, TCSANOW, &specs);
}

unsigned char LinuxSerialImpl::read_byte()
{
	unsigned char ret;

	read(_descriptor, &ret, 1);

	return ret;
}

bool LinuxSerialImpl::is_open()
{
	if(_descriptor != -1)
		return true;

	return false;
}

void LinuxSerialImpl::close()
{
}

void LinuxSerialImpl::flush()
{
	tcflush(_descriptor, TCIOFLUSH);
}

#endif
