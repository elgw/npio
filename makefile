# Use make TARGET=T
# Where T is one of PERFORMANCE, VALGRIND or DEBUG
TARGET?=PERFORMANCE

CC=gcc -std=gnu99

LDFLAGS=-lm
CFLAGS=-Wall -Wextra -pedantic -Iinclude/


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


all: npio libnpio.a

%.o: src/%.c
	$(CC) -fpic $(CFLAGS) -c $< -o $@

npio: libnpio.a src/npio_cli.c
	$(CC) $(CFLAGS) src/npio_cli.c -o npio -L./ -lnpio $(LDFLAGS)

libnpio.a: npio.o dp.o
	$(CC) -fpic --shared $(CFLAGS) npio.o dp.o -o libnpio.a

clean:
	rm -f *.o
	rm -f npio
	rm -f libnpio.a
	rm -f *.npy

install: libnpio.a src/npio.h npio
	cp libnpio.a /usr/local/lib/
	cp src/npio.h /usr/local/include/
	cp npio /usr/local/bin
