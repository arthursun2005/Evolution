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

#define max_translation 1.0f

#define max_translation_squared (max_translation * max_translation)

#define max_rotation (0.5f * M_PI)

#define max_rotation_squared (max_rotation * max_rotation)

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
    
    inline float invMass() const {
        return 1.0f / mass();
    }
};

inline void constrain(vec2* A, float V2) {
    float v2 = A->lengthSq();
    if(v2 > V2) (*A) *= sqrtf(V2/v2);
}

inline void constrain(vec2* position, vec2* velocity, const AABB& aabb) {
    if(position->x < aabb.lowerBound.x) {
        position->x = aabb.lowerBound.x;
        velocity->x = -0.5f * velocity->x;
    }
    
    if(position->y < aabb.lowerBound.y) {
        position->y = aabb.lowerBound.y;
        velocity->y = -0.5f * velocity->y;
    }
    
    if(position->x > aabb.upperBound.x) {
        position->x = aabb.upperBound.x;
        velocity->x = -0.5f * velocity->x;
    }
    
    if(position->y > aabb.upperBound.y) {
        position->y = aabb.upperBound.y;
        velocity->y = -0.5f * velocity->y;
    }
}

class Stick;

class Body;

class World;

#endif /* Obj_h */
