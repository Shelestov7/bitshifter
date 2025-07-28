COMPILER = clang
CFILES = main.c
OUT = -o bin/main
INCLUDE = -I/opt/homebrew/include/
LIBDIR  = -L/opt/homebrew/lib/
LIBS = -lraylib

run:
	mkdir -p bin
	$(COMPILER) $(CFILES) $(INCLUDE) $(OUT) $(LIBDIR) $(LIBS) && ./bin/main

debug:
	mkdir -p bin
	$(COMPILER) -g -O0 $(CFILES) $(INCLUDE) -o bin/debug $(LIBDIR) $(LIBS)

