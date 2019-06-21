//
//  main.cpp
//  Evolution
//
//  Created by Arthur Sun on 5/31/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include <iostream>
#include "World.hpp"
#include "Graphics.h"

#define WIDTH 200
#define HEIGHT 200

GLFWwindow *window;
World world(WIDTH, HEIGHT);
Graphics<World> renderer(&world);

double mouseX, mouseY;
double pmouseX = mouseX, pmouseY = mouseY;
int width = 1280;
int height = 840;
float dt = 0.016f;

Frame frame;

float dMouseX = 0.0f;
float dMouseY = 0.0f;

#ifdef DEBUG

int framesPerSecond = 0;
float lastSecondTime = glfwGetTime();

#endif

#define mouse_scale 0.95f

#define scroll_scale 0.001f

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
}

inline vec2 getMouse() {
    float k = 2.0f / frame.scl;
    vec2 mouse((mouseX * 2.0f - width) * k, (mouseY * 2.0f - height) * -k);
    mouse -= frame.offset;
    return mouse;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_RELEASE) {
        if(key == GLFW_KEY_A) {
            BodyDef def;
            def.position = getMouse();
            world.createBody(&def);
        }
        
        if(key == GLFW_KEY_D) {
            world.alter();
        }
        
        if(key == GLFW_KEY_R) {
            world.clear();
        }
        
        if(key == GLFW_KEY_Q) {
            printf("%.7f\n", world.getTreeQuality());
        }
        
        if(key == GLFW_KEY_N) {
            printf("%d\n", world.size());
        }
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    frame.scl *= exp(yoffset * scroll_scale);
}

void resizeCallback(GLFWwindow* window, int w, int h)
{
    width = w;
    height = h;
    frame.w = width * 2;
    frame.h = height * 2;
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(0));
    
    if(!glfwInit())
        return EXIT_FAILURE;
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    window = glfwCreateWindow(width, height, "Evolution", NULL, NULL);
    
    glfwMakeContextCurrent(window);
    
    glewInit();
    
    const unsigned char* version = glGetString(GL_VERSION);
    
    printf("%s\n", version);
    
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    
    glfwSetCursor(window, cursor);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    
    mouseX = width * 0.5f;
    mouseY = height * 0.5f;
    
    frame.x = 0;
    frame.y = 0;
    frame.w = width * 2;
    frame.h = height * 2;
    frame.scl = 100.0f;
    frame.offset = vec2(0.0f, 0.0f);
    
    renderer.initialize();
    
    glfwSwapInterval(1);
    
    BodyDef def;
    def.position = vec2(-4.0f, -0.5f);
    def.velocity = vec2(3.0f, 0.0f);
    world.createBody(&def);
    
    def.position = vec2(4.0f, 0.5f);
    def.velocity = vec2(-6.0f, 0.0f);
    world.createBody(&def);
    
    do {
        float currentTime = glfwGetTime();
        bool press = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
#ifdef DEBUG
        ++framesPerSecond;
        if(currentTime - lastSecondTime >= 1.0f) {
            printf("%f ms/frame \n", 1000.0f * (currentTime - lastSecondTime)/(float)framesPerSecond);
            framesPerSecond = 0;
            lastSecondTime = currentTime;
        }
#endif
        
        if(press) {
            dMouseX = (mouseX - pmouseX);
            dMouseY = (pmouseY - mouseY);
        }else{
            dMouseX *= mouse_scale;
            dMouseY *= mouse_scale;
        }
        
        frame.offset.x += dMouseX * 4.0f / frame.scl;
        frame.offset.y += dMouseY * 4.0f / frame.scl;
        
        pmouseX = mouseX;
        pmouseY = mouseY;
        
        {
            glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            //renderer.setVision();
            
            world.step(dt, 5);
            
            renderer.render(0, frame);
        }
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    } while (glfwWindowShouldClose(window) == GL_FALSE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    
    renderer.destory();
    
    glfwDestroyWindow(window);
    glfwDestroyCursor(cursor);
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
