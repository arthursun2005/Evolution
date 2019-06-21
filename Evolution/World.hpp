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
    float impulse = 8.0f;
    
    Obj* obj1;
    Obj* obj2;
    
    vec2 normal;
    vec2 point;
    float force;
    
    inline void solve() {
        obj1->applyImpulse(point, -impulse * force * normal);
        obj2->applyImpulse(point, impulse * force * normal);
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
        
        NearestBody() {
            body = NULL;
            shortestLengthSq = FLT_MAX;
        }
        
        bool callback(void* data) {
            Obj* obj = (Obj*)data;
            
            if(obj->type == Obj::e_stick || obj != self)
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
    
    void solveBodyBody(Body* A, Body* B, float dt);
    void solveBodyStick(Body* A, Stick* B, float dt);
    void solveStickStick(Stick* A, Stick* B, float dt);
    
    void solveCircleCircle(Obj* A, Obj* B, const vec2& p1, const vec2& p2, float dt);
    
    void solveCircleLine(Obj* A, Stick* B, const vec2& p1, float dt);
    
    inline void moveProxies(float dt) {
        for(Body* body : bodies) {
            tree.moveProxy(body->node, body->aabb(), dt * body->velocity);
            tree.moveProxy(body->stick.node, body->stick.aabb(), dt * body->stick.velocity);
        }
    }
    
    void step(float dt);
    
    void brainInputs() {
        for(Body* body : bodies) {
            NearestBody collector;
            collector.self = body;
            AABB fatAABB = body->box(targetRadius);
            tree.query(&collector, fatAABB);
            body->target = collector.body;
            body->setInputs();
        }
    }
    
public:
    
    float targetRadius = 12.0f;
    
    const int width;
    const int height;
    
    const AABB aabb;
    
    int maxBodies;
    
    World(int width, int height) : width(width), height(height), bodies(NULL), aabb(vec2(-0.5f * width, -0.5f * height), vec2(0.5f * width, 0.5f * height)) {
        maxBodies = (width * height) / (targetRadius * targetRadius);
    }
    
    ~World() {
        for(Body* body : bodies) {
            delete(body);
        }
    }
    
    void generate(BodyDef def) {
        float stride = def.radius * targetRadius;
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
