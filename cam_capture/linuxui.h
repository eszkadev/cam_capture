/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * linuxui.h
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

#ifndef _LINUXUI_
#define _LINUXUI_

#include <string>
#include <iostream>
#include "serial.h"
#include "cam_capture.h"
#include "ui.h"


#include "preview.h"
#include <gtkmm.h>
#include <gtkmm/main.h>
#include <thread>

/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/ui/cam_capture.ui" */
#define UI_FILE "cam_capture/cam_capture.ui"

class LinuxUI : public UI
{
public:
	LinuxUI(void* ptr);
	~LinuxUI();

	virtual void run();
	virtual void update();

	virtual void set_status(std::string text);
	virtual void show_progress();
	virtual void hide_progress();
	virtual void pulse_progress();
	virtual void error(std::string text);

	virtual std::string get_port() { return port_edit->get_text(); };
	virtual std::string get_speed() { return speed_cb->get_active_text(); }
	virtual std::string get_res() { return resolution_cb->get_active_text(); }
	virtual void set_res(int x, int y) { preview->change_res(x, y); }
	virtual unsigned int** get_buffer() { return preview->get_buffer(); };
	virtual unsigned int get_size_x() { return preview->get_size_x(); };
	virtual unsigned int get_size_y() { return preview->get_size_y(); };
	virtual void enable_capture();
	virtual void disable_capture();

	static void capture_thread_start();
	static void save();

private:
	static Gtk::Button* capture_button;
	static Gtk::Button* save_button;
	static Gtk::Button* open_button;
	Gtk::Label* status;
	Gtk::Entry* port_edit;
	static Gtk::ComboBoxText* speed_cb;
	static Gtk::ComboBoxText* resolution_cb;
	Gtk::Spinner* spinner;
	static Preview* preview;
	Gtk::Box* container;
	Gtk::ProgressBar* progress;

	static std::thread* capture_thread;
	Gtk::Main* kit;
	static Gtk::Window* main_win;
	
};


#endif
