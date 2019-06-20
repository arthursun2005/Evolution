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
                //solveBodyStick((Body*)obj1, (Stick*)obj2, dt);
            }
        }else{
            if(type2 == Obj::e_body) {
                //solveBodyStick((Body*)obj2, (Stick*)obj1, dt);
            }else{
                solveStickStick((Stick*)obj1, (Stick*)obj2, dt);
            }
        }
    }
}

void World::solveBodyBody(Body *A, Body *B, float dt) {    
    vec2 D = B->position - A->position;
    float M = D.lengthSq();
    float t = A->radius + B->radius;
    
    float total = A->mass() + B->mass();
    
    if(M < (t * t)) {
        Manifold m;
        
        M = D.length();
        vec2 normal = D / M;
        float depth = 1.0f - M/t;
        
        m.force = total * depth;

        m.obj1 = A;
        m.obj2 = B;
        
        m.normal = normal;
        m.point = 0.5f * (A->position + B->position);
        
        m.solve();
    }
}

void World::solveBodyStick(Body *A, Stick *B, float dt) {
    vec2 Q = B->normal.T().I();
    
    vec2 pT = A->position * Q;
    vec2 bT = B->position * Q;
    
    vec2 dT = bT - pT;
    
    float r = A->radius + B->radius;
    
    float total = A->mass() + B->mass();
    
    float dy = fabs(dT.y);
    
    if(dy < r) {
        Manifold m;
        
        m.obj1 = A;
        m.obj2 = B;
        
        if(fabs(dT.x) < B->length * 0.5f) {
            vec2 normal;
            
            float depth = r - dy;
            
            if(pT.y > bT.y) {
                normal = B->normal;
            }else{
                normal = -B->normal;
            }
            
            m.force = total * depth;
            m.normal = normal;
            
            vec2 p = vec2(pT.x, 0.5f * (pT.y + bT.y + B->radius - A->radius));
            
            m.point = p * Q.T();
            
            m.solve();
        }else{
        }
    }
}

void World::solveStickStick(Stick *A, Stick *B, float dt) {
}
