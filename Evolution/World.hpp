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
    
    typedef std::list<Body*>::iterator iterator_type;
    
    typedef std::list<Body*>::const_iterator const_iterator_type;
    
    std::list<Body*> bodies;
    
    DynamicTree tree;
    
    inline void destoryBody(const iterator_type& it) {
        Body* body = *it;
        bodies.erase(it);
        tree.destoryProxy(body->node);
        tree.destoryProxy(body->stick.node);
        delete(body);
    }
    
    std::vector<Contact> contacts;
    
    /// dynamics
    void solveContacts(float dt);
    
    void solveBodyBody(Body* A, Body* B, float dt);
    void solveBodyStick(Body* A, Stick* B, float dt);
    void solveStickStick(Stick* A, Stick* B, float dt);
    
    void solveCircleCircle(Obj* A, Obj* B, const vec2& p1, const vec2& p2, float dt);
    
    void solveCircleLine(Obj* A, Stick* B, const vec2& p1, const vec2& p2, float dt);
    
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
        
        iterator_type begin = bodies.begin();
        while(begin != bodies.end()) {
            Body* body = *begin;
            
            body->step(dt);
            body->constrain(aabb());
            
            if(body->health <= 0.0f) {
                iterator_type it = begin;
                ++begin;
                destoryBody(it);
                continue;
            }
            
            ++begin;
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
    
    inline iterator_type begin() {
        return bodies.begin();
    }
    
    inline iterator_type end() {
        return bodies.end();
    }
    
    inline const_iterator_type cbegin() const {
        return bodies.cbegin();
    }
    
    inline const_iterator_type cend() const {
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
