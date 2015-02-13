## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(CAM_CAPTURE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = cam_capture

cam_capture_SOURCES = \
	../cam_capture/main.cc \
	../cam_capture/preview.h \
	../cam_capture/serial.cpp \
	../cam_capture/serial.h \
	../cam_capture/ui.cpp \
	../cam_capture/ui.h \
	../cam_capture/EasyBMP/EasyBMP.cpp \
	../cam_capture/EasyBMP/EasyBMP.h \
	../cam_capture/EasyBMP/EasyBMP_BMP.h \
	../cam_capture/EasyBMP/EasyBMP_DataStructures.h \
	../cam_capture/EasyBMP/EasyBMP_VariousBMPutilities.h \
	../cam_capture/cam_capture.h \
	../cam_capture/linuxui.cpp \
	../cam_capture/linuxui.h \
	../cam_capture/cam_capture.cpp

cam_capture_CXXFLAGS = -std=c++11 

cam_capture_LDFLAGS = 

cam_capture_LDADD = $(CAM_CAPTURE_LIBS)


if NATIVE_WIN32
cam_capture_LDFLAGS += -mwindows
endif



EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

