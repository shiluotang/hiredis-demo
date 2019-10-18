#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([hiredis-demo], [1.0], [shengquangang2015@163.com])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([tests])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR
LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES([HIREDIS], [hiredis])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT
