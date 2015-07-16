AC_INIT([Qlom], [0.1], [davidk@openismus.com], [qlom], [http://gitorious.org/qlom])
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

PKG_PROG_PKG_CONFIG

GLOM_1_22="glom-1.22 >= 1.22.6"
GLOM_1_24="glom-1.24 >= 1.24.0"
GLOM_1_26="glom-1.26 >= 1.26.0"
GLOM_1_30="glom-1.30 >= 1.29.0"
AC_MSG_CHECKING([for a suitable libglom version])
PKG_CHECK_EXISTS($GLOM_1_30,
                   [AC_MSG_RESULT([1.30])
                   REQUIRED_GLOM_VERSION=$GLOM_1_30],
                   [PKG_CHECK_EXISTS([$GLOM_1_26],
                   [AC_MSG_RESULT([1.26])
                   REQUIRED_GLOM_VERSION=$GLOM_1_26],
                   [PKG_CHECK_EXISTS([$GLOM_1_24],
                     [AC_MSG_RESULT([1.24])
                     REQUIRED_GLOM_VERSION=$GLOM_1_24],
                     [PKG_CHECK_EXISTS([$GLOM_1_22],
                       [AC_MSG_RESULT([1.12])
                       REQUIRED_GLOM_VERSION=$GLOM_1_22],
                       [AC_MSG_ERROR([suitable libglom version not found])])])])])

PKG_CHECK_MODULES([QLOM], [$REQUIRED_GLOM_VERSION])
AC_SUBST([QLOM_CFLAGS])
AC_SUBST([QLOM_LIBS])

AC_ARG_ENABLE([maemo],
              [AS_HELP_STRING([--enable-maemo],
                              [build with support for the Maemo platform])],
              [qlom_enable_maemo=$enableval],
              [qlom_enable_maemo=no])

AM_CONDITIONAL([QLOM_ENABLE_MAEMO], [test "x$qlom_enable_maemo" = xyes])

AS_IF([test "x$qlom_enable_maemo" = xyes],
     [PKG_CHECK_EXISTS([osso-af-settings],
      [MM_PKG_CONFIG_SUBST([OSSO_DESKTOPDIR], [--variable=desktopentrydir osso-af-settings])
       AC_DEFINE([QLOM_ENABLE_MAEMO], [1],
        [Define to enable support for the Maemo platform.])],
      [AC_MSG_ERROR([osso-af-settings not found])])
      ])

DK_ARG_ENABLE_WARNINGS([QLOM_WARNINGS],
        [-Wall],
        [-Wall -Wextra])
AC_SUBST([QLOM_WARNINGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
