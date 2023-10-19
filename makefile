# PERFORMANCE, VALGRIND or DEBUG
TARGET?=PERFORMANCE

CC=gcc -std=gnu99

LDFLAGS=-lm
CFLAGS=-Wall -Wextra -pedantic


ifeq ($(TARGET), PERFORMANCE)
	CFLAGS+=-O3 -DNDEBUG
	LDFLAGS+=-flto
endif

ifeq ($(TARGET), VALGRIND)
	CFLAGS+=-g
endif

ifeq ($(TARGET), DEBUG)
	CFLAGS+=-g3
	CFLAGS+=-fsanitize=undefined
	CFLAGS+=-fsanitize=address
	CFLAGS+=-fanalyzer
endif

$(info building for TARGET=$(TARGET))

objects := npio.o dp.o

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

npio: $(objects)
	$(CC) $(CFLAGS) src/npio_cli.c $(objects) -o npio $(LDFLAGS)

clean:
	rm *.o
	rm npio
