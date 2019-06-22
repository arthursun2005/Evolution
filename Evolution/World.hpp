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
#include "Timer.h"
#include <list>

struct Manifold
{
    float step = 0.25f;
    
    Obj* obj1;
    Obj* obj2;
    
    vec2 normal;
    vec2 point;
    float force;
    
    void incHit(Obj* obj, Obj* obj2) {
        if(obj->type == Obj::e_stick) {
            if(obj2->type == Obj::e_body && ((Stick*)obj)->owner != obj2)
                ++((Stick*)obj)->owner->hits;
        }
    }
    
    void solve() {
        obj1->applyImpulse(point, -step * force * normal);
        obj2->applyImpulse(point, step * force * normal);
        
        incHit(obj1, obj2);
        incHit(obj2, obj1);
    }
};

class World
{
    
public:
    
    struct NearestBody
    {
        Body* body;
        Body* self;
        
        float shortestLengthSq;
        
        NearestBody(float radius) {
            body = NULL;
            shortestLengthSq = radius * radius;
        }
        
        bool callback(void* data) {
            Obj* obj = (Obj*)data;
            
            if(obj->type == Obj::e_stick || obj == self)
                return true;
            
            vec2 dir = obj->position - self->position;
            
            float lengthSq = dir.lengthSq();
            
            if(lengthSq < shortestLengthSq) {
                body = (Body*)obj;
                shortestLengthSq = lengthSq;
            }
            
            return true;
        }
    };
    
protected:
    
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
    
    static void solveBodyBody(Body* A, Body* B, float dt);
    static void solveBodyStick(Body* A, Stick* B, float dt);
    static void solveStickStick(Stick* A, Stick* B, float dt);
    
    static void solveCircleCircle(Obj* A, Obj* B, const vec2& p1, const vec2& p2, float dt);
    
    static void solveCircleLine(Obj* A, Stick* B, const vec2& p1, float dt);
    
    inline void moveProxies(float dt) {
        for(Body* body : bodies) {
            tree.moveProxy(body->node, body->aabb(), dt * body->velocity);
            tree.moveProxy(body->stick.node, body->stick.aabb(), dt * body->stick.velocity);
        }
    }
    
    void step(float dt);
    
    void brainInputs() {
        for(Body* body : bodies) {
            NearestBody collector(targetRadius);
            collector.self = body;
            AABB fatAABB = body->box(targetRadius);
            tree.query(&collector, fatAABB);
            body->target = collector.body;
            body->setInputs();
        }
    }
    
public:
    
    float targetRadius = 8.0f;
    
    float width;
    float height;
    
    AABB aabb;
    
    int maxBodies;
    
    World(float width, float height) : width(width), height(height), bodies(NULL), aabb(vec2(-0.5f * width, -0.5f * height), vec2(0.5f * width, 0.5f * height)) {
        maxBodies = (width * height) / (targetRadius * targetRadius) * 0.25f;
    }
    
    ~World() {
        for(Body* body : bodies) {
            delete(body);
        }
    }
    
    void generate(BodyDef def) {
        float stride = 2.0f * def.radius * targetRadius;
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
            Brain::alter(&body->brain, &(*begin)->brain);
        }
    }
    
    void wipe(int l) {
        iterator_type begin = bodies.begin();
        
        while(begin != bodies.end()) {
            if((*begin)->hits < l) {
                iterator_type it = begin;
                ++begin;
                destoryBody(it);
            }else{
                (*begin)->hits = 0;
                ++begin;
            }
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
    
    inline int getMaxBrainComplexity() const {
        int c = 0;
        
        for(Body* body : bodies)
            c = std::max(c, body->brain.totalSize());
        
        return c;
    }
    
    void step(float dt, int its) {
        brainInputs();
        
        for(Body* body : bodies)
            body->stepBrain(dt);
        
        dt /= (float) its;
        for(int i = 0; i < its; ++i)
            step(dt);
    }
};

#endif /* World_hpp */
