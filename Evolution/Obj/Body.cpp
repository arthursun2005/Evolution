//
//  Body.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Body.hpp"

BodyDef::BodyDef() {
    damping = 0.5f;
    
    radius = 1.0f;
    
    stick.radius = 0.15f;
    stick.length = 4.0f;
    stick.position = vec2(radius + 2.0f * stick.radius, 0.0f);
    stick.velocity = vec2(0.0f, 0.0f);
    
    maxHealth = 100.0f;
    
    color = Colorf(0.0f);
    
    position = vec2(0.0f, 0.0f);
    velocity = vec2(0.0f, 0.0f);
    
    density = 1.0f;
    
    maxStickForce = 20.0f;
    maxForce = 60.0f;
    
    armLength = 3.0f;
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
    
    brain = new Brain(input_size, output_size);
    
    type = e_body;
    density = def->density;
    
    maxForce = def->maxForce;
    maxStickForce = maxForce + def->maxStickForce;
    
    wound = 0.0f;
    
    armLength = def->armLength;
    
    this->world = world;
}

