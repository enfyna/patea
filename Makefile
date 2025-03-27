default: build/patea

CC=clang

CFLAGS=-Wall -Wextra -pedantic -I./build -I./src/include/
GTK_INC=`pkg-config -cflags -libs gtk+-3.0`
VTE_INC=`pkg-config -cflags -libs vte-2.91`

HEADERS=build/gres.h $(wildcard src/include/*.h)
SOURCES=$(wildcard src/*.c) build/gres.c

build/patea: $(SOURCES) $(HEADERS)
	$(info SOURCES => $(SOURCES))
	$(info HEADERS => $(HEADERS))
	$(info =====================)
	$(CC) $(SOURCES) -o $@ $(VTE_INC) $(GTK_INC) $(CFLAGS)

build/gres.c: src/res/gres.xml src/res/*
	@mkdir -p build
	glib-compile-resources src/res/gres.xml --generate-source --target=build/gres.c

build/gres.h: src/res/gres.xml src/res/*
	@mkdir -p build
	glib-compile-resources src/res/gres.xml --generate-header --target=build/gres.h

