default: patea.data build/patea

CC=clang

CFLAGS= -I./build -I./src/include/ -I./src/third-party/ -lm \
	-Wall -Wextra -Wno-overlength-strings -pedantic

GTK_INC=`pkg-config -cflags -libs gtk+-3.0`
VTE_INC=`pkg-config -cflags -libs vte-2.91`
SQL_INC=`pkg-config -cflags -libs sqlite3`
LIBS= $(GTK_INC) $(VTE_INC) $(SQL_INC)

HEADERS=build/gres.h $(wildcard src/include/*.h) $(wildcard src/third-party/*.h)
SOURCES=build/gres.c $(wildcard src/*.c) $(wildcard src/third-party/*.c)

build/patea: $(SOURCES) $(HEADERS)
	$(info SOURCES => $(SOURCES))
	$(info HEADERS => $(HEADERS))
	$(info =====================)
	$(CC) $(SOURCES) -o $@ $(CFLAGS) $(LIBS)

build/gres.c: src/res/gres.xml src/res/*
	@mkdir -p build
	glib-compile-resources src/res/gres.xml --generate-source --target=build/gres.c

build/gres.h: src/res/gres.xml src/res/*
	@mkdir -p build
	glib-compile-resources src/res/gres.xml --generate-header --target=build/gres.h

patea.data: sql/data.sql
	sqlite3 patea.data < sql/data.sql

.PHONY: clean
clean:
	rm -fr build/
