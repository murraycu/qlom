TARGET = qlom

CONFIG+=link_pkgconfig
PKGCONFIG+=glom-1.30

CONFIG += c++11

CONFIG += no_keywords

QT += gui widgets sql

INCLUDEPATH = "./src"

configh.input = config.h.in
configh.output = config.h

PACKAGE_NAME = $$TARGET
PACKAGE_VERSION = "0.1"
QMAKE_SUBSTITUTES += config.h.in

HEADERS += \
		   src/table.h \
		   src/relationship.h \
		   src/error.h \
		   src/gui/list_view.h \
		   src/gui/main_window.h \
		   src/tables_model.h \
		   src/connection_dialog.h \
		   src/list_layout_model.h \
		   src/layout_delegates.h \
		   src/document.h \
		   src/utils.h

SOURCES += \
		   src/qlom.cc \
		   src/table.cc \
		   src/relationship.cc \
		   src/error.cc \
		   src/gui/list_view.cc \
		   src/gui/main_window.cc \
		   src/tables_model.cc \
		   src/connection_dialog.cc \
		   src/list_layout_model.cc \
		   src/layout_delegates.cc \
		   src/document.cc \
		   src/utils.cc
