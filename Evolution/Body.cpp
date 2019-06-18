//
//  Body.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Body.hpp"

BodyDef::BodyDef() {
    damping = 0.994f;
    
    brain_inputs = 1;
    brain_outputs = 1;
    
    radius = 1.0f;
    
    stick.radius = 0.125f;
    stick.length = 3.0f;
    stick.position = vec2(radius + 1.5f * stick.radius, 0.0f);
    stick.velocity = vec2(0.0f, 0.0f);
    
    maxHealth = 65536.0f;
    
    color = Colorf(0.0f);
    
    position = vec2(0.0f, 0.0f);
    velocity = vec2(0.0f, 0.0f);
    
    viewDiameter = 16;
}

Body::Body(const BodyDef* def, World* world) {
    position = def->position;
    velocity = def->velocity;
    
    damping = def->damping;
    
    stick = def->stick;
    
    stick.position += position;
    stick.velocity += velocity;
    
    stick.owner = this;
    
    radius = def->radius;
    
    health = maxHealth = def->maxHealth;
    
    color = def->color;
    
    brain = new Brain(def->viewDiameter * def->viewDiameter, Body_OutputSize);
    
    type = e_body;
    
    this->world = world;
}
