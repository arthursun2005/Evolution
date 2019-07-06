//
//  World.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "World.hpp"

Body* World::createBody(const BodyDef* def) {
    Body* body = new Body(def);
    body->node = tree.createProxy(body->aabb(), body);
    body->stick.node = tree.createProxy(body->stick.aabb(), &body->stick);
    bodies.push_back(body);
    return body;
}

void World::destoryBody(Body* body) {
    iterator_type begin = bodies.begin();
    iterator_type end = bodies.end();
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
    vec2 j = B->normal.I();
    vec2 _l = vec2(B->length * 0.5f, 0.0f);
    vec2 p1 = _l * j;
    vec2 p2 = B->position - p1;
    p1 += B->position;

    solveCircleLine(A, B, A->position, dt);
    
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

    solveCircleLine(A, B, p1, dt);
    solveCircleLine(A, B, p2, dt);
    
    solveCircleLine(B, A, p3, dt);
    solveCircleLine(B, A, p4, dt);
    
    solveCircleCircle(A, B, p1, p3, dt);
    solveCircleCircle(A, B, p1, p4, dt);
    
    solveCircleCircle(A, B, p2, p3, dt);
    solveCircleCircle(A, B, p2, p4, dt);
}

void World::solveCircleCircle(Obj *A, Obj *B, const vec2 &p1, const vec2 &p2, float dt) {
    vec2 D = p2 - p1;
    float M = D.lengthSq();
    float t = A->radius + B->radius;
    
    if(M < (t * t)) {
        float total = A->mass() + B->mass();
        
        Manifold m(total, dt);
        
        M = D.length();
        vec2 normal = D / M;
        
        m.force = t - M;
        
        m.obj1 = A;
        m.obj2 = B;
        
        m.normal = normal;
        m.point = 0.5f * (p1 + p2);
        
        float I = m.solve();
        
        if(B->type == Obj::e_stick && A->type == Obj::e_body) {
            ((Body*)A)->health -= I;
        }
    }
}

void World::solveCircleLine(Obj* A, Stick* B, const vec2& p1, float dt) {
    vec2 normal = B->normal;
    
    vec2 Q = normal.T().I();
    
    vec2 pT = p1 * Q;
    vec2 bT = B->position * Q;
    
    float r = A->radius + B->radius;
    
    float dy = fabs(bT.y - pT.y);
    
    if(dy < r) {
        if(fabs(bT.x - pT.x) < B->length * 0.5f) {
            float total = A->mass() + B->mass();
            
            Manifold m(total, dt);
            
            m.obj1 = A;
            m.obj2 = B;
            
            vec2 n, p;
            vec2 QT = Q.T();
            
            float depth = r - dy;
            
            if(pT.y > bT.y) {
                n = vec2(0.0f, -1.0f);
                p = vec2(pT.x, bT.y + B->radius);
            }else{
                n = vec2(0.0f, 1.0f);
                p = vec2(pT.x, bT.y - B->radius);
            }
            
            m.normal = n * QT;
            m.point = p * QT;
            m.force = depth;
            
            m.solve();
        }
    }
}

void World::step(float dt) {
    moveProxies(dt);
    
    getContacts();
    
    solveContacts(dt);
    
    iterator_type begin = bodies.begin();
    while(begin != bodies.end()) {
        Body* body = *begin;
        
        body->step(dt);
        body->constrain(aabb);
        
        if(body->health <= 0.0f) {
            iterator_type it = begin;
            ++begin;
            destoryBody(it);
            continue;
        }
        
        ++begin;
    }
}
