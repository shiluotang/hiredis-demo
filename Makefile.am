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
test1_SOURCES	= ./tests/test1.cpp
test1_LDADD		= libhiredisxx.la

check_PROGRAMS += test2
test2_SOURCES	= ./tests/test2.cpp
test2_LDADD		= libhiredisxx.la
test2_CPPFLAGS	= $(AM_CPPFLAGS) $(CJSON_CPPFLAGS)
test2_LDFLAGS	= $(CJSON_LIBS)

check_PROGRAMS += test3
test3_SOURCES	= ./tests/test3.cpp
test3_LDADD		= libhiredisxx.la
test3_CPPFLAGS	= $(AM_CPPFLAGS) $(CJSON_CPPFLAGS)
test3_LDFLAGS	= $(CJSON_LIBS)

check_PROGRAMS += test4
test4_SOURCES	= ./tests/test4.cpp
test4_LDADD		= libhiredisxx.la
test4_CPPFLAGS	= $(AM_CPPFLAGS) $(CJSON_CPPFLAGS)
test4_LDFLAGS	= $(CJSON_LIBS)

check_PROGRAMS += test5
test5_SOURCES	= ./tests/test5.cpp
test5_LDADD     = libhiredisxx.la
test5_CPPFLAGS	= $(AM_CPPFLAGS) $(CJSON_CPPFLAGS)
test5_LDFLAGS	= $(CJSON_LIBS)

TESTS = $(check_PROGRAMS)
