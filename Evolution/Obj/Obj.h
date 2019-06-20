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
#include "Collision.h"
#include "color.h"

class Obj
{
    
public:
    
    int type;
    
    vec2 position;
    vec2 velocity;
    
    float radius;
    
    float density;
    
    int node;
    
    enum type
    {
        e_body,
        e_stick
    };
    
    virtual void applyImpulse(const vec2&, const vec2&) = 0;
    
    virtual AABB aabb() const = 0;
    
    virtual float area() const = 0;
    
    inline float mass() const {
        return area() * density;
    }
};

class Stick;

class Body;

class World;

#endif /* Obj_h */
