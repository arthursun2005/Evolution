//
//  World.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "World.hpp"

#define point_impulse 8.0f

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
            tree.destoryProxy((*begin)->node);
            tree.destoryProxy((*begin)->stick.node);
            destoryBody(begin);
            return;
        }
    }
}

void Body::think() {
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
    if(A == B) return;
    
    vec2 D = B->position - A->position;
    float M = D.lengthSq();
    float t = A->radius + B->radius;
    
    float massA = A->area() * A->density;
    float massB = B->area() * B->density;
    float total = massA + massB;
    
    if(M < (t * t)) {
        M = D.length();
        vec2 normal = D / M;
        float depth = t - M;
        
        float fA = point_impulse * depth * massB / total;
        float fB = point_impulse * depth * massA / total;
        
        A->velocity -= fA * normal;
        B->velocity += fB * normal;
    }
}

void World::solveBodyStick(Body *A, Stick *B, float dt) {
    vec2 Q = B->normal.T().I();
    
    vec2 pT = A->position * Q;
    vec2 bT = B->position * Q;
    
    vec2 dT = bT - pT;
    
    float r = A->radius + B->radius;
    
    float massA = A->area() * A->density;
    float massB = B->area() * B->density;
    float total = massA + massB;
    
    float dy = fabs(dT.y);
    
    if(dy < r) {
        if(fabs(dT.x) < B->length * 0.5f) {
            vec2 normal;
            float rot;
            
            float depth = r - dy;
            
            if(pT.y > bT.y) {
                normal = B->normal;
                rot = massA / total * depth * dT.x;
            }else{
                normal = -B->normal;
                rot = -massA / total * depth * dT.x;
            }
            
            float fA = point_impulse * depth * massB / total;
            float fB = point_impulse * depth * massA / total;
            
            A->velocity -= fA * normal;
            B->velocity += fB * normal;
            B->angularVelocity += rot;
        }else{
            vec2 j = B->normal.I();
            vec2 _l = vec2(B->length * 0.5f, 0.0f);
            vec2 p1 = _l * j;
            vec2 p2 = B->position - p1;
            p1 += B->position;
            
            vec2 D1 = p1 - A->position;
            vec2 D2 = p2 - A->position;
            
            float M1 = D1.lengthSq();
            float M2 = D2.lengthSq();
            
            if(M1 < (r * r)) {
                M1 = D1.length();
                vec2 normal = D1 / M1;
                float depth = r - M1;
                
                float fA = depth * massB / total;
                float fB = depth * massA / total;
                
                float k = dot(normal, B->normal);
                float rot = k * massA / total * depth * dT.x;
                
                A->velocity -= fA * normal;
                B->velocity += fB * normal;
                B->angularVelocity += rot;
            }
            
            if(M2 < (r * r)) {
                M2 = D2.length();
                vec2 normal = D2 / M2;
                float depth = r - M2;
                
                float fA = depth * massB / total;
                float fB = depth * massA / total;
                
                float k = dot(normal, B->normal);
                float rot = k * massA / total * depth * dT.x;
                
                A->velocity -= fA * normal;
                B->velocity += fB * normal;
                B->angularVelocity += rot;
            }
        }
    }
}

void World::solveStickStick(Stick *A, Stick *B, float dt) {
    if(A == B) return;
}
