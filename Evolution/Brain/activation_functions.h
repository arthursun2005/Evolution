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
        e_tanh,
        e_step,
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
        switch(type) {
            case e_linear:
                return x;
                break;
                
            case e_sigmoid:
                return 1.0f / (1.0f + expf(-x));
                break;
                
            case e_tanh:
                return tanhf(x);
                break;
                
            case e_step:
                return firstbitf(x);
                break;
                
            case e_relu:
                return x * firstbitf(x);
                break;
                
            case e_gauss:
                return expf(-(x * x) * 0.5f);
                break;
                
            case e_sine:
                return sinf(M_PI * x);
                break;
                
            case e_cosine:
                return cosf(M_PI * x);
                break;
                
            case e_abs:
                return fabs(x);
                break;
                
            case e_inv:
                return -x;
                break;
                
            default:
                throw ("invalid function type");
                return 0.0f;
                break;
        }
    }
};

#endif /* activation_functions_h */
