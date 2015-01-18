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

#include "preview.h"
#include "config.h"
#include "serial.h"
#include "EasyBMP/EasyBMP.h"

#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <gtkmm/main.h>


/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/ui/cam_capture.ui" */
#define UI_FILE "src/cam_capture.ui"

int size_x = 160;
int size_y = 120;
std::string port_path;

Gtk::Main* kit = 0;
Gtk::Window* main_win = 0;
Gtk::Button* capture_button = 0;
Gtk::Button* save_button = 0;
Gtk::Button* open_button = 0;
Gtk::Label* status = 0;
Gtk::Entry* port_edit = 0;
Gtk::ComboBoxText* speed_cb = 0;
Gtk::Spinner* spinner = 0;
Preview* preview = 0;
Gtk::Box* container = 0;
Gtk::ProgressBar* progress = 0;

std::thread* capture_thread = 0, *gtk_thread = 0;
Serial* serial = 0;

void open_serial_port()
{
	std::string path = port_edit->get_text();

	if(serial->is_open())
		serial->close();
		
	serial->open_port(path.c_str());

	if(serial->is_open())
	{
		serial->set_baud(atoi(speed_cb->get_active_text().c_str()));
		port_path = path;
		save_button->set_sensitive(true);
		capture_button->set_sensitive(true);
	}
	else
	{
		Gtk::MessageDialog error_dialog("Cannot open this port");
		error_dialog.run();
	}
}

void save_image()
{
	Gtk::FileChooserDialog save_dialog("Save image", Gtk::FILE_CHOOSER_ACTION_SAVE);
	Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
	filter->set_name("*.bmp");
	filter->add_mime_type("image/bmp");
	save_dialog.add_filter(filter);
	save_dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	save_dialog.add_button("_Save", Gtk::RESPONSE_OK);

	int result = save_dialog.run();

	if(result == Gtk::RESPONSE_OK)
	{
		std::string filename = save_dialog.get_filename();

		BMP Output;
		Output.SetSize(size_x*2, size_y*2);
		Output.SetBitDepth(24);

		unsigned char r,g,b;

		for(int y = 0; y < size_y*2; y += 2)
		{
			for(int x = 0; x < size_x*2; x += 2)
			{
				unsigned int color = preview->get_buffer()[x/2][y/2];

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
}

void capture_image(int size_x, int size_y, unsigned int** image)
{
	unsigned int buf;
	const char start[] = "START";
	unsigned int i = 0;
	unsigned int n, x, y;
	unsigned char tmp[128];

	status->set_text("Waiting for VSYNC...");

	spinner->show();
	spinner->start();

	do
	{
		buf = serial->read_byte();
		if(start[i] == buf)
			i++;
		else
			i = 0;
	}
	while(i < strlen(start));

	status->set_text("Loading data...");

	spinner->stop();
	spinner->hide();
	progress->show();

	for(y = 0; y < size_y; ++y)
	{
		for(x = 0; x < size_x; ++x)
		{
			buf = serial->read_byte();
			image[x][y] = buf << 8;
			buf = serial->read_byte();
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
}

void capture()
{
	capture_image(size_x, size_y, preview->get_buffer());
	preview->signal_draw();
	main_win->queue_draw();
	capture_button->set_sensitive(true);
	save_button->set_sensitive(true);
}

void capture_thread_start()
{
	capture_button->set_sensitive(false);
	save_button->set_sensitive(false);
	capture_thread = new std::thread(capture);
}

void gtk_thread_start()
{
	kit->run(*main_win);
}

int main(int argc, char *argv[])
{
	kit = new Gtk::Main(argc, argv);

	//Load the Glade file and instiate its widgets:
	Glib::RefPtr<Gtk::Builder> builder;
	try
	{
		builder = Gtk::Builder::create_from_file(UI_FILE);
	}
	catch (const Glib::FileError & ex)
	{
		std::cerr << ex.what() << std::endl;
		return 1;
	}

	builder->get_widget("main_window", main_win);
	builder->get_widget("capture_button", capture_button);
	builder->get_widget("save_button", save_button);
	builder->get_widget("open_button", open_button);
	builder->get_widget("container", container);
	builder->get_widget("status", status);
	builder->get_widget("spinner", spinner);
	builder->get_widget("port", port_edit);
	builder->get_widget("speed", speed_cb);

	speed_cb->append("9600");
	speed_cb->append("57600");
	speed_cb->append("115200");
	speed_cb->append("460800");
	speed_cb->set_active_text("57600");
	
	builder->get_widget("progress", progress);
	progress->set_pulse_step(0.1);
	progress->hide();
	
	preview = new Preview(size_x, size_y);
	container->pack_start((Gtk::Widget&)*preview, true, true);
	preview->set_size_request(size_x*2, size_y*2);
	preview->show();

#ifdef __linux__
	serial = new LinuxSerialImpl();
#else
	serial = new WindowsSerialImpl();
#endif

	capture_button->set_sensitive(false);
	capture_button->signal_clicked().connect(sigc::ptr_fun(capture_thread_start));
	
	save_button->set_sensitive(false);
	save_button->signal_clicked().connect(sigc::ptr_fun(save_image));
	
	open_button->signal_clicked().connect(sigc::ptr_fun(open_serial_port));

	if(main_win)
	{
		gtk_thread = new std::thread(gtk_thread_start);
		gtk_thread->join();
		capture_thread->join();
	}
	
	return 0;
}

