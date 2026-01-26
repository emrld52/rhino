![App screenshot](rhino_logo.png)

# Rhino Framework

Rhino is a simple 3D renderer written from the ground up in C using the OpenGL rendering API as its backend, written in just under a week (with 2 or so weeks prior dedicated to mathematical research and graphics research).

# Features 

📦 - Working 3D projection, 3D standard projection x view x model matrix
🎨 - Textures, binding to multiple texture units and rendering to multiple textures at once with various effects
⌨ - Input handling, 3D FPS-style camera in the available demo
💡 - Point lighting system

![App screenshot](example.gif)

# Backstory

I've desired to take a significant plunge into graphics programming for a while now. My previous project "ZombieGuy40" touched upon graphics/engine dev in a 2D space, however Rhino is my first attempt at making a 3D renderer from scratch in C. I'd considered myself quite average at math before working on Rhino (I knew some foundational linear algebra & trig), however wanting to enter the graphics programming field I thought it'd be meaningful to make a 3D renderer from scratch, learning all the math and concepts myself with merely the LearnOpenGL website and the 3Blue1Brown linear algebra course.

I knew in order to enter the field of games that I so desired (graphics programming), I'd need to seriously catch up on my math ability, and for the independant research project of my college games course, I decided this would be the perfect time to do so and get some practical nitty gritty graphics programming done.

# Development

The development of the Rhino renderer was a unique challenge for me. A significant chunk of developing Rhino was merely reading up on linear algebra concepts (e.g matrices, matrix-matrix multiplication, 4x4 matrices) and more advanced trigonometry, however there was also a handful of unique technical lessons learnt in terms of practical execution. These are as follows :

- Working with bindings and the OpenGL state, understanding VAOs + objects & targets
- Creating a text file parser from scratch in C to load my vertex & fragment shaders
- Learning how to apply learnt math concepts with the CGLM math library

# Libraries

Rhino only uses a handful of low-level libraries :

- GLFW (windowing and context management)

- GLAD (loading OpenGL function pointers)

- stb_image.h (image parsing)

- CGLM (C port of the OpenGL Math library)

# Building

Windows (MSYS2 + GCC + Make) :

- clone repo
- ensure GLFW library is downloaded and in your includes
- run "make"

Linux (GCC + Make) :
- should be the same, you will need to change the bound libraries in the Makefile to that of the relevant Linux libraries though, if I can find these I'll include them in an ifdef
