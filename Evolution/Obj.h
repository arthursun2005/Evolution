//
//  Obj.h
//  Evolution
//
//  Created by Arthur Sun on 6/17/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Obj_h
#define Obj_h

#include "vec2.h"
#include "common.h"

class Obj
{
    
public:
    
    int type;
    
    vec2 position;
    vec2 velocity;
    
    float radius;
    
    int node;
    
    enum type
    {
        e_body,
        e_stick
    };
    
    AABB aabb() const {
        vec2 ext = vec2(radius, radius);
        return AABB(position - ext, position + ext);
    }
};

class Stick;

class Body;

class World;

#endif /* Obj_h */
