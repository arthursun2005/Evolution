//
//  Neuron.h
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Neuron_h
#define Neuron_h

#include "activation_functions.h"
#include <list>

enum neuron_flags
{
    e_neuron_computed = 0b1,
    e_neuron_input = 0b10,
    e_neuron_output = 0b100
};

struct Neuron
{
    float value;
    float bias;
    
    struct Link {
        int index;
        float weight;
    };
    
    std::list<Link> inputs;
    
    int flags;
    
    ActivationFunction f;
    
    inline Neuron() : bias(0.0f) {}
    
    inline void add_link(int index, float weight = 1.0f) {
        Link link;
        link.index = index;
        link.weight = weight;
        inputs.push_back(link);
    }
    
    inline void clear() {
        inputs.clear();
        bias = 0.0f;
    }
    
    inline void alter(float scl) {
        static std::default_random_engine generator;
        static std::normal_distribution<float> distribution(0.0f, 1.0f);
        
        for(Link& link : inputs)
            link.weight += scl * distribution(generator);
        
        bias += scl * distribution(generator);
    }
    
    inline void remove_link(int index) {
        std::list<Neuron::Link>::iterator begin = inputs.begin();
        std::list<Neuron::Link>::iterator end = inputs.end();
        for(; begin != end; ++begin) {
            if(begin->index == index) {
                inputs.erase(begin);
                return;
            }
        }
    }
    
    inline bool has_link(int index) const {
        std::list<Neuron::Link>::const_iterator begin = inputs.cbegin();
        std::list<Neuron::Link>::const_iterator end = inputs.cend();
        
        for(; begin != end; ++begin) {
            if(begin->index == index)
                return true;
        }
        
        return false;
    }
    
    inline void compute(const Neuron* neurons) {
        float sum = 0.0f;
        
        for(Link& link : inputs)
            sum += link.weight * neurons[link.index].value;
        
        value = f(sum + bias);
        flags |= e_neuron_computed;
    }
};

inline void compute_value(int index, Neuron* neurons) {
    for(Neuron::Link& link : neurons[index].inputs) {
        if((neurons[link.index].flags & e_neuron_computed) == 0)
            compute_value(link.index, neurons);
    }
    
    if((neurons[index].flags & e_neuron_computed) == 0)
        neurons[index].compute(neurons);
}

#endif /* Neuron_h */
