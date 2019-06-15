//
//  activation_functions.h
//  Evolution
//
//  Created by Arthur Sun on 6/15/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef activation_functions_h
#define activation_functions_h

enum activation_function_types {
    e_linear = 0,
    e_sigmoid,
    e_tanh,
    e_step,
    e_relu,
    e_gauss,
    e_sine,
    e_cosine,
    e_abs,
    e_inv,
    numberOfActivationFunctions
};

inline int bit(float x) {
    return 0b1 & ~(*(int*)&x >> 31);
}

struct ActivationFunction
{
    int type = e_linear;
    
    inline float operator () (float x) const {
        if(type == e_linear) {
            return x;
        }
        
        if(type == e_sigmoid) {
            return 1.0f / (1.0f + exp(-x));
        }
        
        if(type == e_tanh) {
            return tanh(x);
        }
        
        if(type == e_step) {
            return bit(x);
        }
        
        if(type == e_relu) {
            return bit(x) * x;
        }
        
        if(type == e_gauss) {
            return exp(-(x * x) * 0.5f);
        }
        
        if(type == e_sine) {
            return sinf(M_PI * x);
        }
        
        if(type == e_cosine) {
            return cosf(M_PI * x);
        }
        
        if(type == e_abs) {
            return x < 0.0f ? -x : x;
        }
        
        if(type == e_inv) {
            return -x;
        }
        
        return 0.0f;
    }
};

#endif /* activation_functions_h */
