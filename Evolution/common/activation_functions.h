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

enum activation_function_types {
    activation_linear = 0,
    activation_sigmoid,
    activation_smooth,
    activation_tanh,
    activation_step,
    activation_swish,
    activation_gauss,
    activation_sine,
    activation_cosine,
    activation_abs,
    activation_inv,
    numberOfActivationFunctions
};

struct ActivationFunction
{
    int type = activation_linear;
        
    inline float operator () (float x) const {
        if(type == activation_linear) {
            return x;
        }
        
        if(type == activation_sigmoid) {
            return 1.0f / (1.0f + exp(-x));
        }
        
        if(type == activation_smooth) {
            return x / (1.0f + fabs(x));
        }
        
        if(type == activation_tanh) {
            return tanh(x);
        }
        
        if(type == activation_step) {
            return firstbit(x);
        }
        
        if(type == activation_swish) {
            return x / (1.0f + exp(-x));
        }
        
        if(type == activation_gauss) {
            return exp(-(x * x) * 0.5f);
        }
        
        if(type == activation_sine) {
            return sinf(M_PI * x);
        }
        
        if(type == activation_cosine) {
            return cosf(M_PI * x);
        }
        
        if(type == activation_abs) {
            return fabs(x);
        }
        
        if(type == activation_inv) {
            return -x;
        }
        
        return 0.0f;
    }
};

#endif /* activation_functions_h */
