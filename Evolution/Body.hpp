//
//  Body.hpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Body_hpp
#define Body_hpp

#include "color.h"
#include "Stick.h"
#include "Brain.h"

/// body force, stick force, stick force local position
#define Body_OutputSize 6

struct BodyDef
{
    vec2 velocity;
    vec2 position;
    
    float damping;
    
    float radius;
    
    Stick stick;
    
    int brain_inputs;
    int brain_outputs;
    
    float maxHealth;
    
    Colorf color;
    
    int viewDiameter;
    
    BodyDef();
};

class Body
{
    
    float maxHealth;
    float health;
    
    Brain* brain;
    
    vec2 velocity;
    vec2 position;
    
    float damping;
    
    float radius;
        
    friend class World;
    
public:
    
    Colorf color;
    
    Stick stick;
    
    Body(const BodyDef* def);
    
    ~Body() {
        delete(brain);
    }
    
    inline vec2 getPosition() const {
        return position;
    }
    
    inline vec2 getVelocity() const {
        return velocity;
    }
    
    inline float getRadius() const {
        return radius;
    }
    
    void think();
    
    void step(float dt) {
        think();
        
        stick.step(dt);
        
        velocity *= damping;
        position += dt * velocity;
    }
    
};

#endif /* Body_hpp */
