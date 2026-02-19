CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm

SRC = src/main.c src/app.c src/camera.c src/scene.c src/texture.c
OBJ = $(SRC:.c=.o)
TARGET = Program.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)