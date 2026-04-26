# Use make TARGET=T
# Where T is one of PERFORMANCE, VALGRIND or DEBUG
TARGET?=PERFORMANCE

CC=gcc -std=gnu99

LDFLAGS=-lm
#CFLAGS=-Wall -Wextra -pedantic
# https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
CFLAGS=-O2 -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough \
-Werror=format-security \
-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
-D_GLIBCXX_ASSERTIONS \
-fstrict-flex-arrays=3 \
-fstack-clash-protection -fstack-protector-strong \
-Wl,-z,nodlopen -Wl,-z,noexecstack \
-Wl,-z,relro -Wl,-z,now \
-Wl,--as-needed -Wl,--no-copy-dt-needed-entries

CFLAGS+=-Iinclude/


ifeq ($(TARGET), PERFORMANCE)
	CFLAGS+=-O3 -DNDEBUG
	LDFLAGS+=-flto
endif

ifeq ($(TARGET), VALGRIND)
	CFLAGS+=-g3
endif

ifeq ($(TARGET), DEBUG)
	CFLAGS+=-g3
	CFLAGS+=-fsanitize=undefined
	CFLAGS+=-fsanitize=address
	CFLAGS+=-fanalyzer
endif

$(info building for TARGET=$(TARGET))


all: npio libnpio.a libnpio.so

%.o: src/%.c
	$(CC) -fpic $(CFLAGS) -c $< -o $@

npio: libnpio.a src/npio_cli.c
	$(CC) $(CFLAGS) src/npio_cli.c -o npio -L./ -lnpio $(LDFLAGS)

npio_complete: src/npio_cli.c src/npio.c
	$(CC) $(CFLAGS) src/npio.c src/npio_cli.c -o npio_complete $(LDFLAGS)

libnpio.a: npio.o
	# r : insert with replacement
	# s : add/update index
	# v : verbosive
	ar rvs libnpio.a npio.o

libnpio.so: npio.o
	$(CC) -fPIC --shared $(CFLAGS) npio.o -o libnpio.so

clean:
	rm -f *.o
	rm -f npio
	rm -f libnpio.a
	rm -f libnpio.so
	rm -f *.npy

install: libnpio.a src/npio.h npio
	cp libnpio.a /usr/local/lib/
	cp src/npio.h /usr/local/include/
	cp npio /usr/local/bin
