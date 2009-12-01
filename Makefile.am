include $(top_srcdir)/build-aux/autotroll.mk

ACLOCAL_AMFLAGS = -I m4
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal

bin_PROGRAMS = src/qlom
src_qlom_SOURCES = src/qlom.cc \
		   src/glom_document.cc \
		   src/glom_document.moc.cc \
		   src/glom_document.h \
		   src/glom_table.cc \
		   src/glom_table.h \
		   src/glom_relationship.cc \
		   src/glom_relationship.h \
		   src/error_reporter.cc \
		   src/error_reporter.moc.cc \
		   src/error_reporter.h \
		   src/qlom_error.cc \
		   src/qlom_error.h \
                   src/gui/main_window.cc \
                   src/gui/main_window.moc.cc \
                   src/gui/main_window.h \
                   src/glom_tables_model.cc \
                   src/glom_tables_model.moc.cc \
                   src/glom_tables_model.h \
                   src/connection_dialog.cc \
                   src/connection_dialog.moc.cc \
                   src/connection_dialog.h \
                   src/glom_list_layout_model.cc \
                   src/glom_list_layout_model.moc.cc \
                   src/glom_list_layout_model.h \
                   src/glom_layout_delegates.cc \
                   src/glom_layout_delegates.moc.cc \
                   src/glom_layout_delegates.h \
                   src/utils.cc \
                   src/utils.h \
                   src/qlom_query_builder.cc \
                   src/qlom_query_builder.h

qlom_includes = -I$(top_builddir) -I$(top_srcdir)/src

src_qlom_CXXFLAGS = $(qlom_includes) $(QT_CXXFLAGS) $(QLOM_CFLAGS) \
                    $(QLOM_WARNINGS)
src_qlom_CPPFLAGS = $(QT_CPPFLAGS)
src_qlom_LDFLAGS  = $(QT_LDFLAGS)
src_qlom_LDADD = $(QT_LIBS) $(QLOM_LIBS)

BUILT_SOURCES = src/glom_document.moc.cc \
		src/error_reporter.moc.cc \
                src/gui/main_window.moc.cc \
                src/glom_tables_model.moc.cc \
                src/glom_list_layout_model.moc.cc \
                src/glom_layout_delegates.moc.cc \
                src/connection_dialog.moc.cc

mimedir = $(datadir)/mime
mimepackagesdir = $(mimedir)/packages
dist_mimepackages_DATA = qlom.xml

desktopdir = $(datadir)/applications
dist_desktop_DATA = qlom.desktop
