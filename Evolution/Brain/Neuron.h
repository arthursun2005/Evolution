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
#include <vector>
#include <stack>

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
    
    static inline float rand() {
        static std::default_random_engine generator;
        static std::normal_distribution<float> distribution(0.0f, 1.0f);
        return distribution(generator);
    }
    
    std::vector<Link> inputs;
    
    int flags;
    
    ActivationFunction f;
    
    inline Neuron() : bias(0.0f) {}
    
    inline void add_link(int index) {
        Link link;
        link.index = index;
        link.weight = 1.0f;
        inputs.push_back(link);
    }
    
    inline void setRandom(float scl) {
        for(Link& link : inputs)
            link.weight = scl * rand();
        
        bias = scl * rand();
    }
    
    inline void setShared(float value) {
        for(Link& link : inputs)
            link.weight = value;
        
        bias = value;
    }
    
    inline void clear() {
        inputs.clear();
        bias = 0.0f;
    }
    
    inline void alter(float scl) {
        for(Link& link : inputs)
            link.weight += scl * rand();
        
        bias += scl * rand();
    }
    
    inline void remove_link(int index) {
        std::vector<Neuron::Link>::iterator begin = inputs.begin();
        std::vector<Neuron::Link>::iterator end = inputs.end();
        while(end-- != begin) {
            if(begin->index == index) {
                inputs.erase(begin);
                return;
            }
        }
    }
    
    inline bool has_link(int index) const {
        for(const Link& link : inputs) {
            if(link.index == index)
                return true;
        }
        
        return false;
    }
    
    inline void compute(const Neuron* neurons) {
        float sum = 0.0f;
        
        for(const Link& link : inputs)
            sum += link.weight * neurons[link.index].value;
        
        value = f(sum + bias);
        flags |= e_neuron_computed;
    }
    
    static bool has_neuron(int index1, int index2, const Neuron* neurons) {
        std::stack<int> stack;
        
        stack.push(index1);
        
        while(!stack.empty()) {
            int n = stack.top();
            stack.pop();
            
            for(const Link& link : neurons[n].inputs) {
                if(link.index == index2)
                    return true;
                
                stack.push(link.index);
            }
        }
        
        return false;
    }
    
    static void compute_value(int index, Neuron* neurons) {
        if((neurons[index].flags & e_neuron_computed) != 0)
            return;
        
        for(const Neuron::Link& link : neurons[index].inputs) {
            if((neurons[link.index].flags & e_neuron_computed) == 0)
                compute_value(link.index, neurons);
        }
        
        neurons[index].compute(neurons);
    }
};



#endif /* Neuron_h */
