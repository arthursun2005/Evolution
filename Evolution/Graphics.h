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
#include "World.hpp"

template <class T>
struct Graphics
{
    Graphics() {}
    
    Graphics(const Graphics&) = delete;
    
    void initialize();
    void destory();
    
    template <class It>
    void render(GLuint target, const Frame& frame, It* begin, It* end);
};

template <>
struct Graphics <Stick>
{
    static const int number_of_vertices = 32;
    
    GLuint vao[1];
    
    /// (positions, normal), (length, radius), shape
    GLuint vbo[3];
    
    typedef struct {
        vec2 p;
        vec2 n;
    } vbo0_type;
    
    typedef vec2 vbo1_type;
    
    std::vector<vbo0_type> vbo0;
    
    std::vector<vbo1_type> vbo1;
    
    glProgram program;
    
    void initialize() {
        program.initialize_with_header("glsl/stick.vs", "glsl/fill.fs", "glsl/common.glsl");
        
        glGenVertexArrays(1, vao);
        glGenBuffers(3, vbo);
        
        glBindVertexArray(vao[0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);
        
        vec2 vertices[number_of_vertices];
        
        for(int i = 0; i < number_of_vertices; ++i) {
            float a = 2.0f * M_PI * i / (float) number_of_vertices;
            vertices[i].x = cosf(a);
            vertices[i].y = sinf(a);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        glVertexAttribDivisor(0, 1);
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 0);
        
        glBindVertexArray(0);
    }
    
    void destory() {
        program.destory();
        
        glDeleteVertexArrays(1, vao);
        glDeleteBuffers(3, vbo);
    }
    
    template <class It>
    int load(It begin, It end) {
        size_t size = std::distance(begin, end);
        vbo0.resize(size);
        vbo1.resize(size);
        
        size_t i = 0;
        while(i != size) {
            vbo0[i].p = (*begin)->stick.position;
            vbo0[i].n = (*begin)->stick.normal;
            vbo1[i] = vec2((*begin)->stick.length, (*begin)->stick.radius);
            ++i;
            ++begin;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(vbo0_type), vbo0.data(), GL_STREAM_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(vbo1_type), vbo1.data(), GL_STREAM_DRAW);
        
        return (int)size;
    }
    
    template <class It>
    void render(GLuint target, const Frame& frame, It begin, It end) {
        int size = load(begin, end);
        
        program.bind();
        program.uniform2f("scl", frame.scl/(float)frame.w, frame.scl/(float)frame.h);
        program.uniform2f("offset", frame.offset.x, frame.offset.y);
        program.uniform4f("color", stick_color.r, stick_color.g, stick_color.b, stick_color.a);
        
        glBindFramebuffer(GL_FRAMEBUFFER, target);
        glBindVertexArray(vao[0]);
        glViewport(frame.x, frame.y, frame.w, frame.h);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, number_of_vertices, size);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

template <>
struct Graphics <Body>
{
    static const int number_of_vertices = 32;
    
    GLuint vao[1];
    
    /// (positions, radii), colors, shape
    GLuint vbo[3];
    
    typedef struct {
        vec2 p;
        float r;
    } vbo0_type;
    
    typedef Colorf vbo1_type;
    
    std::vector<vbo0_type> vbo0;
    
    std::vector<vbo1_type> vbo1;
    
    glProgram program;
    
    void initialize() {
        program.initialize_with_header("glsl/shape.vs", "glsl/fill.fs", "glsl/common.glsl");
        
        glGenVertexArrays(1, vao);
        glGenBuffers(3, vbo);
        
        glBindVertexArray(vao[0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);
        
        vec2 vertices[number_of_vertices];
        
        for(int i = 0; i < number_of_vertices; ++i) {
            float a = 2.0f * M_PI * i / (float) number_of_vertices;
            vertices[i].x = cosf(a);
            vertices[i].y = sinf(a);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        glVertexAttribDivisor(0, 1);
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 0);
        
        glBindVertexArray(0);
    }
    
    void destory() {
        program.destory();
        
        glDeleteVertexArrays(1, vao);
        glDeleteBuffers(3, vbo);
    }
    
    template <class It>
    int load(It begin, It end) {
        size_t size = std::distance(begin, end);
        vbo0.resize(size);
        vbo1.resize(size);
        
        size_t i = 0;
        while(i != size) {
            vbo0[i].p = (*begin)->getPosition();
            vbo0[i].r = (*begin)->getRadius();
            vbo1[i] = (*begin)->color;
            ++i;
            ++begin;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(vbo0_type), vbo0.data(), GL_STREAM_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(vbo1_type), vbo1.data(), GL_STREAM_DRAW);
        
        return (int)size;
    }
    
    template <class It>
    void render(GLuint target, const Frame& frame, It begin, It end) {
        int size = load(begin, end);
        
        program.bind();
        program.uniform2f("scl", frame.scl/(float)frame.w, frame.scl/(float)frame.h);
        program.uniform2f("offset", frame.offset.x, frame.offset.y);
        
        glBindFramebuffer(GL_FRAMEBUFFER, target);
        glBindVertexArray(vao[0]);
        glViewport(frame.x, frame.y, frame.w, frame.h);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, number_of_vertices, size);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

template <>
struct Graphics <World>
{
    const World* world;
    
    Texture2D texture;
    
    Graphics<Stick> stick_renderer;
    
    Graphics<Body> body_renderer;
    
    Graphics(World* world) : world(world) {}
    
    void initialize() {
        texture.initialize(GL_LINEAR);
        texture.image(GL_RGBA32F, GL_RGBA, world->width, world->height, GL_FLOAT, 0);
        
        stick_renderer.initialize();
        body_renderer.initialize();
    }
    
    void destory() {
        texture.destory();
        
        stick_renderer.destory();
        body_renderer.destory();
    }
    
    void render(GLuint target, const Frame& frame) {
        body_renderer.render(target, frame, world->cbegin(), world->cend());
        stick_renderer.render(target, frame, world->cbegin(), world->cend());
    }
};

#endif /* Graphics_h */
