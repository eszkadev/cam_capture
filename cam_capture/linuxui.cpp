/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * linuxui.cpp
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

#include "linuxui.h"

std::thread* LinuxUI::capture_thread = 0;
Gtk::Window* LinuxUI::main_win = 0;
Preview* LinuxUI::preview = 0;
Gtk::Button* LinuxUI::capture_button = 0;
Gtk::Button* LinuxUI::save_button = 0;
Gtk::Button* LinuxUI::open_button = 0;
Gtk::ComboBoxText* LinuxUI::speed_cb = 0;
Gtk::ComboBoxText* LinuxUI::resolution_cb = 0;

void LinuxUI::capture_thread_start()
{
	capture_thread = new std::thread(capture_image, ui->get_size_x(), ui->get_size_y(), ui->get_buffer());
}

void LinuxUI::save()
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
		save_image(save_dialog.get_filename(), ui->get_size_x(), ui->get_size_y(), ui->get_buffer());
	}
}

LinuxUI::LinuxUI(void* ptr)
{
	kit = new Gtk::Main(NULL, NULL/*argc, argv*/);

	//Load the Glade file and instiate its widgets:
	Glib::RefPtr<Gtk::Builder> builder;
	try
	{
		builder = Gtk::Builder::create_from_file(UI_FILE);
	}
	catch (const Glib::FileError & ex)
	{
		std::cerr << ex.what() << std::endl;
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
	builder->get_widget("resolution", resolution_cb);

	speed_cb->append("9600");
	speed_cb->append("57600");
	speed_cb->append("115200");
	speed_cb->set_active_text("115200");

	resolution_cb->append(r120);
	resolution_cb->append(r240);
	resolution_cb->set_active_text(r120);
	resolution_cb->signal_changed().connect(sigc::ptr_fun(change_resolution));
	
	builder->get_widget("progress", progress);
	progress->set_pulse_step(0.1);
	progress->hide();
	
	preview = new Preview(160, 120);
	container->pack_start((Gtk::Widget&)*preview, true, true);
	preview->show();

	capture_button->set_sensitive(false);
	capture_button->signal_clicked().connect(sigc::ptr_fun(&LinuxUI::capture_thread_start));
	
	save_button->set_sensitive(false);
	save_button->signal_clicked().connect(sigc::ptr_fun(&LinuxUI::save));
	
	open_button->signal_clicked().connect(sigc::ptr_fun(open_serial_port));
}

LinuxUI::~LinuxUI()
{
}

void LinuxUI::run()
{
	if(main_win)
	{
		kit->run(*main_win);
		if(capture_thread && capture_thread->joinable())
			capture_thread->join();
	}
}

void LinuxUI::update()
{
	preview->signal_draw();
	main_win->queue_draw();
}

void LinuxUI::set_status(std::string text)
{
	status->set_text(text);
}

void LinuxUI::show_progress()
{
	progress->show();
}

void LinuxUI::hide_progress()
{
	progress->hide();
}

void LinuxUI::pulse_progress()
{
	progress->pulse();
}

void LinuxUI::error(std::string text)
{
	Gtk::MessageDialog error_dialog(text);
	error_dialog.run();
}

void LinuxUI::enable_capture()
{
	capture_button->set_sensitive(true);
	save_button->set_sensitive(true);
	speed_cb->set_sensitive(true);
	resolution_cb->set_sensitive(true);
	open_button->set_sensitive(true);
}

void LinuxUI::disable_capture()
{
	capture_button->set_sensitive(false);
	save_button->set_sensitive(false);
	speed_cb->set_sensitive(false);
	resolution_cb->set_sensitive(false);
	open_button->set_sensitive(false);
}