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
    static constexpr float impulse = 0.25f;
    
    Obj* obj1;
    Obj* obj2;
    
    vec2 normal;
    vec2 point;
    float force;
    
    inline void solve() {
        obj1->applyImpulse(point, -force * impulse * normal);
        obj2->applyImpulse(point, force * impulse * normal);
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
    
    void solveCircleLine(Obj* A, Stick* B, const vec2& p1, float dt);
    
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
    
public:
    
    const int width;
    const int height;
    
    const AABB aabb;
    
    int maxBodies;
    
    World(int width, int height) : width(width), height(height), bodies(NULL), aabb(vec2(-0.5f * width, -0.5f * height), vec2(0.5f * width, 0.5f * height)) {
        maxBodies = 512;
    }
    
    ~World() {
        for(Body* body : bodies) {
            delete(body);
        }
    }
    
    void generate(BodyDef def) {
        float stride = def.radius * 8.0f;
        for(float x = aabb.lowerBound.x + stride; x < aabb.upperBound.x; x += stride) {
            for(float y = aabb.lowerBound.y + stride; y < aabb.upperBound.y; y += stride) {
                if(bodies.size() >= maxBodies) return;
                def.position = vec2(x, y);
                createBody(&def);
            }
        }
    }
    
    void alter() {
        BodyDef def;
        iterator_type begin = bodies.begin();
        while(begin != bodies.end()) {
            if(bodies.size() >= maxBodies) return;
            def.position = vec2(randf(aabb.lowerBound.x, aabb.upperBound.x), randf(aabb.lowerBound.y, aabb.upperBound.y));
            Body* body = createBody(&def);
            Brain::alter(body->brain, (*begin)->brain);
        }
    }
    
    void clear() {
        iterator_type begin = bodies.begin();
        
        while(begin != bodies.end()) {
            iterator_type it = begin;
            ++begin;
            destoryBody(it);
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
