//
//  vec2.h
//  Evolution
//
//  Created by Arthur Sun on 6/15/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef vec2_h
#define vec2_h

#include <cmath>

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
    
    /// (x, -y)
    /// times -1
    inline vec2 T () const {
        return vec2(x, -y);
    }
    
    /// (y, -x)
    /// rotate pi/2
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

inline bool operator == (const vec2& a, const vec2& b) {
    return a.x == b.x && a.y == b.y;
}

inline float dot (const vec2& a, const vec2& b) {
    return a.x * b.x + a.y * b.y;
}

#endif /* vec2_h */
