//
//  BodySystem.h
//  Evolution
//
//  Created by Arthur Sun on 6/22/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef BodySystem_h
#define BodySystem_h

#include "Body.hpp"
#include "DynamicTree.hpp"
#include "Timer.h"
#include <list>
#include <algorithm>

class BodySystem
{
    
public:
        
    typedef std::list<Body*>::iterator iterator_type;
    
    typedef std::list<Body*>::const_iterator const_iterator_type;
    
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
    
    inline int getMaxBrainComplexity() const {
        int c = 0;
        
        for(Body* body : bodies)
            c = std::max(c, body->brain->totalSize());
        
        return c;
    }
    
protected:
    
    BrainSystem bs;
    
    std::list<Body*> bodies;
    
};

#endif /* BodySystem_h */
