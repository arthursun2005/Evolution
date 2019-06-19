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

static Colorf health_color(0.0f, 1.0f, 0.2f);

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

class Body : public Obj
{
    
public:
    
    float maxHealth;
    float health;
    
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
    
    void think();
    
    void step(float dt) {
        think();
        
        stick.step(dt);
        
        velocity *= powf(damping, dt);
        position += dt * velocity;
    }
    
};

#endif /* Body_hpp */
