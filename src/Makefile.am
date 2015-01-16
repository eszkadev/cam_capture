## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = cam_capture.ui


AM_CPPFLAGS = \
	-std=c++11 \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(CAM_CAPTURE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = cam_capture

cam_capture_SOURCES = \
	main.cc \
	serial_port.cpp \
	serial_port.h \
	preview.h \
	EasyBMP/EasyBMP.cpp \
	EasyBMP/EasyBMP.h \
	EasyBMP/EasyBMP_BMP.h \
	EasyBMP/EasyBMP_DataStructures.h \
	EasyBMP/EasyBMP_VariousBMPutilities.h

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

