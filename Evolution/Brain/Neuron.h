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

struct NeuroLink {
    size_t index;
    float weight;
    unsigned int age;
    
    inline NeuroLink() {}
    
    inline NeuroLink(size_t index) : index(index), weight(0.0f), age(1u) {}
};

struct Neuron : public ActivationFunction
{
    float value;
    float bias;
    unsigned int age;
    int flags;
    
    std::vector<NeuroLink> inputs;
    
    static inline float rand() {
        static std::default_random_engine generator;
        static std::normal_distribution<float> distribution(0.0f, 1.0f);
        return distribution(generator);
    }
    
    inline Neuron() {
        reset();
    }
    
    inline void add_link(const NeuroLink& link) {
        inputs.push_back(link);
    }
    
    inline void reset() {
        inputs.clear();
        bias = 0.0f;
        age = 1u;
    }
    
    inline void mutate() {
        for(NeuroLink& link : inputs)
            link.weight += rand() / (float)link.age;
        
        bias += rand() / (float)age;
    }
    
    inline void grow() {
        for(NeuroLink& link : inputs)
            ++link.age;
        
        ++age;
    }
    
    inline void remove_link(size_t index) {
        auto begin = inputs.begin();
        auto end = inputs.end();
        while(end-- != begin) {
            if(begin->index == index) {
                inputs.erase(begin);
                return;
            }
        }
    }
    
    inline bool has_link(size_t index) const {
        for(const NeuroLink& link : inputs) {
            if(link.index == index)
                return true;
        }
        
        return false;
    }
    
    inline void compute(const Neuron* neurons) {
        float sum = bias;
        
        for(const NeuroLink& link : inputs)
            sum += link.weight * neurons[link.index].value;
        
        value = operator () (sum);
        flags |= e_neuron_computed;
    }
    
    inline void compute_input() {
        assert(inputs.empty() && (flags & e_neuron_input) != 0);
        value = operator () (value + bias);
    }
    
    static bool has_neuron(size_t index1, size_t index2, const Neuron* neurons) {
        std::stack<size_t> stack;
        
        stack.push(index1);
        
        while(!stack.empty()) {
            size_t n = stack.top();
            stack.pop();
            
            for(const NeuroLink& link : neurons[n].inputs) {
                if(link.index == index2)
                    return true;
                
                stack.push(link.index);
            }
        }
        
        return false;
    }
    
    static void compute_value(size_t index, Neuron* neurons) {
        if((neurons[index].flags & e_neuron_computed) != 0)
            return;
        
        for(const NeuroLink& link : neurons[index].inputs) {
            if((neurons[link.index].flags & e_neuron_computed) == 0)
                compute_value(link.index, neurons);
        }
        
        neurons[index].compute(neurons);
    }
};

#endif /* Neuron_h */
