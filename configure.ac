#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([librsm], [0.0.1], [mickyching@163.com])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
# Checks for libraries.
AC_PROG_RANLIB
# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main])
# FIXME: Replace `main' with a function in `-lGLU':
AC_CHECK_LIB([GLU], [main])
# FIXME: Replace `main' with a function in `-lglut':
AC_CHECK_LIB([glut], [main])
# FIXME: Replace `main' with a function in `-lgts':
AC_CHECK_LIB([gts], [main])
# FIXME: Replace `main' with a function in `-lglib-2.0':
AC_CHECK_LIB([glib-2.0], [main])

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([floor sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 examples/Makefile
                 ])
AC_CONFIG_SUBDIRS([])
AC_OUTPUT
