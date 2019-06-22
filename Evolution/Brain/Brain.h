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
    std::vector<Neuron> neurons;
    
    /// creates a neuron that is from -> neuron -> to
    /// if from or to is -1, no link is added
    inline int create_neuron(int from, int to, int type) {
        int index = (int)neurons.size();
        
        Neuron neuron;
        
        neuron.flags = 0;
        
        /// we add the links if not given -1
        if(from != -1)
            neuron.add_link(from);
        
        if(to != -1)
            neurons[to].add_link(index);
        
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
    
    Brain(int _input_size, int _output_size) {
        reset(_input_size, _output_size);
    }
    
    Brain(const Brain& brain) = delete;
    
    Brain& operator = (const Brain& brain) = delete;
    
    inline Neuron* inputs() {
        return neurons.data();
    }
    
    inline Neuron* outputs() {
        return neurons.data() + input_size;
    }
    
    inline int size() const {
        return (int)neurons.size();
    }
    
    inline int totalSize() const {
        int links = 0;
        for(const Neuron &neuron : neurons)
            links += neuron.inputs.size();
        return size() + links;
    }
    
    void reset(int _input_size, int _output_size) {
        input_size = _input_size;
        output_size = _output_size;
        
        int size = input_size + output_size;
        neurons.resize(size);
        
        for(Neuron &neuron : neurons)
            neuron.clear();
        
        for(int i = 0; i < input_size; ++i) {
            neurons[i].flags |= e_neuron_computed | e_neuron_input;
        }
        
        for(int i = 0; i < output_size; ++i) {
            neurons[input_size + i].flags |= e_neuron_output;
        }
        
        for(int i = 0; i < 8; ++i) {
            if(rand() & 0x1) {
                int index1 = input_size + (rand() % output_size);
                int index2 = rand() % size;
                if(rand() & 0x1) {
                    if(!neurons[index1].has_link(index2))
                        neurons[index1].add_link(index2);
                }else{
                    create_neuron(index2, index1, ActivationFunction::rand());
                }
            }
        }
    }
    
    void write(std::ofstream& os) const {
        int total = (int)neurons.size();
        os.write((char*)&input_size, sizeof(input_size));
        os.write((char*)&output_size, sizeof(output_size));
        os.write((char*)&total, sizeof(total));
        for(const Neuron &neuron : neurons)
            write(os, &neuron);
    }
    
    void read(std::ifstream& is) {
        int total;
        int i, o;
        
        is.read((char*)&i, sizeof(i));
        is.read((char*)&o, sizeof(o));
        is.read((char*)&total, sizeof(total));
        
        input_size = i;
        output_size = o;
        
        neurons.resize(total);
        
        for(Neuron& neuron : neurons) {
            read(is, &neuron);
        }
    }

    inline void compute() {
        int size = (int)neurons.size();
        
        for(int i = input_size; i < size; ++i)
            neurons[i].flags &= ~ e_neuron_computed;
        
        for(int i = 0; i < output_size; ++i) {
            compute_value(input_size + i, neurons.data());
        }
    }
    
    inline void alter(float scl) {
        int size = (int)neurons.size();
        
        for(int i = input_size; i < size; ++i)
            neurons[i].alter(scl);
    }
    
    inline static void copy(Brain* dst, const Brain* src) {
        int size = (int)src->neurons.size();
        dst->neurons.resize(size);
        
        for(int i = 0; i < size; ++i) {
            dst->neurons[i] = src->neurons[i];
        }
    }
    
    static void alter(Brain* result, const Brain* brain) {
        copy(result, brain);
        
        /// 1/2 chance of altering
        
        /// 1/2 chance of changing function type of a random neuron
        
        /// 1/2 chance of adding a link (3/4) or adding a neuron (1/4)
        
        if((rand() & 0x1)) {
            int func_type = ActivationFunction::rand();
            int size = (int)(result->neurons.size());
            int index1 = result->input_size + (rand() % (size - result->input_size));
            
            if((rand() & 0x1)) {
                
                int index2 = rand() % size;
                
                if((rand() & 0xf) < 0x4) {
                    result->create_neuron(index2, index1, func_type);
                }else{
                    if(!result->neurons[index1].has_link(index2))
                        result->neurons[index1].add_link(index2);
                }
            }else{
                result->neurons[index1].f.type = func_type;
            }
        }
        
        result->alter(brain_alter_scale);
    }
    
};

#endif /* Brain_h */
