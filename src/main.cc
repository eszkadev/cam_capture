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

#include "preview.h"
#include "config.h"
#include "serial_port.h"
#include "EasyBMP/EasyBMP.h"

#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <gtkmm/main.h>


/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/ui/cam_capture.ui" */
#define UI_FILE "src/cam_capture.ui"
#define PORT_NOT_OPEN -1

int size_x = 80;
int size_y = 60;
int port = PORT_NOT_OPEN;
std::string port_path;

Gtk::Main* kit = 0;
Gtk::Window* main_win = 0;
Gtk::Button* capture_button = 0;
Gtk::Button* save_button = 0;
Gtk::Button* open_button = 0;
Gtk::Label* status = 0;
Gtk::Entry* port_edit = 0;
Gtk::Spinner* spinner = 0;
Preview* preview = 0;
Gtk::Box* container = 0;
Gtk::ProgressBar* progress = 0;
std::thread* capture_thread = 0, *gtk_thread = 0;

void open_serial_port()
{
	std::string path = port_edit->get_text();
	if(port_path != path)
	{
		if(port != PORT_NOT_OPEN)
			close(port);
		
		port = open_port(path.c_str());

		if(port >= 0)
		{
			setup_port(port, 57600);
			port_path = path;
			save_button->set_sensitive(true);
			capture_button->set_sensitive(true);
		}
		else
		{
			Gtk::MessageDialog error_dialog("Cannot open this port");
			error_dialog.run();
			port = PORT_NOT_OPEN;
		}
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
		Output.SetSize(size_x, size_y);
		Output.SetBitDepth(24);

		unsigned char r,g,b;

		for(int y = 0; y < size_y; y++)
		{
			for(int x = 0; x < size_x; x++)
			{
				unsigned int color = preview->get_buffer()[x][y];

				r = (unsigned char)((color >> 11) << 3);
				g = (unsigned char)(((color & 2016) >> 5) << 2);
				b = (unsigned char)((color & 31) << 3);

				if(r > 255)
					r = 255;
				if(g > 255)
					g = 255;
				if(b > 255)
					b = 255;

				Output(x,y)->Red = r;
				Output(x,y)->Green = g;
				Output(x,y)->Blue = b;
				Output(x,y)->Alpha = 0;
			}
		}

		Output.WriteToFile(filename.c_str());
	}
}

void capture()
{
	capture_image(port, size_x, size_y, preview->get_buffer());
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
	
	builder->get_widget("progress", progress);
	progress->set_pulse_step(0.1);
	progress->hide();
	
	preview = new Preview(size_x, size_y);
	container->pack_start((Gtk::Widget&)*preview, true, true);
	preview->set_size_request(160, 120);
	preview->show();

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

