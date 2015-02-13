/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * cam_capture.h
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

#include "cam_capture.h"

void save_image(std::string filename, unsigned int size_x, unsigned int size_y, unsigned int** imgage)
{
	BMP Output;
	Output.SetSize(size_x*2, size_y*2);
	Output.SetBitDepth(24);

	unsigned char r,g,b;

	for(unsigned int y = 0; y < size_y*2; y += 2)
	{
		for(unsigned int x = 0; x < size_x*2; x += 2)
		{
			unsigned int color = imgage[x/2][y/2];

			r = (unsigned char)((color >> 11) << 3);
			g = (unsigned char)(((color & 2016) >> 5) << 2);
			b = (unsigned char)((color & 31) << 3);

			if(r > 255)
				r = 255;
			if(g > 255)
				g = 255;
			if(b > 255)
				b = 255;

			Output(x, y)->Red = r;
			Output(x, y)->Green = g;
			Output(x, y)->Blue = b;
			Output(x, y)->Alpha = 0;

			Output(x, y + 1)->Red = r;
			Output(x, y + 1)->Green = g;
			Output(x, y + 1)->Blue = b;
			Output(x, y + 1)->Alpha = 0;

			Output(x + 1, y + 1)->Red = r;
			Output(x + 1, y + 1)->Green = g;
			Output(x + 1, y + 1)->Blue = b;
			Output(x + 1, y + 1)->Alpha = 0;

			Output(x + 1, y)->Red = r;
			Output(x + 1, y)->Green = g;
			Output(x + 1, y)->Blue = b;
			Output(x + 1, y)->Alpha = 0;
		}
	}

	Output.WriteToFile(filename.c_str());
}

void capture_image(unsigned int size_x, unsigned int size_y, unsigned int** image)
{
	unsigned int buf;
	const char start[] = "START";
	unsigned int i = 0;

	if(ui)
		ui->disable_capture();

	std::cout << "Waiting for VSYNC..." << std::endl;
	if(ui)
		ui->set_status("Waiting for VSYNC...");

	serial->flush();

	do
	{
		buf = serial->read_byte();
		if(start[i] == buf)
			i++;
		else
			i = 0;
	}
	while(i < strlen(start));

	std::cout << "Loading data..." << std::endl;
	if(ui)
	{
		ui->set_status("Loading data...");
		ui->show_progress();
	}

	for(unsigned int y = 0; y < size_y; ++y)
	{
		for(unsigned int x = 0; x < size_x; ++x)
		{
			buf = serial->read_byte();
			image[x][y] = buf << 8;
			buf = serial->read_byte();
			image[x][y] += buf;
		}

		std::stringstream ss;
		ss << y*100/size_y << "%";
		std::cout << ss.str() << std::endl;

		if(ui)
		{
			ui->pulse_progress();
			ui->set_status(ss.str());
			ui->update();
		}
	}

	if(ui)
	{
		ui->update();
		ui->set_status("");
		ui->hide_progress();
		ui->enable_capture();
	}
}
