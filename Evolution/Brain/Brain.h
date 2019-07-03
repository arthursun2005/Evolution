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
    
    void write(std::ofstream& os, const Neuron* n) const {
        size_t size = n->inputs.size();
        os.write((char*)&size, sizeof(size));
        
        for(const NeuroLink& link : n->inputs) {
            os.write((char*)&link, sizeof(link));
        }
        
        os.write((char*)&n, sizeof(n) - sizeof(n->inputs));
    }
    
    void read(std::ifstream& is, Neuron* n) {
        size_t size;
        is.read((char*)&size, sizeof(size));
        n->inputs.resize(size);
        
        for(const NeuroLink& link : n->inputs) {
            is.read((char*)&link, sizeof(link));
        }
        
        is.read((char*)&n, sizeof(n) - sizeof(n->inputs));
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
    
    void write(std::ofstream& os) const {
        size_t total = neurons.size();
        
        os.write((char*)&input_size, sizeof(input_size));
        os.write((char*)&output_size, sizeof(output_size));
        os.write((char*)&total, sizeof(total));
        os.write((char*)&links, sizeof(links));
        
        for(const Neuron &neuron : neurons)
            write(os, &neuron);
    }
    
    void read(std::ifstream& is) {
        size_t total;
        size_t i, o;
        
        is.read((char*)&i, sizeof(i));
        is.read((char*)&o, sizeof(o));
        is.read((char*)&total, sizeof(total));
        is.read((char*)&links, sizeof(links));
        
        input_size = i;
        output_size = o;
        
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
        
        if(k < 0x2) {
            
            size_t index1 = -1;
            
            while(index1 == -1 || neurons[index1].inputs.empty())
                index1 = input_size + (rand64() % (size - input_size));
            
            size_t i = rand64() % neurons[index1].inputs.size();
            
            size_t index2 = neurons[index1].inputs[i].index;
            
            neurons[index1].inputs.erase(neurons[index1].inputs.begin() + i);
            
            size_t neuron = create_neuron();
            
            neurons[neuron].add_link(NeuroLink(index2));
            neurons[index1].add_link(NeuroLink(neuron));
            
            ++links;
            
        }else if(k < 0x4) {
            size_t index1 = -1;
            size_t index2 = 0;
            
            size_t tries = 0;
            
            while(index1 == -1 || Neuron::has_neuron(index1, index2, neurons.data()) || Neuron::has_neuron(index2, index1, neurons.data()) || index1 == index2) {
                
                if(tries >= size)
                    return;
                
                index1 = input_size + (rand64() % (size - input_size));
                index2 = rand64() % size;
                
                ++tries;
            }
            
            neurons[index1].add_link(NeuroLink(index2));
            ++links;
        }else if(k < 0x8) {
            size_t index = rand64() % size;
            int func_type = ActivationFunction::rand();
            neurons[index].type = func_type;
        }
    }
    
    friend class BrainSystem;
    
};

#endif /* Brain_h */

