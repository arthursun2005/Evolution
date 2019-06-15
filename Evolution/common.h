//
//  common.h
//  Evolution
//
//  Created by Arthur Sun on 5/31/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef common_h
#define common_h

#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <new>
#include <cmath>
#include <string>
#include <unistd.h>

inline void* Alloc(size_t size) {
#ifdef _LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE
    return ::operator new(size);
#else
    return __builtin_operator_new(size);
#endif
}

inline void Free(void* ptr) {
#ifdef _LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE
    ::operator delete(ptr);
#else
    __builtin_operator_delete(ptr);
#endif
}

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

inline bool end_in(const std::string& str, const std::string& end) {
    size_t s1 = end.size();
    size_t s2 = str.size();
    
    if(s1 > s2)
        return false;
    
    for(size_t i = 0; i < s1; ++i)
        if(end[s1 - i - 1] != str[s2 - i - 1]) return false;
    
    return true;
}

inline bool start_with(const std::string& str, const std::string& start) {
    size_t s1 = start.size();
    size_t s2 = str.size();
    
    if(s1 > s2)
        return false;
    
    for(size_t i = 0; i < s1; ++i)
        if(start[i] != str[i]) return false;
    
    return true;
}

#endif /* common_h */
