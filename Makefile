# === Переменные ===
CC = clang
CFLAGS = -I/opt/homebrew/include -Wall -Wextra -O0
LDFLAGS = -L/opt/homebrew/lib -lraylib

TARGET = main
SRC = main.c
OBJ = main.o

# === Цели ===
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
