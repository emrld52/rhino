#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

// returns unsigned int to shader program

unsigned int link_and_compile_shaders(char* vertex_path, char* fragment_path);