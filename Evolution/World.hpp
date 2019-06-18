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
    
    void step(float dt) {
        getContacts();
        solveContacts();
        
        for(Body* body : bodies) {
            body->step(dt);
        }
    }
    
    inline void getContacts() {
        contacts.clear();
        tree.query(&contacts);
    }
    
    inline void step(float dt, int its) {
        dt /= (float) its;
        for(int i = 0; i < its; ++i)
            step(dt);
    }
};

#endif /* World_hpp */
