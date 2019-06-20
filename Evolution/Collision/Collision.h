//
//  Collision.h
//  Evolution
//
//  Created by Arthur Sun on 6/20/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Collision_h
#define Collision_h

#define aabb_extension 0.005f

#include "vec2.h"

/// axis aligned bounding box
struct AABB
{
    vec2 lowerBound;
    vec2 upperBound;
    
    inline AABB() {}
    
    inline AABB(const vec2& lowerBound, const vec2& upperBound) : lowerBound(lowerBound), upperBound(upperBound) {}
    
    inline float area() const {
        return (upperBound.x - lowerBound.x) * (upperBound.y - lowerBound.y);
    }
    
    inline float perimeter() const {
        return 2.0f * (upperBound.x - lowerBound.x + upperBound.y - lowerBound.y);
    }
    
    inline bool contains(const AABB& aabb) const {
        return lowerBound.x <= aabb.lowerBound.x && lowerBound.y <= aabb.lowerBound.y && upperBound.x >= aabb.upperBound.x && upperBound.y >= aabb.upperBound.y;
    }
};

inline vec2 min(const vec2& a, const vec2& b) {
    return vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}

inline vec2 max(const vec2& a, const vec2& b) {
    return vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}

inline AABB combine_aabb(const AABB& a, const AABB& b) {
    return AABB(min(a.lowerBound, b.lowerBound), max(a.upperBound, b.upperBound));
}

inline bool touches(const AABB& a, const AABB& b) {
    vec2 d1 = b.upperBound - a.lowerBound;
    vec2 d2 = a.upperBound - b.lowerBound;
    return firstbit(d1.x) * firstbit(d1.y) * firstbit(d2.x) * firstbit(d2.y);
}

inline AABB extendAABB(const AABB& aabb) {
    static const vec2 extension = vec2(aabb_extension, aabb_extension);
    return AABB(aabb.lowerBound - extension, aabb.upperBound + extension);
}

struct Contact
{
    void* obj1;
    void* obj2;
    
    friend inline bool operator == (const Contact& A, const Contact& B) {
        return (A.obj1 == B.obj1 && A.obj2 == B.obj2) || (A.obj1 == B.obj2 && A.obj2 == B.obj1);
    }
};

#endif /* Collision_h */
