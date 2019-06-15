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
#include <list>

class World
{
    
    std::list<Body*> bodies;
    
public:
    
    const int width;
    const int height;
    
    World(int width, int height) : width(width), height(height), bodies(NULL) {
    }
    
    ~World() {
        for(Body* body : bodies) {
            Free(body);
        }
    }
    
    inline std::list<Body*>::iterator begin() {
        return bodies.begin();
    }
    
    inline std::list<Body*>::iterator end() {
        return bodies.end();
    }
    
    void step(float dt) {
        for(Body* body : bodies) {
            body->step(dt);
        }
    }
};

#endif /* World_hpp */
