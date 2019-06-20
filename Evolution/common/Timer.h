//
//  Timer.h
//  Evolution
//
//  Created by Arthur Sun on 6/16/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

#include <chrono>

template <class _clock_type>
class _Timer
{
    
    typedef _clock_type clock_type;
    typedef typename clock_type::time_point time_point;
    static constexpr double time_type_ratio = clock_type::period::num / (double) clock_type::period::den;
    
    time_point start;
    
public:
    
    inline _Timer() : start(clock()) {}
    
    inline time_point clock() const {
        return clock_type::now();
    }
    
    inline double now() const {
        return (clock() - start).count() * time_type_ratio;
    }
    
    inline void reset() {
        start = clock();
    }
    
    inline void reset(const time_point& point) {
        start = point;
    }
    
};

typedef _Timer<std::chrono::high_resolution_clock> Timer;

#endif /* Timer_h */
