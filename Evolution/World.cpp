//
//  World.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "World.hpp"

Body* World::createBody(const BodyDef* def) {
    Body* body = new Body(def, this);
    body->node = tree.createProxy(body->aabb(), body);
    body->stick.node = tree.createProxy(body->stick.aabb(), &body->stick);
    bodies.push_back(body);
    return body;
}

void World::destoryBody(Body* body) {
    std::list<Body*>::iterator begin = bodies.begin();
    std::list<Body*>::iterator end = bodies.end();
    while(begin != end) {
        if((*begin) == body) {
            destoryBody(begin);
            return;
        }
    }
}

void World::solveContacts(float dt) {
    for(Contact& contact : contacts) {
        Obj* obj1 = (Obj*)contact.obj1;
        Obj* obj2 = (Obj*)contact.obj2;
        
        int type1 = obj1->type;
        int type2 = obj2->type;
        
        if(type1 == Obj::e_body) {
            if(type2 == Obj::e_body) {
                solveBodyBody((Body*)obj1, (Body*)obj2, dt);
            }else{
                solveBodyStick((Body*)obj1, (Stick*)obj2, dt);
            }
        }else{
            if(type2 == Obj::e_body) {
                solveBodyStick((Body*)obj2, (Stick*)obj1, dt);
            }else{
                solveStickStick((Stick*)obj1, (Stick*)obj2, dt);
            }
        }
    }
}

void World::solveBodyBody(Body *A, Body *B, float dt) {    
    solveCircleCircle(A, B, A->position, B->position, dt);
}

void World::solveBodyStick(Body *A, Stick *B, float dt) {
    solveCircleLine(A, B, A->position, B->position, dt);
    
    vec2 j = B->normal.I();
    vec2 _l = vec2(B->length * 0.5f, 0.0f);
    vec2 p1 = _l * j;
    vec2 p2 = B->position - p1;
    p1 += B->position;
    
    solveCircleCircle(A, B, A->position, p1, dt);
    solveCircleCircle(A, B, A->position, p2, dt);
}

void World::solveStickStick(Stick *A, Stick *B, float dt) {
    vec2 j, _l;
    
    j = A->normal.I();
    _l = vec2(A->length * 0.5f, 0.0f);
    vec2 p1 = _l * j;
    vec2 p2 = A->position - p1;
    p1 += A->position;
    
    j = B->normal.I();
    _l = vec2(B->length * 0.5f, 0.0f);
    vec2 p3 = _l * j;
    vec2 p4 = B->position - p3;
    p3 += B->position;
    
    solveCircleLine(A, B, p1, B->position, dt);
    solveCircleLine(A, B, p2, B->position, dt);
    
    solveCircleLine(B, A, p3, A->position, dt);
    solveCircleLine(B, A, p4, A->position, dt);
    
    
    solveCircleCircle(A, B, p1, p3, dt);
    solveCircleCircle(A, B, p1, p4, dt);
    
    solveCircleCircle(A, B, p2, p3, dt);
    solveCircleCircle(A, B, p2, p4, dt);
}

void World::solveCircleCircle(Obj* A, Obj* B, const vec2& p1, const vec2& p2, float dt) {
    vec2 D = p2 - p1;
    float M = D.lengthSq();
    float t = A->radius + B->radius;
    
    float total = A->mass() + B->mass();
    
    if(M < (t * t)) {
        Manifold m;
        
        M = D.length();
        vec2 normal = D / M;
        float depth = t - M;
        
        m.force = total * depth / dt;
        
        m.obj1 = A;
        m.obj2 = B;
        
        m.normal = normal;
        m.point = 0.5f * (p1 + p2);
        
        m.solve();
    }
}

void World::solveCircleLine(Obj* A, Stick* B, const vec2& p1, const vec2& p2, float dt) {
    vec2 normal = B->normal;
    
    vec2 Q = normal.T().I();
    
    vec2 pT = p1 * Q;
    vec2 bT = p2 * Q;
    
    vec2 dT = bT - pT;
    
    float r = A->radius + B->radius;
    
    float total = A->mass() + B->mass();
    
    float dy = fabs(dT.y);
    
    if(dy < r) {
        Manifold m;
        
        m.obj1 = A;
        m.obj2 = B;
        
        if(fabs(dT.x) < B->length * 0.5f) {
            vec2 n;
            
            float depth = r - dy;
            
            if(pT.y > bT.y) {
                n = normal;
            }else{
                n = -normal;
            }
            
            m.force = total * depth / dt;
            m.normal = n;
            
            vec2 p = vec2(pT.x, 0.5f * (pT.y + bT.y + B->radius - A->radius));
            
            m.point = p * Q.T();
            
            m.solve();
        }
    }
}

void Body::think(float dt) {
    Neuron* in = brain->inputs();

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
