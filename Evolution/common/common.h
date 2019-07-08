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
#include <float.h>

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

#define uint32_inv_max 1.0f / (float)0xffffffff

inline uint32_t rand32() {
    return arc4random();
}

inline uint32_t rand32(uint32_t ub) {
    return arc4random_uniform(ub);
}

inline float gaussian_randomf() {
    thread_local std::default_random_engine g;
    thread_local std::normal_distribution<float> d(0.0f, 1.0f);
    return d(g);
}

inline float randomf(float a, float b) {
    return rand32() * uint32_inv_max * (b - a) + a;
}

inline int firstbitf(float x) {
    return 1 & (~((*(int*)&x) >> 31));
}

inline void* Alloc(uint size) {
    return ::operator new(size);
}

inline void Free(void* ptr) {
    return ::operator delete(ptr);
}

#endif /* common_h */
