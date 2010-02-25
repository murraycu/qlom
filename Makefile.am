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
		   src/gui/list_view.cc \
		   src/gui/list_view.moc.cc \
		   src/gui/list_view.h \
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
		src/gui/list_view.moc.cc \
                src/gui/main_window.moc.cc \
                src/tables_model.moc.cc \
                src/list_layout_model.moc.cc \
                src/layout_delegates.moc.cc \
                src/connection_dialog.moc.cc

mimedir = $(datadir)/mime
mimepackagesdir = $(mimedir)/packages
dist_mimepackages_DATA = qlom.xml

desktopdir = $(datadir)/applications
dist_desktop_DATA = qlom.desktop
