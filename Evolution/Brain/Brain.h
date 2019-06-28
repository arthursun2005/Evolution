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
    
    int links;
    
    /// creates a neuron that is from -> neuron -> to
    /// if from or to is -1, no link is added
    inline int create_neuron(int from, int to, int type) {
        int index = (int)neurons.size();
        
        Neuron neuron;
        
        neuron.flags = 0;
        
        /// we add the links if not given -1
        if(from != -1) {
            neuron.add_link(from);
            ++links;
        }
        
        if(to != -1) {
            neurons[to].add_link(index);
            ++links;
        }
        
        neuron.f.type = type;
        
        neurons.push_back(neuron);
        
        return index;
    }
    
    void write(std::ofstream& os, const Neuron* n) const {
        int size = (int)n->inputs.size();
        os.write((char*)&size, sizeof(size));
        
        for(const Neuron::Link& link : n->inputs) {
            os.write((char*)&link.index, sizeof(link.index));
            os.write((char*)&link.weight, sizeof(link.weight));
        }
        
        os.write((char*)&n->bias, sizeof(n->bias));
        os.write((char*)&n->f, sizeof(n->f));
    }
    
    void read(std::ifstream& is, Neuron* n) {
        int size;
        is.read((char*)&size, sizeof(size));
        n->inputs.resize(size);
        
        for(const Neuron::Link& link : n->inputs) {
            is.read((char*)&link.index, sizeof(link.index));
            is.read((char*)&link.weight, sizeof(link.weight));
        }
        
        is.read((char*)&n->bias, sizeof(n->bias));
        is.read((char*)&n->f, sizeof(n->f));
    }
    
    int input_size;
    int output_size;
    
public:
    
    float reward;
    
    inline Brain() : input_size(0), output_size(0) {}
    
    inline Brain(int _input_size, int _output_size) {
        reset(_input_size, _output_size);
    }
    
    inline Neuron* inputs() {
        return neurons.data();
    }
    
    inline Neuron* outputs() {
        return neurons.data() + input_size;
    }
    
    inline int numOfNeurons() const {
        return (int)neurons.size();
    }
    
    inline int numOfLinks() const {
        return links;
    }
    
    inline int totalSize() const {
        return numOfNeurons() + numOfLinks();
    }
    
    inline void clear() {
        neurons.clear();
        input_size = 0;
        output_size = 0;
        links = 0;
    }
    
    void reset(int _input_size, int _output_size) {
        reward = 0.0f;
        links = 0;
        
        input_size = _input_size;
        output_size = _output_size;
        
        int size = input_size + output_size;
        neurons.resize(size);
        
        for(Neuron &neuron : neurons)
            neuron.clear();
        
        for(int i = 0; i < input_size; ++i) {
            neurons[i].flags = e_neuron_computed | e_neuron_input;
        }
        
        for(int i = 0; i < output_size; ++i) {
            neurons[input_size + i].flags = e_neuron_output;
        }
        
        int index1 = input_size + (rand() % output_size);
        int index2 = rand() % input_size;
        neurons[index1].add_link(index2);
        ++links;
    }
    
    void write(std::ofstream& os) const {
        int total = (int)neurons.size();
        
        os.write((char*)&input_size, sizeof(input_size));
        os.write((char*)&output_size, sizeof(output_size));
        os.write((char*)&total, sizeof(total));
        os.write((char*)&links, sizeof(links));
        
        for(const Neuron &neuron : neurons)
            write(os, &neuron);
    }
    
    void read(std::ifstream& is) {
        int total;
        int i, o;
        
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
        int size = (int)neurons.size();
        
        for(int i = 0; i < input_size; ++i)
            neurons[i].value = neurons[i].f(neurons[i].value + neurons[i].bias);
        
        for(int i = input_size; i < size; ++i)
            neurons[i].flags &= ~ e_neuron_computed;
        
        for(int i = 0; i < output_size; ++i)
            Neuron::compute_value(input_size + i, neurons.data());
    }
    
    inline void alter(float scl) {
        for(Neuron& n : neurons)
            n.alter(scl);
    }
    
    inline void setRandom(float scl) {
        for(Neuron& n : neurons)
            n.setRandom(scl);
    }
    
    inline void setShared(float value) {
        for(Neuron& n : neurons)
            n.setShared(value);
    }
    
    void mutate() {
        int size = (int)(neurons.size());
        
        int k = rand() & 0xFFFF;
        
        if(k <= 0x4FFF) {
            
            int index1 = -1;
            
            while(index1 == -1 || neurons[index1].inputs.empty())
                index1 = input_size + (rand() % (size - input_size));
            
            
            int i = rand() % neurons[index1].inputs.size();
            
            int index2 = neurons[index1].inputs[i].index;
            
            neurons[index1].inputs.erase(neurons[index1].inputs.begin() + i);
            --links;
            
            int func_type = ActivationFunction::rand();
            
            create_neuron(index2, index1, func_type);
            
        }else if(k <= 0x7FFF) {
            int index1 = -1;
            int index2 = 0;
            
            int tries = 0;
            
            while(index1 == -1 || Neuron::has_neuron(index1, index2, neurons.data()) || Neuron::has_neuron(index2, index1, neurons.data()) || index1 == index2) {
                if(tries >= size) return;
                index1 = input_size + (rand() % (size - input_size));
                index2 = rand() % size;
                ++tries;
            }
            
            neurons[index1].add_link(index2);
            ++links;
        }else{
            int index = rand() % size;
            int func_type = ActivationFunction::rand();
            neurons[index].f.type = func_type;
        }
    }
    
    static void produce(const Brain* A, const Brain* B, Brain* C) {
        *C = *A;
        
        int size = C->numOfNeurons();
        for(int i = 0; i < size; ++i) {
            int lsize = (int)C->neurons[i].inputs.size();
            
            for(int j = 0; j < lsize; ++j) {
                if(rand() & 0b1) {
                    C->neurons[i].inputs[j].weight = B->neurons[i].inputs[j].weight;
                }
            }
            
            if(rand() & 0b1) {
                C->neurons[i].bias = B->neurons[i].bias;
            }
        }
    }
    
    friend class BrainSystem;
    
};

#endif /* Brain_h */

