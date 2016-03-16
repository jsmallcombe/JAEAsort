#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(range, 1.0.0, ricardo.yanez@calel.org)
AM_INIT_AUTOMAKE(range, 1.0.0)
AC_CONFIG_SRCDIR([src/range.h])
AC_CONFIG_HEADERS([config.h])

# 
AC_DISABLE_STATIC

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt log])

AC_OUTPUT(Makefile src/Makefile doc/Makefile examples/Makefile man/Makefile)
