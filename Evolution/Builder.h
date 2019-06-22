//
//  Builder.h
//  Evolution
//
//  Created by Arthur Sun on 6/22/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Builder_h
#define Builder_h

#include "World.hpp"

class Builder
{
    
public:
    
    struct Room
    {
        AABB aabb;
        Body* A;
        Body* B;
    };
    
    Builder(int x, int y, float w, float h) {}
    
private:
    
    std::list<Body*> bodies;
    
};

#endif /* Builder_h */
