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

#define memory_rate 0.75f

#define learn_rate 0.25f

struct NeuralLink {
    uint index;
    
    uint age;
    
    float weight;
    float rate;
    
    inline NeuralLink() {}
    
    inline NeuralLink(uint index) : index(index), weight(gaussian_randomf()), rate(0.0f), age(0) {}
};

inline float learn_at_age(float x) {
    return learn_rate * gaussian_randomf() / x;
}

struct Neuron : public ActivationFunction
{
    float value;
    uint age;
    bool computed;
    
    float bias;
    float rate;
    
    std::vector<NeuralLink> inputs;
    
    inline Neuron() {
        reset();
    }
    
    inline void add_link(const NeuralLink& link) {
        inputs.push_back(link);
    }
    
    inline void reset() {
        inputs.clear();
        bias = gaussian_randomf();
        rate = 0.0f;
        age = 0;
    }
    
    inline void mutate() {
        for(NeuralLink& link : inputs)
            link.rate += learn_at_age(link.age);
        
        bias += rate;
    }
    
    inline void grow() {
        for(NeuralLink& link : inputs) {
            ++link.age;
            link.rate *= memory_rate;
            link.weight += link.rate;
        }
        
        ++age;
        rate *= memory_rate;
        rate += learn_at_age(age);
    }
    
    inline void renew() {
        for(NeuralLink& link : inputs) {
            link.age = 0;
            link.rate = 0.0f;
            link.weight = gaussian_randomf();
        }
        
        age = 0;
        rate = 0.0f;
        bias = gaussian_randomf();
    }
    
    inline void remove_link(uint index) {
        auto begin = inputs.begin();
        auto end = inputs.end();
        while(end-- != begin) {
            if(begin->index == index) {
                inputs.erase(begin);
                return;
            }
        }
    }
    
    inline bool has_link(uint index) const {
        for(const NeuralLink& link : inputs) {
            if(link.index == index)
                return true;
        }
        
        return false;
    }
    
    inline void compute(const Neuron* neurons) {
        float sum = bias;
        
        for(const NeuralLink& link : inputs)
            sum += link.weight * neurons[link.index].value;
        
        value = operator () (sum);
        computed = true;
    }
    
    inline void compute_input() {
        value = operator () (value + bias);
    }
    
    inline static bool has_neuron(uint index1, uint index2, const Neuron* neurons) {
        std::stack<uint> stack;
        
        stack.push(index1);
        
        while(!stack.empty()) {
            uint n = stack.top();
            stack.pop();
            
            for(const NeuralLink& link : neurons[n].inputs) {
                if(link.index == index2)
                    return true;
                
                stack.push(link.index);
            }
        }
        
        return false;
    }
    
    inline static void compute_value(uint index, Neuron* neurons) {
        if(neurons[index].computed)
            return;
        
        for(const NeuralLink& link : neurons[index].inputs)
            compute_value(link.index, neurons);
        
        neurons[index].compute(neurons);
    }
};

#endif /* Neuron_h */
