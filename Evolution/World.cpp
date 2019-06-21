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
