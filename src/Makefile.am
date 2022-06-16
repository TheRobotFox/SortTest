bin_PROGRAMS = $(top_builddir)/bin/SortTest
__top_builddir__bin_SortTest_SOURCES =  argparse.c csv.c Sort.c List.c SortTest.c GUI.c
include_HEADERS = argparse.h List.h GUI.h csv.h Sort.h

if BUILD_NSPIRE

__top_builddir__bin_SortTest_CFLAGS=-Os
else
__top_builddir__bin_SortTest_CFLAGS=-O3 -mavx2 -m64 -s
endif

if BUILD_LINUX
__top_builddir__bin_SortTest_LDADD=-lX11 -lpthread -lm
endif
if BUILD_WIN32
__top_builddir__bin_SortTest_LDADD=-lgdi32
__top_builddir__bin_SortTest_CFLAGS+=-U__unix__ -D_WIN32
endif

if BUILD_NSPIRE
__top_builddir__bin_SortTest_CFLAGS+=-U__unix__ -DNSPIRE -marm
__top_builddir__bin_SortTest_LDFLAGS=-Wl,--nspireio,--gc-sections

EXEEXT+=.elf

SUFFIXES: .tns .zehn .elf .c .o

GENZEHN = genzehn
ZEHNFLAGS = --name "SortTest"

EXE=$(top_builddir)/bin/SortTest

.elf.zehn:
		$(GENZEHN) --input $< --output $@ $(ZEHNFLAGS)
		rm $<
.zehn.tns: 
		make-prg $< $@
		rm $<

all-am: Makefile $(PROGRAMS:.elf=.tns) $(HEADERS)

clean-local:
	rm -f $(PROGRAMS:.elf=.tns)

endif
