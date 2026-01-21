#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

bool has_user_exited = false;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // make a fullscreen window on the primary monitor, change glfwgetprimarymonitor to null to go back to windowed

    window = glfwCreateWindow(2560, 1440, "Hello World", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window) || !has_user_exited)
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // escape program

        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwTerminate();
            return 0;
        }
    }

    glfwTerminate();
    return 0;
}