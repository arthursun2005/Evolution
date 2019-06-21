//
//  Body.hpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Body_hpp
#define Body_hpp

#include "Stick.h"
#include "Brain.h"

static Colorf health_color(0.0f, 1.0f, 0.2f);

struct BodyDef
{
    vec2 velocity;
    vec2 position;
    
    float damping;
    
    float radius;
    float density;
    
    Stick stick;
    
    int brain_inputs;
    int brain_outputs;
    
    float maxHealth;
    
    float maxStickForce;
    float maxForce;
    
    float armLength;
    
    Colorf color;
    
    int viewDiameter;
    
    BodyDef();
};

class Body : public Obj
{
    
public:
    
    /// body force, stick force, stick force local position
    static const int output_size = 6;
    
    int viewDiameter;
    
    float maxStickForce;
    float maxForce;
    
    float maxHealth;
    float health;
    
    float armLength;
    
    float wound;
    
    Brain* brain;
    
    float damping;
    
    Colorf color;
    
    Stick stick;
    
    World* world;
    
    Body(const BodyDef* def, World* world);
    
    inline ~Body() {
        delete(brain);
    }
    
    inline float getHealthRatio() const {
        return health/maxHealth;
    }
    
    void think(float dt);
    
    void step(float dt) {
        think(dt);
        
        ::constrain(&velocity, max_translation_squared / (dt * dt));
        
        float arm = radius * (armLength + 1.0f);
        vec2 diff = stick.position - position;
        float c2 = arm * arm;
        
        float v2 = diff.lengthSq();
        if(v2 > c2) {
            diff *= sqrtf(c2/v2);
            stick.velocity = velocity;
        }
        
        stick.position = position + diff;
        
        stick.step(dt);
        
        velocity *= powf(damping, dt);
        position += dt * velocity;
        
        float a = 0.2f;
        health -= wound * a;
        wound *= (1.0f - a);
    }
    
    void applyImpulse(const vec2& world, const vec2& imp) {
        float invMass = 1.0f / (area() * density);
        vec2 d = (world - position).norm();
        d = vec2(fabs(d.x), fabs(d.y));
        vec2 accel = invMass * scl(d, imp);
        velocity += accel;
        wound += accel.length();
    }
    
    inline AABB aabb() const {
        vec2 ext = vec2(radius, radius);
        return AABB(position - ext, position + ext);
    }
    
    inline float area() const {
        return radius * radius * M_PI;
    }
    
    inline void constrain(const AABB& aabb) {
        ::constrain(&position, &velocity, aabb);
        ::constrain(&stick.position, &stick.velocity, aabb);
    }
    
};

#endif /* Body_hpp */
