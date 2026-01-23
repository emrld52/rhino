#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// fragment shader source
    
const char *fragment_shader_source = "#version 330 core\n"
"in vec3 col;\n"

"out vec4 frag_color;\n"

"void main()\n"
"{"
"   frag_color = vec4(col.xyz, 1.0f);\n"
"}\0";

// returns unsigned int to shader program

unsigned int link_and_compile_shaders(char* vertex_path, char* fragment_path) {
    // buffers which we will load shaders into, will then send these to gpu memory, compile and link them and then return the unsigned int pointing to the shader program in memory

    int success;

    char error_log[512];

    long length;

    // read shader file, load into memory, due to weird opengl shenanigans convert to const char * const * later on

    FILE* pF = fopen(vertex_path, "r");

    int i = 0;

    char temp[256];

    char* vert_shader_source[1024];

    while(fgets(temp, sizeof(temp), pF)) {
        vert_shader_source[i] = malloc(256);
        memcpy(vert_shader_source[i], temp, 256);
        i++;
    }

    // --- VERTEX SHADER --- //

    unsigned int vertex_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, i, (const char * const *)vert_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, error_log);
        printf("error when compiling vertex shader : %s", error_log);
    }

    // --- FRAGMENT SHADER --- //

    unsigned int fragment_shader;

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, error_log);
        printf("error when compiling fragment shader : %s", error_log);
    }

    // --- LINKING --- //

    unsigned int shader_program;

    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, error_log);
        printf("error when linking shader program : %s", error_log);
    }

    return shader_program;
}