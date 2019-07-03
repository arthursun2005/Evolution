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

inline int32_t rand32() {
    return rand();
}

inline int64_t rand64() {
    return (int64_t(rand32()) << 32) | rand32();
}

inline int firstbitf(float x) {
    return 0b1 & (~((*(int*)&x) >> 31));
}

inline float randf(float a, float b) {
    static constexpr float inv_max = 1.0f/RAND_MAX;
    return rand() * inv_max * (b - a) + a;
}

#endif /* common_h */
