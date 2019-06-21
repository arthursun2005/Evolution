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
        linearDamping = 0.33f;
        angularDamping = 0.2f;
        
        /// stick pointing up
        normal = vec2(1.0f, 0.0f);
        
        type = e_stick;
        density = 4.0f;
    }
    
    void step(float dt) {
        float ca2 = max_rotation_squared / (dt * dt);
        
        constrain(&velocity, max_translation_squared / (dt * dt));
        
        float a2 = angularVelocity * angularVelocity;
        if(a2 > ca2) {
            angularVelocity *= sqrtf(ca2/a2);
        }
        
        angularVelocity *= powf(angularDamping, dt);
        velocity *= powf(linearDamping, dt);
        
        float a = dt * angularVelocity;
        normal = normal * vec2(cosf(a), sinf(a));
        position += dt * velocity;
    }
    
    AABB aabb() const {
        vec2 p1 = vec2(0.0f, length * 0.5f) * normal;
        vec2 p2 = -p1;
        vec2 ext = vec2(radius, radius);
        vec2 min_p = min(p1, p2);
        vec2 max_p = max(p1, p2);
        return AABB(position + min_p - ext, position + max_p + ext);
    }
    
    inline float area() const {
        return radius * (2.0f * length + radius * M_PI);
    }
    
    void applyImpulse(const vec2& world, const vec2& imp) {
        float invMass = Obj::invMass();
        vec2 q = (world - position).norm();
        velocity += invMass * scl(vec2(fabs(q.x), fabs(q.y)), imp);
        angularVelocity -= (invMass * M_1_PI * 0.5f * dot(imp, q.I()));
    }
};

#endif /* Stick_h */
