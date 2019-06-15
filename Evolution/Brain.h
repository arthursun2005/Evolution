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
    std::vector<Neuron*> neurons;
    
    /// creates a neuron that is from -> neuron -> to
    /// if from or to is NULL, no link is added
    inline Neuron* create_neuron(Neuron* from, Neuron* to, int type) {
        Neuron *neuron = new Neuron();
        
        neuron->flags = 0;
        
        /// we add the link if not given NULL
        if(from != NULL) neuron->add_link(from);
        
        if(to != NULL) to->add_link(neuron);
        
        neuron->f.type = type;
        
        neurons.push_back(neuron);
        
        return neuron;
    }
    
    inline void alterWeights(float scl) {
        for(Neuron* neuron : neurons)
            neuron->alter(scl);
    }
    
    void write(std::ofstream& os, Neuron* n) const {
        size_t index;
        int input;
        
        size_t size = n->inputs.size();
        os.write((char*)&size, sizeof(size));
        for(Neuron::Link& link : n->inputs) {
            input = link.input->flags & e_neuron_input;
            
            if(input != 0) {
                index = (link.input - inputs);
            }else{
                index = (link.input - *neurons.data());
            }
            
            os.write((char*)&index, sizeof(index));
            os.write((char*)&input, sizeof(input));
            os.write((char*)&link.weight, sizeof(link.weight));
        }
        
        os.write((char*)&n->bias, sizeof(n->bias));
        os.write((char*)&n->f, sizeof(n->f));
    }
    
    void read(std::ifstream& is, Neuron* n) {
        size_t index, size;
        int input;
        
        is.read((char*)&size, sizeof(size));
        n->inputs.resize(size);
        for(Neuron::Link& link : n->inputs) {
            is.read((char*)&index, sizeof(index));
            is.read((char*)&input, sizeof(input));
            is.read((char*)&link.weight, sizeof(link.weight));
            
            if(input != 0) {
                link.input = inputs + index;
            }else{
                link.input = *neurons.data() + index;
            }
        }
        
        is.read((char*)&n->bias, sizeof(n->bias));
        is.read((char*)&n->f, sizeof(n->f));
    }
    
    size_t input_size;
    size_t output_size;

public:
    
    Neuron* inputs;
    Neuron* outputs;
    
    Brain(size_t _input_size, size_t _output_size) : input_size(0), output_size(0) {
        reset(_input_size, _output_size);
    }
    
    Brain(const Brain& brain) = delete;
    
    Brain& operator = (const Brain& brain) = delete;
    
    inline ~Brain() {
        Free(inputs);
        Free(outputs);
        
        clear();
    }
    
    void reset(size_t _input_size, size_t _output_size) {
        assert(_input_size != 0);
        assert(_output_size != 0);
        
        clear();
        
        if(_input_size != input_size) {
            if(input_size != 0) Free(inputs);
            input_size = _input_size;
            inputs = (Neuron*)Alloc(sizeof(Neuron) * input_size);
            
            for(size_t i = 0; i < input_size; ++i) {
                inputs[i].flags = e_neuron_input | e_neuron_computed;
                Construct(inputs + i);
            }
        }
        
        if(_output_size != output_size) {
            if(output_size != 0) Free(outputs);
            output_size = _output_size;
            outputs = (Neuron*)Alloc(sizeof(Neuron) * output_size);
            
            for(size_t i = 0; i < output_size; ++i) {
                outputs[i].flags = e_neuron_output;
                Construct(outputs + i);
            }
        }
    }
    
    inline void write(std::ofstream& os) const {
        size_t size = neurons.size();
        os.write((char*)&input_size, sizeof(input_size));
        os.write((char*)&output_size, sizeof(output_size));
        os.write((char*)&size, sizeof(size));
        for(Neuron* neuron : neurons)
            write(os, neuron);
    }
    
    inline void read(std::ifstream& is) {
        size_t size;
        size_t i, o;
        
        is.read((char*)&i, sizeof(i));
        is.read((char*)&o, sizeof(o));
        
        reset(i, o);
        
        is.read((char*)&size, sizeof(size));
        
        clear();
        
        neurons.resize(size);
        
        for(Neuron*& neuron : neurons) {
            neuron = new Neuron();
            read(is, neuron);
        }
    }
    
    inline void clear() {
        for(Neuron* neuron : neurons)
            delete(neuron);
        
        neurons.clear();
        
        for(size_t i = 0; i < output_size; ++i)
            outputs[i].clear();
    }
    
    inline void compute() {
        for(Neuron* neuron : neurons)
            neuron->flags &= ~ e_neuron_computed;
        
        for(size_t i = 0; i < output_size; ++i)
            compute_value(outputs + i);
    }
    
    inline void alter(float scl) {
        for(Neuron* neuron : neurons)
            neuron->alter(scl);
        
        for(size_t i = 0; i < output_size; ++i)
            outputs[i].alter(scl);
    }
    
    friend void copy(Brain* dst, const Brain* src) {
        dst->reset(src->input_size, src->output_size);
        
        for(const Neuron* neuron : src->neurons) {
            dst->create_neuron(NULL, NULL, neuron->f.type);
        }
        
        for(Neuron* const neuron : src->neurons) {
            size_t index = (&neuron - src->neurons.data());
            dst->neurons[index]->bias = neuron->bias;
            for(const Neuron::Link& link : neuron->inputs) {
                if((link.input->flags & e_neuron_input) != 0) {
                    size_t index2 = (link.input - src->inputs);
                    dst->neurons[index]->add_link(dst->inputs + index2, link.weight);
                }else{
                    size_t index2 = (&link.input - src->neurons.data());
                    dst->neurons[index]->add_link(*(dst->neurons.data() + index2), link.weight);
                }
            }
        }
    }
    
    friend void alter(Brain* result, const Brain* brain) {
        copy(result, brain);
        
        /// 1/2 chance of altering
        
        /// 1/2 chance of changing function type of a random neuron
        
        /// 1/2 chance of adding a link (3/4) or adding a neuron (1/4)
        
        if((rand() & 0x1)) {
            int func_type = rand() % numberOfActivationFunctions;
            
            if((rand() & 0x1)) {
                size_t size = result->neurons.size();
                size_t index1 = rand() % (size + result->output_size);
                size_t index2 = rand() % (size + result->input_size + result->output_size);
                
                Neuron* n1 = index1 < size ? result->neurons[index1] : &result->outputs[index1 - size];
                
                Neuron* n2 = index2 < size ? result->neurons[index2] : ((index2 - size) < result->input_size ? &result->inputs[index2 - size] : &result->outputs[index2 - size - result->input_size]);
                
                /// n2 ... n1
                if((rand() & 0xf) < 0x4) {
                    /// remove any link
                    n1->remove_link(n2);
                    
                    result->create_neuron(n2, n1, func_type);
                }else{
                    if(!n1->has_link(n2))
                        n1->add_link(n2);
                }
            }else{
                size_t index = rand() % result->neurons.size();
                result->neurons[index]->f.type = func_type;
            }
        }
        
        result->alter(0.5f);
    }
    
};

#endif /* Brain_h */
