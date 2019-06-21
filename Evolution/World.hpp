//
//  World.hpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef World_hpp
#define World_hpp

#include "Body.hpp"
#include "DynamicTree.hpp"
#include <list>

struct Manifold
{
    Obj* obj1;
    Obj* obj2;
    
    vec2 normal;
    vec2 point;
    float force;
    
    inline void solve() {
        obj1->applyImpulse(point, -force * 0.5f * normal);
        obj2->applyImpulse(point, force * 0.5f * normal);
    }
};

class World
{
    
    std::list<Body*> bodies;
    
    DynamicTree tree;
    
    inline void destoryBody(const std::list<Body*>::iterator& it) {
        Body* body = *it;
        bodies.erase(it);
        delete(body);
    }
    
    std::vector<Contact> contacts;
    
    /// dynamics
    void solveContacts(float dt);
    
    void solveBodyBody(Body* A, Body* B, float dt);
    void solveBodyStick(Body* A, Stick* B, float dt);
    void solveStickStick(Stick* A, Stick* B, float dt);
    
    void solveCircleCircle(Obj* A, Obj* B, const vec2& p1, const vec2& p2, float dt) {
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
    
    void solveCircleLine(Obj* A, Stick* B, const vec2& p1, const vec2& p2, float dt) {
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
    
    inline void moveProxies() {
        for(Body* body : bodies) {
            tree.moveProxy(body->node, body->aabb());
            tree.moveProxy(body->stick.node, body->stick.aabb());
        }
    }
    
    void step(float dt) {
        moveProxies();
        
        getContacts();
        
        solveContacts(dt);
        
        for(Body* body : bodies) {
            body->step(dt);
            body->constrain(aabb());
        }
    }
    
public:
    
    const int width;
    const int height;
    
    World(int width, int height) : width(width), height(height), bodies(NULL) {
    }
    
    ~World() {
        for(Body* body : bodies) {
            delete(body);
        }
    }
    
    inline std::list<Body*>::iterator begin() {
        return bodies.begin();
    }
    
    inline std::list<Body*>::iterator end() {
        return bodies.end();
    }
    
    inline std::list<Body*>::const_iterator cbegin() const {
        return bodies.cbegin();
    }
    
    inline std::list<Body*>::const_iterator cend() const {
        return bodies.cend();
    }
    
    inline int size() const {
        return (int)bodies.size();
    }
    
    Body* createBody(const BodyDef* def);
    
    void destoryBody(Body* body);
    
    inline void getContacts() {
        contacts.clear();
        tree.query(&contacts);
    }
    
    inline AABB aabb() const {
        AABB aabb;
        aabb.lowerBound = vec2(-width * 0.5f, -height * 0.5f);
        aabb.upperBound = vec2(width * 0.5f, height * 0.5f);
        return aabb;
    }
    
    inline float getTreeQuality() const {
        return tree.getAreaRatio();
    }
    
    inline int getTreeMaxBalance() const {
        return tree.getMaxBalance();
    }
    
    inline void step(float dt, int its) {
        dt /= (float) its;
        for(int i = 0; i < its; ++i)
            step(dt);
    }
};

#endif /* World_hpp */
