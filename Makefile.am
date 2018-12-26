AUTOMAKE_OPTIONS = -Wall -Werror foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Wall -Werror $(HIREDIS_CFLAGS)
AM_CFLAGS	=
AM_CXXFLAGS	=
AM_LDFLAGS	= $(HIREDIS_LIBS)

lib_LTLIBRARIES = libhiredisxx.la

libhiredisxx_la_SOURCES = ./src/hiredis.cpp

SUBDIRS = src

check_PROGRAMS =

check_PROGRAMS += test1
test1_SOURCES 	= ./tests/test1.cpp
test1_LDADD		= libhiredisxx.la

check_PROGRAMS += test2
test2_SOURCES 	= ./tests/test2.cpp
test2_LDADD		= libhiredisxx.la
test2_CPPFLAGS	= $(AM_CPPFLAGS) $(CJSON_CPPFLAGS)
test2_LDFLAGS	= $(CJSON_LIBS)

TESTS = $(check_PROGRAMS)
