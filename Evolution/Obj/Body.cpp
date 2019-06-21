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
    
    maxStickForce = 40.0f;
    maxForce = 60.0f;
    
    armLength = 3.0f;
}

Body::Body(const BodyDef* def, World* world) : brain(input_size, output_size) {
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
    in[0].value = velocity.x;
    in[1].value = velocity.y;
    in[2].value = stick.position.x - position.x;
    in[3].value = stick.position.y - position.y;
    in[4].value = stick.velocity.x;
    in[5].value = stick.velocity.y;
    in[6].value = stick.normal.x;
    in[7].value = stick.normal.y;
    in[8].value = stick.angularVelocity;
}

void Body::setInputs() {
    if(target != NULL) {
        Neuron* in = brain.inputs();
        setInputs(in);
        target->setInputs(in + single_input);
        in[input_size - 2].value = target->position.x - position.x;
        in[input_size - 1].value = target->position.y - position.y;
    }
}

void Body::think(float dt) {
    brain.compute();
    
    Neuron* out = brain.outputs();
    
    vec2 force = vec2(out[0].value, out[1].value);
    vec2 stick = vec2(out[2].value, out[3].value);
    vec2 local = vec2(out[4].value, out[5].value);
    
    float sf = maxForce + maxStickForce;
    
    ::constrain(&force, maxForce * maxForce);
    ::constrain(&stick, sf * sf);
    
    float arm = radius * (armLength + 1.0f);
    ::constrain(&local, arm * arm);
    
    velocity += dt * force;
    stick *= this->stick.mass();
    this->stick.applyImpulse(position + local, dt * stick);
}
