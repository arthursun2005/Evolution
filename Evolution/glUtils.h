//
//  glUtils.h
//
//  Created by Arthur Sun on 6/8/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef glUtils_h
#define glUtils_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "common.h"

static GLuint texture_count = 0;

#define reset_texture_count (texture_count = 0)

struct Frame
{
    int x;
    int y;
    int w;
    int h;
    
    float scl;
    vec2 offset;
    
    inline Frame() {}
    
    inline Frame(int x, int y, int w, int h, float scl, const vec2& offset) : x(x), y(y), w(w), h(h), scl(scl), offset(offset) {}
};

enum shader_types
{
    gl_geometry_shader = GL_GEOMETRY_SHADER,
    gl_fragment_shader = GL_FRAGMENT_SHADER,
    gl_vertex_shader = GL_VERTEX_SHADER,
    gl_compute_shader = GL_COMPUTE_SHADER,
    gl_header_shader = 0
};

struct glShader
{
    std::string str;
    shader_types type;
    
    std::string file;
    
    glShader(const char* file_name) {
        fstr(file_name, &str);
        
        if(ends_in(file_name, ".cs")) {
            type = gl_compute_shader;
        }else if(ends_in(file_name, ".fs")) {
            type = gl_fragment_shader;
        }else if(ends_in(file_name, ".vs")) {
            type = gl_vertex_shader;
        }else if(ends_in(file_name, ".gs")) {
            type = gl_geometry_shader;
        }else if(ends_in(file_name, ".glsl")) {
            type = gl_header_shader;
        }else{
            throw std::invalid_argument("file name has to end in `.cs`, `.fs`, `.gs`, `.vs` or `.glsl`");
        }
        
        file.assign(file_name);
    }
    
    glShader(const char* file_name, shader_types type) : type(type) {
        fstr(file_name, &str);
        file.assign(file_name);
    }
    
    inline void apply_header(const glShader& shader) {
        assert(shader.type == gl_header_shader);
        str = shader.str + str;
    }
};

inline void apply_headers(const std::initializer_list<glShader*>& shaders, const glShader& header) {
    for(glShader* shader : shaders)
        shader->apply_header(header);
}

inline GLuint compileShader(const glShader& _shader) {
    GLuint shader = glCreateShader(_shader.type);
    const char* c_str = _shader.str.c_str();
    glShaderSource(shader, 1, &c_str, NULL);
    
    glCompileShader(shader);
    
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    
    if(status == GL_FALSE) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];
        glGetShaderInfoLog(shader, length, NULL, log);
        printf("%s \n", log);
    }
    
    printf("compiled %s\n", _shader.file.c_str());
    
    return shader;
}

inline GLuint compileProgram(const std::initializer_list<GLuint>& shaders) {
    GLuint program = glCreateProgram();
    
    for(GLuint shader : shaders)
        glAttachShader(program, shader);
    
    glLinkProgram(program);
    
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];
        glGetProgramInfoLog(program, length, NULL, log);
        printf("%s \n", log);
    }
    
    return program;
}

inline void deleteShaders(GLuint program, const std::initializer_list<GLuint>& shaders) {
    for(GLuint shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }
}

struct glProgram
{
    GLuint program;
    
    void initialize(const glShader& shader) {
        assert(shader.type == gl_compute_shader);
        GLuint cs = compileShader(shader);
        program = compileProgram({cs});
        deleteShaders(program, {cs});
    }
    
    void initialize(const glShader& shader1, const glShader& shader2) {
        assert(shader1.type == gl_vertex_shader);
        assert(shader2.type == gl_fragment_shader);
        
        GLuint vs = compileShader(shader1);
        GLuint fs = compileShader(shader2);
        
        program = compileProgram({vs, fs});
        deleteShaders(program, {vs, fs});
    }
    
    void initialize(const glShader& shader1, const glShader& shader2, const glShader& shader3) {
        assert(shader1.type == gl_vertex_shader);
        assert(shader2.type == gl_geometry_shader);
        assert(shader3.type == gl_fragment_shader);
        
        GLuint vs = compileShader(shader1);
        GLuint gs = compileShader(shader2);
        GLuint fs = compileShader(shader3);
        
        program = compileProgram({vs, gs, fs});
        deleteShaders(program, {vs, gs, fs});
    }
    
    void initialize_with_header(const glShader& shader1, const glShader& shader2) {
        glShader s1 = shader1;
        s1.apply_header(shader2);
        initialize(s1);
    }
    
    void initialize_with_header(const glShader& shader1, const glShader& shader2, const glShader& shader3) {
        glShader s1 = shader1;
        glShader s2 = shader2;
        apply_headers({&s1, &s2}, shader3);
        initialize(s1, s2);
    }
    
    void initialize_with_header(const glShader& shader1, const glShader& shader2, const glShader& shader3, const glShader& shader4) {
        glShader s1 = shader1;
        glShader s2 = shader2;
        glShader s3 = shader3;
        apply_headers({&s1, &s2, &s3}, shader4);
        initialize(s1, s2, s3);
    }
    
    inline void destory() {
        glDeleteProgram(program);
    }
    
    inline void bind() {
        glUseProgram(program);
    }
    
    inline void uniform1i(const char* n, int i0) const {
        glUniform1i(glGetUniformLocation(program, n), i0);
    }
    
    inline void uniform2i(const char* n, int i0, int i1) const {
        glUniform2i(glGetUniformLocation(program, n), i0, i1);
    }
    
    inline void uniform3i(const char* n, int i0, int i1, int i2) const {
        glUniform3i(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    inline void uniform4i(const char* n, int i0, int i1, int i2, int i3) const {
        glUniform4i(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    inline void uniform1ui(const char* n, uint i0) const {
        glUniform1ui(glGetUniformLocation(program, n), i0);
    }
    
    inline void uniform2ui(const char* n, uint i0, uint i1) const {
        glUniform2ui(glGetUniformLocation(program, n), i0, i1);
    }
    
    inline void uniform3ui(const char* n, uint i0, uint i1, uint i2) const {
        glUniform3ui(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    inline void uniform4ui(const char* n, uint i0, uint i1, uint i2, uint i3) const {
        glUniform4ui(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    inline void uniform1f(const char* n, float f0) const {
        glUniform1f(glGetUniformLocation(program, n), f0);
    }
    
    inline void uniform2f(const char* n, float f0, float f1) const {
        glUniform2f(glGetUniformLocation(program, n), f0, f1);
    }
    
    inline void uniform3f(const char* n, float f0, float f1, float f2) const {
        glUniform3f(glGetUniformLocation(program, n), f0, f1, f2);
    }
    
    inline void uniform4f(const char* n, float f0, float f1, float f2, float f3) const {
        glUniform4f(glGetUniformLocation(program, n), f0, f1, f2, f3);
    }
};

inline void destoryPrograms(const std::initializer_list<glProgram*>& programs) {
    for(glProgram* p : programs)
        p->destory();
}

struct Texture
{
    GLuint id;
    GLuint texture;
    GLuint fbo;
    
    Texture() {}
    
    Texture(const Texture& t) = delete;
    
    Texture& operator = (const Texture& t) = delete;
    
    void initialize(GLenum);
    
    inline void destory() {
        glDeleteTextures(1, &texture);
        glDeleteFramebuffers(1, &fbo);
    }
    
    void bind();
    
    inline void clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

struct Texture2D : public Texture
{
    void initialize(GLenum mode) {
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void image(GLenum iformat, GLenum format, GLuint x, GLuint y, GLenum type, void* pixels)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, iformat, x, y, 0, format, type, pixels);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    inline void bind() {
        id = texture_count++;
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};

template <class T>
struct _DoubleT
{
    T textures[2];
    
    void initialize(GLenum mode) {
        textures[0].initialize(mode);
        textures[1].initialize(mode);
    }
    
    inline const T& operator [] (int i) const {
        return textures[i];
    }
    
    inline T& operator [] (int i) {
        return textures[i];
    }
    
    inline void clear() {
        textures[0].clear();
        textures[1].clear();
    }
    
    inline void destory() {
        textures[0].destory();
        textures[1].destory();
    }
    
    inline void swap() {
        std::swap(textures[0].id, textures[1].id);
        std::swap(textures[0].texture, textures[1].texture);
        std::swap(textures[0].fbo, textures[1].fbo);
    }
};

struct DoubleTexture2D : public _DoubleT<Texture2D> {
    void image(GLenum iformat, GLenum format, GLuint x, GLuint y, GLenum type, void* pixels) {
        textures[0].image(iformat, format, x, y, type, pixels);
        textures[1].image(iformat, format, x, y, type, pixels);
    }
};

inline void destoryTextures(const std::initializer_list<Texture*>& textures) {
    for(Texture* t : textures)
        t->destory();
}

inline void destoryTextures(const std::initializer_list<DoubleTexture2D*>& textures) {
    for(DoubleTexture2D* t : textures)
        t->destory();
}

#endif /* glUtils_h */
