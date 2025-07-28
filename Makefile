COMPILER = clang
CFILES = main.c
OUT = -o bin/main
INCLUDE = -I/opt/homebrew/include/
LIBDIR  = -L/opt/homebrew/lib/
LIBS = -lraylib

run:
	mkdir -p bin
	$(COMPILER) $(CFILES) $(INCLUDE) $(OUT) $(LIBDIR) $(LIBS) && ./bin/main