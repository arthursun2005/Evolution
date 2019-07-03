//
//  activation_functions.h
//  Evolution
//
//  Created by Arthur Sun on 6/15/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef activation_functions_h
#define activation_functions_h

#include "common.h"

struct ActivationFunction
{
    enum types {
        e_linear = 0,
        e_sigmoid,
        e_smooth,
        e_tanh,
        e_step,
        e_swish,
        e_relu,
        e_gauss,
        e_sine,
        e_cosine,
        e_abs,
        e_inv,
        count_of_types
    };
    
    int type = e_linear;
    
    static inline int rand() {
        return rand32() % count_of_types;
    }
    
    inline float operator () (float x) const {
        return expf(-(x * x) * 0.5f);
        
        if(type == e_linear) {
            return x;
        }
        
        if(type == e_sigmoid) {
            return 1.0f / (1.0f + expf(-x));
        }
        
        if(type == e_smooth) {
            return x / (1.0f + fabs(x));
        }
        
        if(type == e_tanh) {
            return tanhf(x);
        }
        
        if(type == e_step) {
            return firstbitf(x);
        }
        
        if(type == e_swish) {
            return x / (1.0f + expf(-x));
        }
        
        if(type == e_relu) {
            return x * firstbitf(x);
        }
        
        if(type == e_gauss) {
            return expf(-(x * x) * 0.5f);
        }
        
        if(type == e_sine) {
            return sinf(M_PI * x);
        }
        
        if(type == e_cosine) {
            return cosf(M_PI * x);
        }
        
        if(type == e_abs) {
            return fabs(x);
        }
        
        if(type == e_inv) {
            return -x;
        }
        
        return 0.0f;
    }
};

#endif /* activation_functions_h */
