#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// returns unsigned int to shader program

unsigned int link_and_compile_shaders(char* vertex_path, char* fragment_path) {
    // buffers which we will load shaders into, will then send these to gpu memory, compile and link them and then return the unsigned int pointing to the shader program in memory
    char* vert_shader_source = 0;
    char* frag_shader_source = 0;

    // buffers we will use to check for errors and print out error data for

    int success;

    char error_log[512];

    long length;

    // --- LOAD SHADERS FROM FILE --- //

    FILE *pF = fopen(vertex_path, "rb");

    if(pF) {
        fseek(pF, 0, SEEK_END);
        length = ftell(pF);
        fseek(pF, 0, SEEK_SET);

        vert_shader_source = malloc(length + 1);
        if(vert_shader_source) {
            size_t bytes_read = fread(vert_shader_source, 1, length, pF);
            vert_shader_source[bytes_read] = '\0';
        }
        fclose(pF);
    }

    if(!vert_shader_source) return -1;

    printf("%s", vert_shader_source);

    pF = fopen(fragment_path, "rb");

    if(pF) {
        fseek(pF, 0, SEEK_END);
        length = ftell(pF);
        fseek(pF, 0, SEEK_SET);

        frag_shader_source = malloc(length + 1);
        if(frag_shader_source) {
            size_t bytes_read = fread(frag_shader_source, 1, length, pF);
            frag_shader_source[bytes_read] = '\0';
        }
        fclose(pF);
    }

    if(!frag_shader_source) {
        free(vert_shader_source);
        return -1;
    }

    printf("%s", frag_shader_source);

    // --- VERTEX SHADER --- //

    unsigned int vertex_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, (const char**)&vert_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, error_log);
        printf("error when compiling vertex shader : %s", error_log);
    }

    // --- FRAGMENT SHADER --- //

    unsigned int fragment_shader;

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, (const char**)&frag_shader_source, NULL);
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

    free(vert_shader_source);
    free(frag_shader_source);

    return shader_program;
}