//
//  Brain.h
//  Evolution
//
//  Created by Arthur Sun on 6/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Brain_h
#define Brain_h

#include "Neuron.h"

class Brain
{
    
protected:
    
    std::vector<Neuron> neurons;
    
    size_t links;
    
    inline size_t create_neuron() {
        size_t index = neurons.size();
        Neuron neuron;
        neuron.flags = 0;
        neurons.push_back(neuron);
        return index;
    }
    
    void write(FILE* os, const Neuron* n) const {
        size_t size = n->inputs.size();
        
        fwrite(&size, sizeof(size), 1, os);
        
        for(const NeuroLink& link : n->inputs) {
            fwrite(&link, sizeof(link), 1, os);
        }
        
        fwrite(n, sizeof(*n) - sizeof(n->inputs), 1, os);
    }
    
    void read(FILE* is, Neuron* n) {
        size_t size;
        fread(&size, sizeof(size), 1, is);
        n->inputs.resize(size);
        
        for(NeuroLink& link : n->inputs) {
            fread(&link, sizeof(link), 1, is);
        }
        
        fread(n, sizeof(*n) - sizeof(n->inputs), 1, is);
    }
    
    size_t input_size;
    size_t output_size;
    
public:
    
    float reward;
    
    inline Brain() : input_size(0), output_size(0) {}
    
    inline Brain(size_t _input_size, size_t _output_size) {
        reset(_input_size, _output_size);
    }
    
    inline Neuron* inputs() {
        return neurons.data();
    }
    
    inline Neuron* outputs() {
        return neurons.data() + input_size;
    }
    
    inline size_t numOfNeurons() const {
        return neurons.size();
    }
    
    inline size_t numOfLinks() const {
        return links;
    }
    
    inline size_t totalSize() const {
        return numOfNeurons() + numOfLinks();
    }
    
    inline void clear() {
        neurons.clear();
        input_size = 0;
        output_size = 0;
        links = 0;
    }
    
    void reset(size_t _input_size, size_t _output_size) {
        reward = 0.0f;
        links = 0;
        
        input_size = _input_size;
        output_size = _output_size;
        
        size_t size = input_size + output_size;
        neurons.resize(size);
        
        for(Neuron &neuron : neurons)
            neuron.reset();
        
        for(size_t i = 0; i < input_size; ++i) {
            neurons[i].flags = e_neuron_computed | e_neuron_input;
        }
        
        for(size_t i = 0; i < output_size; ++i) {
            neurons[input_size + i].flags = e_neuron_output;
        }
        
        size_t index1 = input_size + (rand64() % output_size);
        size_t index2 = rand64() % input_size;
        neurons[index1].add_link(NeuroLink(index2));
        ++links;
    }
    
    void write(FILE* os) const {
        size_t total = neurons.size();
        
        fwrite(&input_size, sizeof(&input_size), 1, os);
        fwrite(&output_size, sizeof(&output_size), 1, os);
        fwrite(&total, sizeof(&total), 1, os);
        fwrite(&links, sizeof(&links), 1, os);
        
        for(const Neuron &neuron : neurons)
            write(os, &neuron);
    }
    
    void read(FILE* is) {
        size_t total;
        
        fread(&input_size, sizeof(input_size), 1, is);
        fread(&output_size, sizeof(output_size), 1, is);
        fread(&total, sizeof(total), 1, is);
        fread(&links, sizeof(links), 1, is);
        
        neurons.resize(total);
        
        for(Neuron& neuron : neurons) {
            read(is, &neuron);
        }
    }
    
    inline void compute() {
        size_t size = neurons.size();
        
        for(size_t i = 0; i < input_size; ++i)
            neurons[i].compute_input();
        
        for(size_t i = input_size; i < size; ++i)
            neurons[i].flags &= ~ e_neuron_computed;
        
        for(size_t i = 0; i < output_size; ++i)
            Neuron::compute_value(input_size + i, neurons.data());
    }
    
    inline void renew() {
        for(Neuron& n : neurons)
            n.renew();
    }
    
    inline void grow() {
        for(Neuron& n : neurons)
            n.grow();
    }
    
    inline void mutate() {
        for(Neuron& n : neurons)
            n.mutate();
    }
    
    void generate() {
        size_t size = neurons.size();
        
        int k = rand32() & 0xf;
        
        if(k < 0x4) {
            
            size_t index1 = input_size + (rand64() % (size - input_size));
            
            if(!neurons[index1].inputs.empty()) {
                size_t i = rand64() % neurons[index1].inputs.size();
                
                size_t index2 = neurons[index1].inputs[i].index;
                
                neurons[index1].inputs.erase(neurons[index1].inputs.begin() + i);
                
                size_t neuron = create_neuron();
                
                neurons[neuron].add_link(NeuroLink(index2));
                neurons[index1].add_link(NeuroLink(neuron));
                
                ++links;
            }
            
        }else if(k < 0x8) {
            size_t index1 = input_size + (rand64() % (size - input_size));
            size_t index2 = rand64() % size;
            
            while(!Neuron::has_neuron(index1, index2, neurons.data()) && !Neuron::has_neuron(index2, index1, neurons.data()) && index1 != index2) {
                neurons[index1].add_link(NeuroLink(index2));
                ++links;
            }
        }else{
            size_t index = rand64() % size;
            int func_type = ActivationFunction::rand();
            neurons[index].type = func_type;
        }
    }
    
    friend class BrainSystem;
    
};

#endif /* Brain_h */

