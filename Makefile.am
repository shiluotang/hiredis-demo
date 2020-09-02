AUTOMAKE_OPTIONS = -Wall -Werror foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Wall
AM_CFLAGS	=
AM_CXXFLAGS	=
AM_LDFLAGS	=

lib_LTLIBRARIES = libhiredisxx.la

libhiredisxx_la_SOURCES = ./src/hiredis.cpp
libhiredisxx_la_CPPFLAGS = $(AM_CPPFLAGS) $(HIREDIS_CFLAGS)
libhiredisxx_la_LIBADD = $(HIREDIS_LIBS)

SUBDIRS = src

check_PROGRAMS =

check_PROGRAMS += test1
test1_SOURCES	= ./tests/test1.cpp
test1_LDADD		= libhiredisxx.la

check_PROGRAMS += test2
test2_SOURCES	= ./tests/test2.cpp
test2_LDADD		= libhiredisxx.la

check_PROGRAMS += test3
test3_SOURCES	= ./tests/test3.cpp
test3_LDADD		= libhiredisxx.la

check_PROGRAMS += test4
test4_SOURCES	= ./tests/test4.cpp
test4_LDADD		= libhiredisxx.la

check_PROGRAMS += test5
test5_SOURCES	= ./tests/test5.cpp
test5_LDADD     = libhiredisxx.la

check_PROGRAMS += test-socket-timeout
test_socket_timeout_SOURCES = ./tests/test-socket-timeout.cpp
test_socket_timeout_LDADD 	= libhiredisxx.la

check_PROGRAMS += test-export-mcc
test_export_mcc_SOURCES = ./tests/test-export-mcc.cpp
test_export_mcc_LDADD 	= libhiredisxx.la

TESTS = $(check_PROGRAMS)
