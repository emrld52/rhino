#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

// resize gl viewport as window is resized, print debug info also

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    printf("\nwindow resized to %dx%d", width, height);
}

void input_handling(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
}

// simple vertex shader, no proces, forward vertex data over unfiltered to next steps in graphics pipeline

const char *vertex_shader_source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos; \n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
"}\0";

// fragment shader source
    
const char *fragment_shader_source = "#version 330 core\n"
"out vec4 frag_color;\n"
"void main()\n"
"{"
"   frag_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

int main(void) {
    // init opengl, set version and profile (core profile)

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // init opengl window (fullscreen, change glfwGetPrimaryMonitor to NULL if you so need/desire windowed mode)

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rhino Framework", NULL, NULL);

    if (window == NULL) {
        printf("\nfailed to create a glfw window.");
        glfwTerminate();
        return -1;
    }

    // set context to the window just created

    glfwMakeContextCurrent(window);

    // frame resizing

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // init glad (opengl function pointers)

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\nfailed to init GLAD, could not load process.");
        return -1;
    }

    // pass dimensions into opengl viewport

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // ------------ SHADERS ------------ //

    // shader object of vertex shader type, store id as vertex_shader as an unsigned int

    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // compile vertex shader

    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    // check if shader compilation was successful, print error if not

    int success;
    char info_log[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf(info_log);
    }

    // bind and compile frag shader, same process as vertex shader

    unsigned int fragment_shader;

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // check for issues and reuse variables from before

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf(info_log);
    }

    // make shader program

    unsigned int shader_program;

    shader_program = glCreateProgram();

    // link shaders

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf(info_log);
    }

    glUseProgram(shader_program);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // triangle primitive as vertex coordinates

    float vertices[] = {
       -0.5f,  -0.5f,   0.0f,
        0.5f,  -0.5f,   0.0f,
        0.0f,   0.5f,   0.0f
    };

    // create buffer object of gl_array_buffer type and bind to gpu with id

    unsigned int VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // allocate memory and copy vertices into gpu memory

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // generate a vao (vertex array object)

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // begin render loop, check input and swap buffers

    while(!glfwWindowShouldClose(window)) {
        // read any input, used to exit program if forced
        input_handling(window);

        // set blank greenish background and clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // exit program, if havent exited manually

    glfwTerminate();

    printf("\nexited program successfully");

    return -1;
}