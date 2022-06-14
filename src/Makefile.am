bin_PROGRAMS = $(top_builddir)/bin/SortTest
__top_builddir__bin_SortTest_SOURCES =  argparse.c csv.c Sort.c List.c SortTest.c GUI.c
include_HEADERS = argparse.h List.h GUI.h csv.h Sort.h
if BUILD_LINUX
__top_builddir__bin_SortTest_LDADD=-lX11
endif
if BUILD_WIN32
__top_builddir__bin_SortTest_LDADD=-lgdi32
__top_builddir__bin_SortTest_CFLAGS=-U__unix__ -D_WIN32
endif
