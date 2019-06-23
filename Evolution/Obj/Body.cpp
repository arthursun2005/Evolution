//
//  Body.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Body.hpp"

BodyDef::BodyDef() {
    damping = 0.1f;
    
    radius = 1.0f;
    
    stick.radius = 0.15f;
    stick.length = 4.0f;
    stick.position = vec2(radius + 2.0f * stick.radius, 0.0f);
    stick.velocity = vec2(0.0f, 0.0f);
    
    maxHealth = 200.0f;
    
    color = Colorf(0.0f);
    
    position = vec2(0.0f, 0.0f);
    velocity = vec2(0.0f, 0.0f);
    
    density = 1.0f;
    
    maxStickForce = 48.0f;
    maxForce = 64.0f;
    
    armLength = 2.0f;
}

Body::Body(const BodyDef* def) : brain(input_size, output_size) {
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
    maxStickForce = def->maxStickForce;
    
    wound = 0.0f;
    
    armLength = def->armLength;
        
    target = NULL;
    
    hits = 0;
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
    in[9].value = radius;
    in[10].value = stick.length;
    in[11].value = stick.radius;
    in[12].value = density;
    in[13].value = stick.density;
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
    
    force *= maxForce;
    stick *= maxStickForce;
    local *= this->stick.length;
    
    ::constrain(&force, maxForce * maxForce);
    ::constrain(&stick, maxStickForce * maxStickForce);
    ::constrain(&local, this->stick.length * this->stick.length);
    
    velocity += dt * force;
    stick *= this->stick.mass();
    this->stick.applyImpulse(this->stick.position + local, dt * stick);
}

void Body::step(float dt) {
    ::constrain(&velocity, max_translation_squared / (dt * dt));
    
    float arm = absArmLength();
    float c2 = arm * arm;
    
    vec2 d = stick.position - position;
    vec2 n = d.norm();
    
    float m = d.lengthSq();
    float w = 1.0f - m/c2;
    float ms = mass();
    
    stick.applyImpulse(position, ms * dt * body_arm_force * w * n);
    applyImpulse(position, -ms * dt * body_arm_force * w * n);
    
    stick.step(dt);
    
    velocity *= powf(damping, dt);
    position += dt * velocity;
    
    health -= wound * dt;
    wound *= (1.0f - dt);
}

void Body::applyImpulse(const vec2& world, const vec2& imp) {
    float invMass = Obj::invMass();
    vec2 d = (world - position).norm();
    d = vec2(fabs(d.x), fabs(d.y));
    vec2 accel = invMass * scl(d, imp);
    velocity += accel;
    wound += accel.length();
}
