//
//  color.h
//  Evolution
//
//  Created by Arthur Sun on 6/15/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef color_h
#define color_h

#include <unistd.h>

struct Colorf
{
    float r;
    float g;
    float b;
    float a;
    
    Colorf() {}
    
    Colorf(float a) : r(a), g(a), b(a), a(1.0f) {}
    
    Colorf(float q, float a) : r(q), g(q), b(q), a(a) {}
    
    Colorf(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
    
    Colorf(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    
    inline uint32_t hex() const {
        int _r = r * 256.0f + 0.5f;
        int _g = g * 256.0f + 0.5f;
        int _b = b * 256.0f + 0.5f;
        int _a = a * 256.0f + 0.5f;
        return (_r << 24) | (_g << 16) | (_b << 8) | _a;
    }
    
    inline float compress() const {
        uint32_t _hex = hex();
        return *(float*)&_hex;
    }
};

#endif /* color_h */
