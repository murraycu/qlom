include $(top_srcdir)/build-aux/autotroll.mk

ACLOCAL_AMFLAGS = -I m4
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal

bin_PROGRAMS = src/qlom
src_qlom_SOURCES = src/qlom.cc \
		   src/table.cc \
		   src/table.h \
		   src/relationship.cc \
		   src/relationship.h \
		   src/error_reporter.cc \
		   src/error_reporter.moc.cc \
		   src/error_reporter.h \
		   src/error.cc \
		   src/error.h \
                   src/gui/main_window.cc \
                   src/gui/main_window.moc.cc \
                   src/gui/main_window.h \
                   src/tables_model.cc \
                   src/tables_model.moc.cc \
                   src/tables_model.h \
                   src/connection_dialog.cc \
                   src/connection_dialog.moc.cc \
                   src/connection_dialog.h \
                   src/list_layout_model.cc \
                   src/list_layout_model.moc.cc \
                   src/list_layout_model.h \
                   src/layout_delegates.cc \
                   src/layout_delegates.moc.cc \
                   src/layout_delegates.h \
		   src/document.cc \
		   src/document.moc.cc \
		   src/document.h \
                   src/utils.cc \
                   src/utils.h

qlom_includes = -I$(top_builddir) -I$(top_srcdir)/src

src_qlom_CXXFLAGS = $(qlom_includes) $(QT_CXXFLAGS) $(QLOM_CFLAGS) \
                    $(QLOM_WARNINGS)
src_qlom_CPPFLAGS = $(QT_CPPFLAGS)
src_qlom_LDFLAGS  = $(QT_LDFLAGS)
src_qlom_LDADD = $(QT_LIBS) $(QLOM_LIBS)

BUILT_SOURCES = src/document.moc.cc \
		src/error_reporter.moc.cc \
                src/gui/main_window.moc.cc \
                src/tables_model.moc.cc \
                src/list_layout_model.moc.cc \
                src/layout_delegates.moc.cc \
                src/connection_dialog.moc.cc

iconthemedir = $(datadir)/icons/hicolor
appicon16dir = $(iconthemedir)/16x16/apps
appicon22dir = $(iconthemedir)/22x22/apps
appicon24dir = $(iconthemedir)/24x24/apps
appicon32dir = $(iconthemedir)/32x32/apps
appicon48dir = $(iconthemedir)/48x48/apps
appiconscdir = $(iconthemedir)/scalable/apps

dist_appicon16_DATA = icons/16x16/qlom.png
dist_appicon22_DATA = icons/22x22/qlom.png
dist_appicon24_DATA = icons/24x24/qlom.png
dist_appicon32_DATA = icons/32x32/qlom.png
dist_appicon48_DATA = icons/48x48/qlom.png
dist_appiconsc_DATA = icons/scalable/qlom.svg

noinst_icons = icons/16x16/qlom.svg icons/22x22/qlom.svg icons/32x32/qlom.svg icons/win32/qlom.ico

dist_noinst_DATA = $(noinst_icons)

mimedir = $(datadir)/mime
mimepackagesdir = $(mimedir)/packages
dist_mimepackages_DATA = qlom.xml

if QLOM_ENABLE_MAEMO
desktopdir = $(OSSO_DESKTOPDIR)
else
desktopdir = $(datadir)/applications
endif
dist_desktop_DATA = qlom.desktop
