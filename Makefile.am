bin_PROGRAMS = SortTest
SortTest_SOURCES = argparse.c csv.c Sort.c SortList.c SortTest.c GUI.c GUI_impl_win.c GUI_conscreen.c GUI_impl_unix.c GUI_impl_ti.c NoFont/Font_Driver.c List/List.c Conscreen/Conscreen_ANSI.c Conscreen/Conscreen.c Conscreen/Conscreen_screen.c Conscreen/Conscreen_string.c Conscreen/Conscreen_console.c

SortTest_DEPENDENCIES = Font.font.c 
include_HEADERS = argparse.h SortList.h GUI.h csv.h Sort.h SortTest.h List/List.h GUI_impl.h NoFont/Font_Driver.h Conscreen/Conscreen.h Conscreen/Conscreen_ANSI.h Conscreen/Conscreen_string.h Conscreen/Conscreen_console.h Conscreen/Conscreen_screen.h


_EXTRA_CLEANUP=Font.font.c 

if BUILD_WIN32
PY=python
else
PY=python3
endif

SortTest_CFLAGS=-Wall -Wpedantic -Wextra

if DEBUG
SortTest_CFLAGS+=-ggdb -fsanitize=address
else

SortTest_CFLAGS+=-Ofast -funsafe-math-optimizations

if BUILD_NSPIRE
_EXTRA_CLEANUP+= $(PROGRAMS:.elf=.tns)
else
SortTest_CFLAGS+=-mavx2 -m64 -s
endif

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

endif


GUI.c: Font.font.c
Font.font.c: Font.bmp
	$(PY) $(top_srcdir)/NoFont/img2font.py $< "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()[]{}!?\"=/\'+-*#.,_" true

clean-local: clean-local-extra

.PHONY: clean-local-extra
clean-local-extra:
	rm -f $(_EXTRA_CLEANUP)
