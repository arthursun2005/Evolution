//
//  Stick.h
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Stick_h
#define Stick_h

#include "vec2.h"
#include "common.h"

struct Body;

struct Stick
{
    Body* owner;
    
    float radius;
    float length;
    
    vec2 position;
    vec2 velocity;
    vec2 normal;
    
    float angularVelocity;
    
    float linearDamping;
    float angularDamping;
    
    Stick() : owner(NULL), angularVelocity(0.0f) {
        linearDamping = 0.99f;
        angularDamping = 0.99f;
        
        /// up
        normal = vec2(0.0f, 1.0f);
    }
    
    void step(float dt) {
        angularVelocity *= angularDamping;
        velocity *= linearDamping;
        
        float a = dt * angularVelocity;
        normal = normal * vec2(cosf(a), sinf(a));
        position += dt * velocity;
    }
};

#endif /* Stick_h */
