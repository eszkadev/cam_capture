/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * preview.h
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

#ifndef _PREVIEW_
#define _PREVIEW_

#include <gtkmm.h>

class Preview : public Gtk::DrawingArea
{
public:

	Preview(unsigned int x, unsigned int y) : Gtk::DrawingArea()
	{
		_size_x = x;
		_size_y = y;

		_image = (unsigned int**)malloc(sizeof(unsigned int*) * (_size_x));
		for(int i = 0; i < _size_x; i++)
		{
			_image[i] = (unsigned int*)malloc(sizeof(unsigned int) * (_size_y));
			for(int j = 0; j < _size_y; j++)
				_image[i][j] = 0;
		}

		set_size_request(_size_x*2, _size_y*2);
	};

	unsigned int** get_buffer()
	{
		return _image;
	}

	unsigned int get_size_x()
	{
		return _size_x;
	}

	unsigned int get_size_y()
	{
		return _size_y;
	}

	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
	{
		unsigned int x, y;
		unsigned int color;
		unsigned char r, g, b;

		for(y = 0; y < _size_y*2; y+=2)
		{
			for(x = 0; x < _size_x*2; x+=2)
			{
				color = _image[x/2][y/2];
	
				r = (unsigned char)((color >> 11) << 3);
				g = (unsigned char)(((color & 2016) >> 5) << 2);
				b = (unsigned char)((color & 31) << 3);

				if(r > 255)
					r = 255;
				if(g > 255)
					g = 255;
				if(b > 255)
					b = 255;

				cr->set_source_rgb((gdouble)r/255, (gdouble)g/255, (gdouble)b/255);
				cr->rectangle(x, y, 2, 2);
				cr->fill();
			}
		}

		cr->stroke();

		return FALSE;
	}

	void change_res(unsigned int w, unsigned int h)
	{
		_size_x = w;
		_size_y = h;

		delete[] _image;

		_image = (unsigned int**)malloc(sizeof(unsigned int*) * (_size_x));
		for(int i = 0; i < _size_x; i++)
		{
			_image[i] = (unsigned int*)malloc(sizeof(unsigned int) * (_size_y));
			for(int j = 0; j < _size_y; j++)
				_image[i][j] = 0;
		}

		set_size_request(_size_x*2, _size_y*2);
	}

private:
	 unsigned int** _image;
	 unsigned int _size_x;
	 unsigned int _size_y;
};

#endif