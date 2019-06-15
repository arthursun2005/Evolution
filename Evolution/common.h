//
//  common.h
//  Evolution
//
//  Created by Arthur Sun on 5/31/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef common_h
#define common_h

#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <new>
#include <cmath>

inline void* Alloc(size_t size) {
#ifdef _LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE
    return ::operator new(size);
#else
    return __builtin_operator_new(size);
#endif
}

inline void Free(void* ptr) {
#ifdef _LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE
    ::operator delete(ptr);
#else
    __builtin_operator_delete(ptr);
#endif
}

enum activation_function_types {
    e_linear = 0,
    e_sigmoid,
    e_tanh,
    e_step,
    e_relu,
    e_gauss,
    e_sine,
    e_cosine,
    e_abs,
    e_inv,
    numberOfActivationFunctions
};

inline int bit(float x) {
    return 0b1 & ~(*(int*)&x >> 31);
}

struct ActivationFunction
{
    int type = e_linear;
    
    inline float operator () (float x) const {
        if(type == e_linear) {
            return x;
        }
        
        if(type == e_sigmoid) {
            return 1.0f / (1.0f + exp(-x));
        }
        
        if(type == e_tanh) {
            return tanh(x);
        }
        
        if(type == e_step) {
            return bit(x);
        }
        
        if(type == e_relu) {
            return bit(x) * x;
        }
        
        if(type == e_gauss) {
            return exp(-(x * x) * 0.5f);
        }
        
        if(type == e_sine) {
            return sinf(M_PI * x);
        }
        
        if(type == e_cosine) {
            return cosf(M_PI * x);
        }
        
        if(type == e_abs) {
            return x < 0.0f ? -x : x;
        }
        
        if(type == e_inv) {
            return -x;
        }
        
        return 0.0f;
    }
};

struct vec2
{
    float x;
    float y;
    
    inline vec2() {}
    
    inline vec2(float x, float y) : x(x), y(y) {}
    
    inline vec2& operator -= (const vec2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    
    inline vec2& operator += (const vec2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    
    inline vec2& operator *= (float s) {
        x *= s;
        y *= s;
        return *this;
    }
    
    inline vec2 operator - () const {
        return vec2(-x, -y);
    }
    
    inline vec2 T () const {
        return vec2(x, -y);
    }
    
    inline vec2 I () const {
        return vec2(y, -x);
    }
    
    inline vec2 norm () const {
        float l = sqrtf(lengthSq());
        return vec2(x/l, y/l);
    }
    
    inline float lengthSq() const {
        return x * x + y * y;
    }
};

inline vec2 operator + (const vec2& a, const vec2& b) {
    return vec2(a.x + b.x, a.y + b.y);
}

inline vec2 operator - (const vec2& a, const vec2& b) {
    return vec2(a.x - b.x, a.y - b.y);
}

inline vec2 operator * (float a, const vec2& b) {
    return vec2(a * b.x, a * b.y);
}

inline vec2 operator * (const vec2& a, const vec2& b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

inline float dot (const vec2& a, const vec2& b) {
    return a.x * b.x + a.y * b.y;
}

inline int fstr(const char* file_name, std::string* str) {
    std::ifstream file;
    file.open(file_name);
    
    if(!file.is_open()) {
        printf("%s cannot be opened \n", file_name);
        return -1;
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    str->assign(ss.str());
    return 0;
}

#endif /* common_h */
