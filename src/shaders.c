#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// returns unsigned int to shader program

unsigned int link_and_compile_shaders(char* vertex_path, char* fragment_path) {
    // print info about max number of vertex attribs

    int num_attributes;

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num_attributes);

    printf("maximum number of vertex attribs : %d", num_attributes);

    // buffers which we will load shaders into, will then send these to gpu memory, compile and link them and then return the unsigned int pointing to the shader program in memory

    int success;

    char error_log[512];

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

    fclose(pF);

    // same for frag shader

    FILE* pF2 = fopen(fragment_path, "r");
    fseek(pF2, 0, SEEK_SET);

    int j = 0;

    char frag_temp[256];

    char* frag_shader_source[1024];

    while(fgets(frag_temp, sizeof(frag_temp), pF2)) {
        frag_shader_source[j] = malloc(256);
        memcpy(frag_shader_source[j], frag_temp, 256);
        j++;
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

    glShaderSource(fragment_shader, j, (const char * const *)frag_shader_source, NULL);
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