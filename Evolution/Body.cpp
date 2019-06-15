//
//  Body.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Body.hpp"

Body::Body(const BodyDef* def) {
    velocity = def->velocity;
    position = def->position;
    damping = def->damping;
    
    stick = def->stick;
    stick.owner = this;
    
    radius = def->radius;
    
    health = maxHealth = def->maxHealth;
    
    color = def->color;
    
    brain = new Brain(def->viewDiameter * def->viewDiameter, Body_OutputSize);
}

void Body::think() {
}
