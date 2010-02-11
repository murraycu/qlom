AC_INIT([Qlom], [0.1], [davidk@openismus.com], [qlom])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/qlom.cc])
AC_PREREQ([2.57])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2 no-define nostdinc subdir-objects])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

AC_PROG_CXX
AC_PROG_INSTALL
AC_LANG([C++])

AT_WITH_QT([core gui sql],[no_keywords])

PKG_CHECK_MODULES([QLOM], [glom-1.12 >= 1.12.6])
AC_SUBST([QLOM_CFLAGS])
AC_SUBST([QLOM_LIBS])

DK_ARG_ENABLE_WARNINGS([QLOM_WARNINGS],
        [-Wall],
        [-Wall -Wextra])
AC_SUBST([QLOM_WARNINGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
