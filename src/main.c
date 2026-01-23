#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// rhino headers

#include "shaders.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 960

#define PRINT_FRAME_TIME_PER_SECONDS 1.0f

#define CAMERA_MOV_SPEED 0.5f
#define CAMERA_ZOOM_SPEED 1.0f
#define CAMERA_ROT_SPEED 1.0f

// time between each frame, used for things such as values that change overtime to stay consistent (e.g movement)

float delta_time = 0.01f;

// camera orientation, starts at origin and of no rotation

float camera_rot = 0;
float camera_x, camera_y = 0;
float cam_x_vel, cam_y_vel = 0;
float camera_zoom = 1;

// resize gl viewport as window is resized, print debug info also

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    printf("\nwindow resized to %dx%d", width, height);
}

// --- INPUT HANDLING --- //

void input_handling(GLFWwindow* window) {
    // quick escape
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

    /* wasd movement, uses camera velocity of which we then pass into a rotation matrix 
    so we move up/down/left/right relative to the camera orientation */
    if(glfwGetKey(window, GLFW_KEY_W)) cam_y_vel = CAMERA_MOV_SPEED;
    else if(glfwGetKey(window, GLFW_KEY_S)) cam_y_vel = -CAMERA_MOV_SPEED;
    else cam_y_vel = 0;
    if(glfwGetKey(window, GLFW_KEY_D)) cam_x_vel = CAMERA_MOV_SPEED;
    else if(glfwGetKey(window, GLFW_KEY_A)) cam_x_vel = -CAMERA_MOV_SPEED;
    else cam_x_vel = 0;

    // rotation matrix

    camera_x += (cam_x_vel * cos(camera_rot) - cam_y_vel * sin(camera_rot)) * delta_time;
    camera_y += (cam_x_vel * sin(camera_rot) + cam_y_vel * cos(camera_rot)) * delta_time;

    // rotation and zoom

    if(glfwGetKey(window, GLFW_KEY_E)) camera_rot += CAMERA_ROT_SPEED * delta_time;
    if(glfwGetKey(window, GLFW_KEY_Q)) camera_rot += -CAMERA_ROT_SPEED * delta_time;

    if(glfwGetKey(window, GLFW_KEY_Z)) camera_zoom += -CAMERA_ZOOM_SPEED * delta_time;
    if(glfwGetKey(window, GLFW_KEY_X)) camera_zoom += CAMERA_ZOOM_SPEED * delta_time;

    // clamp zoom as to not go inverse

    if(camera_zoom <= 0.01f) camera_zoom = 0.01f;
}

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

    // print info about max number of vertex attribs

    int num_attributes;

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num_attributes);

    printf("maximum number of vertex attribs : %d", num_attributes);


    // ------------ SHADERS ------------ //

    unsigned int shader_program = link_and_compile_shaders("vertex_shader.glsl", "fragment_shader.glsl");

    glUseProgram(shader_program);


    // ------- QUAD DEFINE, VBO + VAO ------- //


    // quad primitive as vertex coordinates

    float vertices[] = {
       -0.5f,  -0.5f,   0.0f, /* bottom left */  1.0f, 1.0f, 0.0f, /* blue */
        0.5f,  -0.5f,   0.0f, /* bottom right */ 1.0f, 0.0f, 1.0f, /* yellow */
       -0.5f,   0.5f,   0.0f, /* top left */     0.0f, 1.0f, 1.0f, /* green */
        0.5f,   0.5f,   0.0f, /* top right */    1.0f, 1.0f, 0.0f, /* red */
    };

    unsigned int indices[] = {
        0, 1, 2, // tri 1
        3, 2, 1  // tri 2
    };

    // create buffer object of gl_array_buffer type and bind to gpu with id

    unsigned int VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // allocate memory and copy vertices into gpu memory

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create ebo

    unsigned int EBO;

    glGenBuffers(1, &EBO);

    // generate a vao (vertex array object)

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // bind vbo and ebo

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // unbind

    glBindVertexArray(0);

    // 2nd object, triangle, vao + vbo and attrib pointers

    float vertices2[] = {
        0.75f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.25f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.75f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    unsigned int VBO2, VAO2;

    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    glGenBuffers(1, &VBO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // uniforms, used for rotation and color for now

    float time;
    int time_uniform_location = glGetUniformLocation(shader_program, "time");

    int cam_pos_uniform = glGetUniformLocation(shader_program, "cam_position");
    int cam_rot_uniform = glGetUniformLocation(shader_program, "cam_orientation");
    int cam_zoom_uniform = glGetUniformLocation(shader_program, "zoom");

    // frametime and fps counter timer

    float last_frame_draw = 0.01f;
    float fps_timer_counter = PRINT_FRAME_TIME_PER_SECONDS;

    // begin render loop, check input and swap buffers

    while(!glfwWindowShouldClose(window)) {
        // read any input, used to exit program if forced
        input_handling(window);

        // set blank greenish background and clear screen
        glClearColor(0.65f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // get time for shaders and also frametime counter

        time = glfwGetTime();

        // frame time counters, print frametime counter every N seconds as specified in define at top

        delta_time = time - last_frame_draw;

        fps_timer_counter -= delta_time;

        if(fps_timer_counter <= 0) {
            // reset timer
            fps_timer_counter = PRINT_FRAME_TIME_PER_SECONDS;
            printf("\nfps : %f - frametime : %f\n", 1.0f / delta_time, delta_time);
        }

        glUseProgram(shader_program);

        glUniform1f(time_uniform_location, time);

        glUniform1f(cam_rot_uniform, camera_rot);
        glUniform1f(cam_zoom_uniform, camera_zoom);
        glUniform2f(cam_pos_uniform, camera_x, camera_y);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        last_frame_draw = time;
    }

    // exit program, if havent exited manually

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    glfwTerminate();

    printf("\nexited program successfully");

    return 0;
}