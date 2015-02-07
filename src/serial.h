/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * serial.h
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

#ifdef __linux__
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <termios.h>
	#include <string.h>
	#include <stdlib.h>
#endif

class Serial
{
public:
	Serial() {};
	~Serial() {};

	virtual void open_port(const char* port_name) = 0;
	virtual void set_baud(int baud) = 0;
	virtual unsigned char read_byte() = 0;
	virtual bool is_open() = 0;
	virtual void close() = 0;
	virtual void flush() = 0;
};

class WindowsSerialImpl : public Serial
{
public:
	WindowsSerialImpl();
	~WindowsSerialImpl();

	virtual void open_port(const char* port_name);
	virtual void set_baud(int baud);
	virtual unsigned char read_byte();
	virtual bool is_open();
	virtual void close();
	virtual void flush();
};

#ifdef __linux__
class LinuxSerialImpl : public Serial
{
public:
	LinuxSerialImpl();
	~LinuxSerialImpl();

	virtual void open_port(const char* port_name);
	virtual void set_baud(int baud);
	virtual unsigned char read_byte();
	virtual bool is_open();
	virtual void close();
	virtual void flush();

private:
	int _descriptor;
	int _baud;
	const char* _port_name;
};
#endif