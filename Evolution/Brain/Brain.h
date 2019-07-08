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
    
    uint links;
    
    inline uint create_neuron() {
        uint index = (uint)neurons.size();
        Neuron neuron;
        neurons.push_back(neuron);
        return index;
    }
    
    void write(FILE* os, const Neuron* n) const {
        uint size = (uint)n->inputs.size();
        
        fwrite(&size, sizeof(size), 1, os);
        
        for(const NeuralLink& link : n->inputs) {
            fwrite(&link, sizeof(link), 1, os);
        }
        
        fwrite(n, sizeof(*n) - sizeof(n->inputs), 1, os);
    }
    
    void read(FILE* is, Neuron* n) {
        uint size;
        fread(&size, sizeof(size), 1, is);
        n->inputs.resize(size);
        
        for(NeuralLink& link : n->inputs) {
            fread(&link, sizeof(link), 1, is);
        }
        
        fread(n, sizeof(*n) - sizeof(n->inputs), 1, is);
    }
    
    uint input_size;
    uint output_size;
    
public:
    
    float reward;
    
    inline Brain() : input_size(0), output_size(0) {}
    
    inline Brain(uint _input_size, uint _output_size) {
        reset(_input_size, _output_size);
    }
    
    inline Neuron* inputs() {
        return neurons.data();
    }
    
    inline Neuron* outputs() {
        return neurons.data() + input_size;
    }
    
    inline uint numOfNeurons() const {
        return (uint)neurons.size();
    }
    
    inline uint numOfLinks() const {
        return links;
    }
    
    inline uint totalSize() const {
        return numOfNeurons() + numOfLinks();
    }
    
    inline void clear() {
        neurons.clear();
        input_size = 0;
        output_size = 0;
        links = 0;
    }
    
    void reset(uint _input_size, uint _output_size) {
        reward = 0.0f;
        links = 0;
        
        input_size = _input_size;
        output_size = _output_size;
        
        uint size = input_size + output_size;
        neurons.resize(size);
        
        for(Neuron &neuron : neurons)
            neuron.reset();
        
        for(uint i = 0; i < input_size; ++i) {
            neurons[i].computed = true;
        }
        
        uint index1 = input_size + (rand32(output_size));
        uint index2 = rand32(input_size);
        neurons[index1].add_link(NeuralLink(index2));
        ++links;
    }
    
    void write(FILE* os) const {
        uint total = (uint)neurons.size();
        
        fwrite(&input_size, sizeof(&input_size), 1, os);
        fwrite(&output_size, sizeof(&output_size), 1, os);
        fwrite(&total, sizeof(&total), 1, os);
        fwrite(&links, sizeof(&links), 1, os);
        
        for(const Neuron &neuron : neurons)
            write(os, &neuron);
    }
    
    void read(FILE* is) {
        uint total;
        
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
        uint size = (uint)neurons.size();
        
        for(uint i = 0; i < input_size; ++i)
            neurons[i].compute_input();
        
        for(uint i = input_size; i < size; ++i)
            neurons[i].computed = false;
        
        for(uint i = 0; i < output_size; ++i)
            Neuron::compute_value(input_size + i, neurons.data());
    }
    
    inline void grow() {
        for(Neuron& n : neurons)
            n.grow();
    }
    
    inline void renew() {
        for(Neuron& n : neurons)
            n.renew();
    }
    
    inline void mutate() {
        for(Neuron& n : neurons)
            n.mutate();
    }
    
    void generate() {
        uint size = (uint)neurons.size();
        
        int k = rand32() & 0xf;
        
        if(k < 0x4) {
            
            uint index1 = input_size + (rand32(size - input_size));
            
            if(!neurons[index1].inputs.empty()) {
                uint i = rand32((uint32_t)neurons[index1].inputs.size());
                
                uint index2 = neurons[index1].inputs[i].index;
                
                neurons[index1].inputs.erase(neurons[index1].inputs.begin() + i);
                
                uint neuron = create_neuron();
                
                neurons[neuron].add_link(NeuralLink(index2));
                neurons[index1].add_link(NeuralLink(neuron));
                
                ++links;
            }
            
        }else if(k < 0x8) {
            uint index1 = input_size + (rand32(size - input_size));
            uint index2 = rand32(size);
            
            while(!Neuron::has_neuron(index1, index2, neurons.data()) && !Neuron::has_neuron(index2, index1, neurons.data()) && index1 != index2) {
                neurons[index1].add_link(NeuralLink(index2));
                ++links;
            }
        }else{
            uint index = rand32(size);
            int func_type = ActivationFunction::rand();
            neurons[index].type = func_type;
            neurons[index].renew();
        }
    }
    
    friend class BrainSystem;
    
};

#endif /* Brain_h */

