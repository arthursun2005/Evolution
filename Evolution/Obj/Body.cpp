//
//  Body.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Body.hpp"

BodyDef::BodyDef() {
    damping = 0.33f;
    
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
    maxForce = 20.0f;
    
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
    
    target = NULL;
}

void Body::setInputs(Neuron *in) const {
    in[0].value = position.x;
    in[1].value = position.y;
    in[2].value = velocity.x;
    in[3].value = velocity.y;
    in[4].value = stick.position.x;
    in[5].value = stick.position.y;
    in[6].value = stick.velocity.x;
    in[7].value = stick.velocity.y;
    in[8].value = stick.normal.x;
    in[9].value = stick.normal.y;
    in[10].value = stick.angularVelocity;
}

void Body::setInputs() {
    if(target != NULL) {
        Neuron* in = brain->inputs();
        setInputs(in);
        target->setInputs(in + single_input);
    }
}

void Body::think(float dt) {
    brain->compute();
    
    Neuron* out = brain->outputs();
    
    vec2 force = vec2(out[0].value, out[1].value);
    vec2 stick = vec2(out[2].value, out[3].value);
    vec2 local = vec2(out[4].value, out[5].value);
    
    ::constrain(&force, maxForce * maxForce);
    ::constrain(&stick, maxStickForce * maxStickForce);
    
    float arm = radius * (armLength + 1.0f);
    ::constrain(&local, arm * arm);
    
    float invMass = 1.0f / (area() * density);
    
    velocity += dt * invMass * force;
    this->stick.applyImpulse(position + local, dt * stick);
}
