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

#define body_center_force 6.0f

#define body_arm_force 8.0f

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
            
    BodyDef();
};

class Body : public Obj
{
    
public:
    
    /// body velocity, stick position, stick velocity, stick normal, stick angularVel, radius, stick length, stick radius, density, stick density
    /// '' -> target
    static const int single_input = 14;
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
    
    const Body* target;
    
    int hits;
    
    Body(const BodyDef* def);
    
    inline float getHealthRatio() const {
        return health/maxHealth;
    }
    
    void think(float dt);
    
    inline void stepBrain(float dt) {
        if(target != NULL)
            think(dt);
        else {
            velocity -= dt * body_center_force * position.norm();
        }
    }
    
    inline float absArmLength() const {
        return radius * (armLength + 1.0f);
    }
    
    void step(float dt);
    
    void applyImpulse(const vec2& world, const vec2& imp);
    
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
    
    inline static bool to_best (const Body* a, const Body* b) {
        return a->hits < b->hits;
    }
    
};

#endif /* Body_hpp */
