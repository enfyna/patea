default: patea.data build/patea

CC	?= clang

CFLAGS	= -I./build -I./src/include/ -I./src/third-party/ -lm \
	-Wall -Wextra -Wno-overlength-strings -pedantic 


GTK	?=`pkg-config -cflags -libs gtk+-3.0`
VTE	?=`pkg-config -cflags -libs vte-2.91`
SQL	?=`pkg-config -cflags -libs sqlite3`
LIBS	= $(GTK) $(VTE) $(SQL) -lm

HEADERS	= build/gres.h $(wildcard src/**/*.h)
SOURCES	= build/gres.o build/miniaudio.o $(patsubst src/%.c,build/%.o, $(wildcard src/*.c))

$(info HEADERS => $(HEADERS))
$(info SOURCES => $(SOURCES))

$(shell mkdir -p build)

build/patea: $(SOURCES) $(HEADERS) 
	$(CC) $(SOURCES) -o $@ $(CFLAGS) $(LIBS)

build/%.o: src/%.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LIBS)

build/miniaudio.o: src/third-party/miniaudio.c
	$(CC) $< -c -o $@

build/gres.c: src/res/gres.xml src/res/*
	glib-compile-resources src/res/gres.xml --generate-source --target=build/gres.c

build/gres.o: build/gres.c 
	$(CC) $< -c -o $@ $(LIBS)

build/gres.h: src/res/gres.xml src/res/*
	glib-compile-resources src/res/gres.xml --generate-header --target=build/gres.h

patea.data: sql/data.sql
	rm -f patea.data
	sqlite3 patea.data < sql/data.sql

.PHONY: clean
clean:
	rm -fr build/
