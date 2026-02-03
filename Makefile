SRC += src/main.c src/glad/glad.c src/shaders.c src/textures.c src/rhino_callbacks.c
BIN_DIR = bin

# linux executable assumed x11 and not wayland

ifeq ($(OS), Windows_NT)
	LIBS += -lglfw3 -lopengl32 -lgdi32 -luser32
	PROGRAM_NAME = rhino_demo.exe
else
	LIBS += -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
	PROGRAM_NAME = rhino_demo
endif

build_executable: src/main.c
ifeq ($(OS), Windows_NT)
	gcc $(SRC) -o $(BIN_DIR)/$(PROGRAM_NAME) $(LIBS) -mwindows -O3 -static
else
	gcc $(SRC) -o $(BIN_DIR)/$(PROGRAM_NAME) $(LIBS) -O3
endif