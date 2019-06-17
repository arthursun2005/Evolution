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
#include "Obj.h"

struct Body;

static Colorf stick_color(0.3f, 0.2f, 0.0f);

class Stick : public Obj
{
    
public:
    
    Body* owner;
    
    float length;
    
    vec2 normal;
    
    float angularVelocity;
    
    float linearDamping;
    float angularDamping;
        
    Stick() : owner(NULL), angularVelocity(0.0f) {
        linearDamping = 0.99f;
        angularDamping = 0.99f;
        
        /// stick pointing up
        normal = vec2(1.0f, 0.0f);
        
        type = e_stick;
    }
    
    void step(float dt) {
        angularVelocity *= angularDamping;
        velocity *= linearDamping;
        
        float a = dt * angularVelocity;
        normal = normal * vec2(cosf(a), sinf(a));
        position += dt * velocity;
    }
    
    AABB aabb() const {
        vec2 rot = normal.I();
        vec2 p1 = vec2(length, 0.0f) * rot;
        vec2 p2 = -p1;
        vec2 ext = vec2(radius, radius);
        vec2 min_p = min(p1, p2);
        vec2 max_p = max(p1, p2);
        return AABB(position + min_p - ext, position + max_p + ext);
    }
};

#endif /* Stick_h */
