include $(top_srcdir)/build-aux/autotroll.mk

ACLOCAL_AMFLAGS = -I m4
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal

bin_PROGRAMS = src/qlom
src_qlom_SOURCES = src/qlom.cc \
                   src/main_window.cc \
                   src/main_window.moc.cc \
                   src/main_window.h \
                   src/glom_model.cc \
                   src/glom_model.moc.cc \
                   src/glom_model.h \
                   src/connection_dialog.cc \
                   src/connection_dialog.moc.cc \
                   src/connection_dialog.h \
                   src/glom_layout_model.cc \
                   src/glom_layout_model.moc.cc \
                   src/glom_layout_model.h \
                   src/open_sqlite.cc \
                   src/open_sqlite.h \
                   src/utils.cc \
                   src/utils.h
src_qlom_CXXFLAGS = $(QT_CXXFLAGS) $(QLOM_CFLAGS) $(QLOM_WARNINGS)
src_qlom_CPPFLAGS = $(QT_CPPFLAGS)
src_qlom_LDFLAGS  = $(QT_LDFLAGS)
src_qlom_LDADD = $(QT_LIBS) $(QLOM_LIBS)

BUILT_SOURCES = src/main_window.moc.cc \
                src/glom_model.moc.cc \
                src/glom_layout_model.moc.cc \
                src/connection_dialog.moc.cc

mimedir = $(datadir)/mime
mimepackagesdir = $(mimedir)/packages
dist_mimepackages_DATA = qlom.xml

desktopdir = $(datadir)/applications
dist_desktop_DATA = qlom.desktop
