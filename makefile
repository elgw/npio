# PERFORMANCE, VALGRIND or DEBUG
TARGET?=PERFORMANCE

cc=gcc

ldflags=-lm
cflags=$(cflags_ext) -Wall -pedantic -Wextra


ifeq ($(TARGET), PERFORMANCE)
	cflags+=-O3 -DNDEBUG
	ldflags+=-flto
endif

ifeq ($(TARGET), VALGRIND)
	cflags+=-g
endif

ifeq ($(TARGET), DEBUG)
	cflags+=-g
	cflags+=-fsanitize=undefined
	cflags+=-fsanitize=address
endif

$(info building for TARGET=$(TARGET))

objects := npio.o dp.o

%.o: src/%.c
	$(cc) $(cflags) -c $< -o $@

npio: $(objects)
	$(cc) $(cflags) src/npio_cli.c $(objects) -o npio

clean:
	rm *.o
	rm npio
