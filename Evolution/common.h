//
//  common.h
//  Evolution
//
//  Created by Arthur Sun on 5/31/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef common_h
#define common_h

#include <random>
#include <fstream>
#include <sstream>
#include <new>
#include <cmath>
#include <string>
#include <unistd.h>

/// from std::allocator

#ifdef _LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE

inline void* Alloc(size_t size) {
    return ::operator new(size);
}

inline void Free(void* ptr) {
    ::operator delete(ptr);
}

#else

inline void* Alloc(size_t size) {
    return __builtin_operator_new(size);
}

inline void Free(void* ptr) {
    __builtin_operator_delete(ptr);
}

#endif


template <class T, class... Args>
inline void Construct(T* ptr, Args&&... args) {
    ::new((void*)ptr) T(args...);
}

inline int fstr(const char* file_name, std::string* str) {
    std::ifstream file;
    file.open(file_name);
    
    if(!file.is_open()) {
        printf("%s cannot be opened \n", file_name);
        return -1;
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    str->assign(ss.str());
    return 0;
}

inline bool ends_in(const std::string& str, const std::string& end) {
    size_t s1 = end.size();
    size_t s2 = str.size();
    
    if(s1 > s2)
        return false;
    
    for(size_t i = 0; i < s1; ++i)
        if(end[s1 - i - 1] != str[s2 - i - 1]) return false;
    
    return true;
}

inline bool starts_with(const std::string& str, const std::string& start) {
    size_t s1 = start.size();
    size_t s2 = str.size();
    
    if(s1 > s2)
        return false;
    
    for(size_t i = 0; i < s1; ++i)
        if(start[i] != str[i]) return false;
    
    return true;
}

inline int firstbit(float x) {
    return 0b1 & ~((*(int*)&x) >> 31);
}

#define aabb_extension 0.005f

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

template <>
struct std::hash <std::pair<int, int>>
{
    inline size_t operator () (const std::pair<int, int>& pair) const {
        return (((size_t)pair.first) << 32) | ((size_t)pair.second);
    }
};

#endif /* common_h */
