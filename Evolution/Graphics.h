//
//  Graphics.h
//  Evolution
//
//  Created by Arthur Sun on 6/15/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Graphics_h
#define Graphics_h

#include "glUtils.h"

template <class T>
struct Graphics
{
    Graphics(const Graphics&) = delete;
    
    virtual void initialize();
    virtual void destory();
    
    virtual void render(GLuint target, const Frame& frame, T* objs, int size);
};

template <>
struct Graphics <Stick>
{
    void render(GLuint target, const Frame& frame, Stick* sticks, int size);
};

template <>
struct Graphics <Body>
{
    void render(GLuint target, const Frame& frame, Body* bodies, int size);
};

template <>
struct Graphics <World>
{
    const World* world;
    
    Texture2D texture;
    
    Graphics(World* world) : world(world) {}
    
    void initialize() {
        texture.initialize(GL_LINEAR);
        texture.image(GL_RGBA32F, GL_RGBA, world->width, world->height, GL_FLOAT, 0);
    }
    
    void destory() {
        texture.destory();
    }
    
    void render(GLuint target, const Frame& frame);
};

void Graphics<Stick>::render(GLuint target, const Frame& frame, Stick* sticks, int size) {
}

void Graphics<Body>::render(GLuint target, const Frame& frame, Body* bodies, int size) {
}

#endif /* Graphics_h */
