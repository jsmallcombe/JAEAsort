AM_CFLAGS = -g -O
AM_LDFLAGS = -lm

include_HEADERS = range.h

lib_LTLIBRARIES = librange.la
librange_la_SOURCES = ranges.c
librange_la_LDFLAGS = -O -version-info 1:0:0

bin_PROGRAMS = range
range_SOURCES = range.c
range_LDADD = librange.la
