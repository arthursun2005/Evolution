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
    solveCircleCircle(A, B, A->position, B->position, A->velocity, B->velocity, dt);
}

void World::solveBodyStick(Body *A, Stick *B, float dt) {
    vec2 j = B->normal.I();
    vec2 _l = vec2(B->length * 0.5f, 0.0f);
    vec2 p1 = _l * j;
    vec2 p2 = B->position - p1;
    p1 += B->position;
    
    vec2 v1 = B->velocity + 0.5f * B->length * B->angularVelocity * B->normal;
    vec2 v2 = B->velocity - 0.5f * B->length * B->angularVelocity * B->normal;
    
    solveCircleLine(A, B, A->position, A->velocity, dt);
    
    solveCircleCircle(A, B, A->position, p1, A->velocity, v1, dt);
    solveCircleCircle(A, B, A->position, p2, A->velocity, v2, dt);
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
    
    vec2 v1 = A->velocity + 0.5f * A->length * A->angularVelocity * A->normal;
    vec2 v2 = A->velocity - 0.5f * A->length * A->angularVelocity * A->normal;
    vec2 v3 = B->velocity + 0.5f * B->length * B->angularVelocity * B->normal;
    vec2 v4 = B->velocity - 0.5f * B->length * B->angularVelocity * B->normal;
    
    solveCircleLine(A, B, p1, v1, dt);
    solveCircleLine(A, B, p2, v2, dt);
    
    solveCircleLine(B, A, p3, v3, dt);
    solveCircleLine(B, A, p4, v4, dt);
    
    solveCircleCircle(A, B, p1, p3, v1, v3, dt);
    solveCircleCircle(A, B, p1, p4, v1, v4, dt);
    
    solveCircleCircle(A, B, p2, p3, v2, v3, dt);
    solveCircleCircle(A, B, p2, p4, v2, v4, dt);
}

void World::solveCircleCircle(Obj *A, Obj *B, const vec2 &p1, const vec2 &p2, const vec2 &v1, const vec2 &v2, float dt) {
    vec2 D = p2 - p1;
    float M = D.lengthSq();
    float t = A->radius + B->radius;
    
    if(M < (t * t)) {
        Manifold m;
        
        M = D.length();
        vec2 normal = D / M;
        float depth = t - M;
        float total = std::max(dot(v1, normal), 0.0f) * A->mass() + std::max(dot(-v2, normal), 0.0f) * B->mass();
        m.force = total * depth;
        
        m.obj1 = A;
        m.obj2 = B;
        
        m.normal = normal;
        m.point = 0.5f * (p1 + p2);
        
        m.solve();
    }
}

void World::solveCircleLine(Obj* A, Stick* B, const vec2& p1, const vec2& v1, float dt) {
    vec2 normal = B->normal;
    
    vec2 Q = normal.T().I();
    
    vec2 pT = p1 * Q;
    vec2 bT = B->position * Q;
    
    float r = A->radius + B->radius;
    
    float dx = (bT.x - pT.x);
    float dy = fabs(bT.y - pT.y);
    
    if(dy < r) {
        Manifold m;
        
        m.obj1 = A;
        m.obj2 = B;
        
        if(fabs(dx) < B->length * 0.5f) {
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
            
            vec2 vAt = B->velocity + (dx * vec2(0.0f, B->angularVelocity)) * QT;
            
            float total = std::max(dot(v1, m.normal), 0.0f) * A->mass() + std::max(dot(-vAt, m.normal), 0.0f) * B->mass();
            
            m.point = p * QT;
            m.force = 16.0f * total * depth;
            
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
