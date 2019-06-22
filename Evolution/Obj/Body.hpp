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
    
    float maxHealth;
    
    float maxStickForce;
    float maxForce;
    
    float armLength;
    
    Colorf color;
    
    float centerForce;
        
    BodyDef();
};

class Body : public Obj
{
    
public:
    
    /// body velocity, stick position, stick velocity, stick normal, stick angularVel
    /// '' -> target
    static const int single_input = 9;
    static const int input_size = 2 * single_input + 2;
    
    /// body force, stick force, stick force local position
    static const int output_size = 6;
    
    float maxStickForce;
    float maxForce;
    
    float maxHealth;
    float health;
    
    float armLength;
    
    float wound;
    
    Brain brain;
    
    float damping;
    
    Colorf color;
    
    Stick stick;
    
    World* world;
    
    const Body* target;
    
    int hits;
    
    Body(const BodyDef* def, World* world);
    
    inline float getHealthRatio() const {
        return health/maxHealth;
    }
    
    void think(float dt);
    
    inline void stepBrain(float dt) {
        if(target != NULL)
            think(dt);
        else {
            velocity -= dt * position.norm();
        }
    }
    
    void step(float dt) {
        ::constrain(&velocity, max_translation_squared / (dt * dt));
        
        float arm = radius * (armLength + 1.0f);
        vec2 diff = stick.position - position;
        float c2 = arm * arm;
        
        float v2 = diff.lengthSq();
        if(v2 > c2)
            stick.applyImpulse(position, (sqrt(v2/c2) - 1.0f) * (position - stick.position));
        
        stick.step(dt);
        
        velocity *= powf(damping, dt);
        position += dt * velocity;
        
        float a = 0.005f;
        health -= wound * a;
        wound *= (1.0f - a);
    }
    
    void applyImpulse(const vec2& world, const vec2& imp) {
        float invMass = Obj::invMass();
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
    
    inline AABB box(float _radius) const {
        vec2 ext = vec2(_radius * radius, _radius * radius);
        return AABB(position - ext, position + ext);
    }
    
    inline float area() const {
        return radius * radius * M_PI;
    }
    
    inline void constrain(const AABB& aabb) {
        ::constrain(&position, &velocity, aabb);
        ::constrain(&stick.position, &stick.velocity, aabb);
    }
    
    void setInputs(Neuron* in) const;
    
    void setInputs();
    
};

#endif /* Body_hpp */
