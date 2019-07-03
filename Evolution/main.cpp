//
//  main.cpp
//  Evolution
//
//  Created by Arthur Sun on 5/31/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "BrainSystem.h"
#include <iostream>
#include "World.hpp"
#include "Builder.h"
#include "Graphics.h"

const char* hexFile = "brain.hex";
const char* logFile = "log";

FILE* log_file;

#define TRAINING true

#define WRITING false

#define READING false

#define pop_root 32

GLFWwindow *window;

double mouseX, mouseY;
double pmouseX = mouseX, pmouseY = mouseY;
int width = 1280;
int height = 840;

#if TRAINING
Builder builder(pop_root, pop_root, 60.0f, 60.0f, BodyDef());
Graphics<BodySystem> renderer(&builder);
float dt1 = 0.016f;
float dt2 = 5.0f;
int colSteps = 6;
int subSteps1 = 1;
int subSteps2 = subSteps1 * (dt2/dt1);
int mode = 1;
float totalScore = 0.0f;
#else
World world(500.0f, 500.0f, pop_root * pop_root);
Graphics<BodySystem> renderer(&world);
float dt = 0.016f;
int subSteps = 8;
int generation = 0;
#endif

bool paused = false;

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

void write() {
#if TRAINING
    std::ofstream os;
    os.open(hexFile);
    builder.write(os);
    os.close();
#endif
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_RELEASE) {
#if !TRAINING
        if(key == GLFW_KEY_A) {
            BodyDef def;
            def.position = getMouse();
            world.createBody(&def);
        }
        
        if(key == GLFW_KEY_D) {
            world.alter();
        }
        
        if(key == GLFW_KEY_G) {
            world.generate(BodyDef());
        }
        
        if(key == GLFW_KEY_R) {
            world.clear();
        }
        
        if(key == GLFW_KEY_B) {
            printf("%d\n", generation);
        }
        
        if(key == GLFW_KEY_Q) {
            printf("%.7f\n", world.getTreeQuality());
        }
        
        if(key == GLFW_KEY_N) {
            printf("%d\n", world.size());
        }
        
        if(key == GLFW_KEY_C) {
            printf("%d\n", world.getMaxBrainComplexity());
        }
#else
        if(key == GLFW_KEY_F) {
            mode = 0x1 ^ mode;
        }
        
        if(key == GLFW_KEY_C) {
            printf("%zu\n", builder.getMaxBrainComplexity());
        }
        
        if(key == GLFW_KEY_T) {
            printf("%.3f\n", builder.time);
        }
        
        if(key == GLFW_KEY_B) {
            printf("%d\n", builder.generation);
        }
        
        if(key == GLFW_KEY_K) {
            printf("%zu\n", builder.getBestBrainComplexity());
        }
        
        if(key == GLFW_KEY_B) {
            printf("%d\n", builder.generation);
        }
        
#endif
        
        if(key == GLFW_KEY_P || key == GLFW_KEY_SPACE) {
            paused = !paused;
        }
        
        if(key == GLFW_KEY_W) {
            write();
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
    
    glfwSwapInterval(0);
    
    Timer timer, subTimer;
    
    log_file = fopen(logFile, "w");
    
#if READING
    std::ifstream is;
    is.open(hexFile);
#if TRAINING
    builder.read(is);
#else
    world.read(is);
#endif
    is.close();
#endif
    
    do {
        bool press = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
#ifdef DEBUG
        float currentTime = timer.now();
        
        ++framesPerSecond;
        if(currentTime - lastSecondTime >= 1.0f) {
            printf("%f s/frame \n", (currentTime - lastSecondTime)/(float)framesPerSecond);
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
        
#if !TRAINING
        
        if(world.size() != 0 && world.size() < (world.maxBodies >> 1) && GLFW_PRESS == glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            world.alter();
            ++generation;
        }
        
#endif
        
        {
            glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            if(!paused) {
#if TRAINING
                int gen = builder.generation;
                
                float score;
                
                if(mode == 1)
                    score = builder.step(dt1, colSteps, subSteps1);
                else
                    score = builder.step(dt2, colSteps, subSteps2);
                
                if(gen != builder.generation) {
                    totalScore += score;
                    fprintf(log_file, "%6d ", builder.generation);
                    fprintf(log_file, "%9.3f ", score);
                    fprintf(log_file, "%5zu ", builder.getMaxBrainComplexity());
                    fprintf(log_file, "%5zu ", builder.getBestBrainComplexity());
                    fprintf(log_file, "%9.3f ", totalScore/(float)builder.generation);
                    fprintf(log_file, "\n");
                    fflush(log_file);
                }

#else
                world.step(dt, subSteps);
#endif
            }
            
            renderer.render(0, frame);
        }
        
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    } while (glfwWindowShouldClose(window) == GL_FALSE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    
    fprintf(log_file, "\n");
    
    fprintf(log_file, "real time: %.3f\n", timer.now());
    
#if !TRAINING
    
    fprintf(log_file, "generation: %d\n", generation);
    fprintf(log_file, "max complexity: %d\n", world.getMaxBrainComplexity());
    
#else
    
    fprintf(log_file, "generation: %d\n", builder.generation);
    fprintf(log_file, "max complexity: %zu\n", builder.getMaxBrainComplexity());
    fprintf(log_file, "best complexity: %zu\n", builder.getBestBrainComplexity());
    
#endif
    
#if WRITING
    write();
#endif
    
    renderer.destory();
    
    fclose(log_file);
    
    glfwDestroyWindow(window);
    glfwDestroyCursor(cursor);
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
