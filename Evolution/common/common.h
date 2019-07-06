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

#define uint32_max 0xffffffff

#define uint64_max 0xffffffffffffffff

#define uint32_inv_max 1.0 / (double) uint32_max

#define uint64_inv_max 1.0 / (double) uint64_max

inline uint64_t rand64() {
    thread_local uint64_t r = clock();
    r = r * 0x1fffffffffffffff + 32416190071;
    r = r ^ (r >> 15) ^ (r << 17);
    return r;
}

inline uint32_t rand32() {
    return (uint32_t)rand64();
}

inline float gaussian_randomf() {
    thread_local float k1;
    thread_local float k2;
    thread_local bool G = false;
    
    if(G) {
        G = !G;
        return sinf(2.0f * M_PI * k1) * k2;
    }else{
        G = !G;
        
        k1 = rand32() * uint32_inv_max;
        k2 = rand32() * uint32_inv_max;
        k2 = sqrtf(-2.0f * logf(k2));
        
        return cosf(2.0f * M_PI * k1) * k2;
    }
}

inline float randomf(float a, float b) {
    return rand32() * uint32_inv_max * (b - a) + a;
}

inline int firstbitf(float x) {
    return 0b1 & (~((*(int*)&x) >> 31));
}

#endif /* common_h */
