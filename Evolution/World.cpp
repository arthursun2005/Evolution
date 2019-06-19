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
    float M = dot(D, D);
    float t = A->radius + B->radius;
    
    float massA = A->area() * A->density;
    float massB = B->area() * B->density;
    float total = massA + massB;
    
    if(M < (t * t)) {
        M = sqrtf(M);
        vec2 normal = D.norm();
        float depth = 1.0f - M/t;

        float fA = point_impulse * depth * massB / total;
        float fB = point_impulse * depth * massA / total;
        
        A->velocity -= fA * normal;
        B->velocity += fB * normal;
    }
}

void World::solveBodyStick(Body *A, Stick *B, float dt) {
    vec2 Q = B->normal.I().T();
    vec2 pT = A->position * Q;
    float dy = fabs(B->position.y - pT.y);
    float r = A->radius + B->radius;
    float l2 = B->length * 0.5f;
    
    float massA = A->area() * A->density;
    float massB = B->area() * B->density;
    float total = massA + massB;
    
    if(dy < r) {
        if(pT.x > -l2 + B->position.x && pT.x < l2 + B->position.x) {
            float depth = 1.0f - dy/r;
            
            float fA = point_impulse * depth * massB / total;
            float fB = point_impulse * depth * massA / total;
            
            //A->velocity -= fA * Q;
            //B->velocity += fB * Q;
        }
    }
}

void World::solveStickStick(Stick *A, Stick *B, float dt) {
    if(A == B) return;
}
