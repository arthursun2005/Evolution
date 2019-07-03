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
};

#endif /* color_h */
