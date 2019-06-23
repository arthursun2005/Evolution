//
//  World.hpp
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef World_hpp
#define World_hpp

#include "BodySystem.h"

#define impulse_pressure 0.2f

struct Manifold
{    
    Obj* obj1;
    Obj* obj2;
    
    vec2 normal;
    vec2 point;
    
    float force;
    float mass;
    float impulse;
    float dt;
    
    inline Manifold(float totalMass, float dt) : mass(totalMass), impulse(impulse_pressure/dt), dt(dt) {}
    
    void addScore(Obj* obj1, Obj* obj2, float K) {
        if(obj1->type == Obj::e_body && obj2->type == Obj::e_stick) {
            Body* body1 = (Body*)obj1;
            Body* body2 = ((Stick*)obj2)->owner;
            if(body1 == body2) {
                body2->score -= K;
            }else{
                body2->score += K;
            }
        }
    }
    
    void solve() {
        float I = force * impulse * mass;
        
        obj1->applyImpulse(point, -I * normal);
        obj2->applyImpulse(point, I * normal);
        
        float R = I * dt;
        addScore(obj1, obj2, R);
        addScore(obj2, obj1, R);
    }
};

class World : public BodySystem
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
    
    static void solveBodyBody(Body* A, Body* B, float dt);
    static void solveBodyStick(Body* A, Stick* B, float dt);
    static void solveStickStick(Stick* A, Stick* B, float dt);
    
    static void solveCircleCircle(Obj* A, Obj* B, const vec2& p1, const vec2& p2, float dt);
    
    static void solveCircleLine(Obj* A, Stick* B, const vec2& p1, float dt);
    
protected:
    
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
            body->setInputs(aabb);
        }
    }
    
public:
    
    float targetRadius = 8.0f;
    
    float width;
    float height;
    
    AABB aabb;
    
    int maxBodies;
    
    World(float width, float height) : width(width), height(height), aabb(vec2(-0.5f * width, -0.5f * height), vec2(0.5f * width, 0.5f * height)) {
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
    
    void clear() {
        iterator_type begin = bodies.begin();
        
        while(begin != bodies.end()) {
            iterator_type it = begin;
            ++begin;
            destoryBody(it);
        }
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
