#include "libs/cglm/cglm.h"
#include <GLFW/glfw3.h>

// camera stuff for allowing the navigation of 3d space

#define CAMERA_MOV_SPEED 3.0f
#define CAMERA_SENS 0.004f
#define CAMERA_FOV 60.0f

typedef struct camera_transform_t {
    vec3 posititon;
    vec3 front;
    vec3 up;
    vec3 direction;
    float mov_speed;
    float yaw, pitch;
    bool fullscreen;
} camera_transform;

typedef struct mouse_cursor_t {
    double x_pos, y_pos;
    float sens;
    bool unlocked;
} mouse_cursor;

typedef struct rhino_state_t {
    mouse_cursor mouse;
    camera_transform cam;
    GLFWwindow* window;
    float delta_time;
} rhino_state;

extern rhino_state rhino;