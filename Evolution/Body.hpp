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

#define Body_InputSize 256

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
    
    int maxHealth;
    
    BodyDef() {
        damping = 0.99f;
        
        brain_inputs = 1;
        brain_outputs = 1;
        
        stick.radius = 0.25f;
        stick.length = 1.0f;
        
        radius = 1.0f;
        
        maxHealth = 65536;
    }
};

class Body
{
    
    int maxHealth;
    int health;
    
    Brain* brain;
    
    vec2 velocity;
    vec2 position;
    
    float damping;
    
    float radius;
    
    Stick stick;
    
    friend class World;
    
public:
    
    Body(const BodyDef* def) {
        velocity = def->velocity;
        position = def->position;
        damping = def->damping;
        
        stick = def->stick;
        stick.owner = this;
        
        radius = def->radius;
        
        health = maxHealth = def->maxHealth;
        
        brain = new Brain(Body_InputSize, Body_OutputSize);
    }
    
    ~Body() {
        delete brain;
        brain = NULL;
    }
    
    void think() {
    }
    
    void step(float dt) {
        think();
        
        stick.step(dt);
        
        velocity *= damping;
        position += dt * velocity;
    }
    
};

#endif /* Body_hpp */
