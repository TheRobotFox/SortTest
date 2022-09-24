bin_PROGRAMS = SortTest
SortTest_SOURCES =  argparse.c csv.c Sort.c SortList.c SortTest.c GUI.c GUI_impl_win.c GUI_impl_unix.c GUI_impl_ti.c List/List.c
include_HEADERS = argparse.h SortList.h GUI.h csv.h Sort.h SortTest.h List/List.h GUI_impl.h

if BUILD_NSPIRE

SortTest_CFLAGS=-Ofast -funsafe-math-optimizations
else
SortTest_CFLAGS=-Ofast -mavx2 -Wall -m64 -s -funsafe-math-optimizations
SortTest_CFLAGS_x=-g -fsanitize=address
endif

if BUILD_LINUX
SortTest_LDADD=-lX11 -lpthread -lm
endif
if BUILD_WIN32
SortTest_LDADD=-lgdi32
SortTest_CFLAGS+=-U__unix__ -D_WIN32
endif

if BUILD_NSPIRE
SortTest_CFLAGS+=-U__unix__ -DNSPIRE -marm
SortTest_LDFLAGS=-Wl,--nspireio,--gc-sections

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
