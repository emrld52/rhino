MAKEFLAGS += -j8

CC = gcc
LIBS += -lglfw3 -lopengl32 -lgdi32 -luser32

SRC += src/main.c src/glad/glad.c

BIN_DIR = bin

PROGRAM_NAME = rhino_demo.exe

# windows only, set to -mwindows to hide console, otherwise leave blank (e.g if debugging)

HIDE_CONSOLE = -mwindows

build_executable: src/main.c
	$(CC) $(SRC) -o $(BIN_DIR)/$(PROGRAM_NAME) $(LIBS) $(HIDE_CONSOLE)
	$(BIN_DIR)/$(PROGRAM_NAME)