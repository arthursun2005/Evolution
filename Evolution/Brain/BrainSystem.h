//
//  BrainSystem.h
//  Evolution
//
//  Created by Arthur Sun on 6/24/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef BrainSystem_h
#define BrainSystem_h

#include "Brain.h"
#include <algorithm>
#include <fstream>

#define default_groupsize 16

class BrainSystem
{
    
public:
    
    inline BrainSystem() : brains(NULL), count(0) {}
    
    inline BrainSystem(size_t size) : brains(NULL), count(0) {
        resize(size);
    }
    
    inline BrainSystem(const BrainSystem& bs) : brains(NULL), count(0) {
        resize(bs.size());
        
        for(size_t i = 0; i != count; ++i)
            *(brains[i]) = *(bs[i]);
    }
    
    inline BrainSystem& operator = (const BrainSystem& bs) {
        resize(bs.size());
        
        for(size_t i = 0; i != count; ++i)
            *(brains[i]) = *(bs[i]);
        
        return *this;
    }
    
    ~BrainSystem() {
        if(brains != NULL) {
            for(size_t i = 0; i != count; ++i)
                delete brains[i];
            
            delete[] brains;
            brains = NULL;
        }
    }
    
    void resize(size_t size) {
        if(size == count) {
            for(size_t i = 0; i != count; ++i)
                brains[i]->clear();
        }else{
            Brain** oldBrains = brains;
            size_t oldCount = count;
            
            count = size;
            brains = new Brain*[size];
            
            if(count > oldCount) {
                for(size_t i = 0; i != oldCount; ++i)
                    brains[i] = oldBrains[i];
                
                for(size_t i = oldCount; i != count; ++i)
                    brains[i] = new Brain();
                
                if(oldBrains != NULL)
                    delete[] oldBrains;
            }else if(count < oldCount) {
                for(size_t i = 0; i != count; ++i)
                    brains[i] = oldBrains[i];
                
                for(size_t i = count; i != oldCount; ++i)
                    delete oldBrains[i];
                
                delete[] oldBrains;
            }
        }
    }
    
    inline void reset(size_t input_size, size_t output_size) {
        for(size_t i = 0; i != count; ++i)
            brains[i]->reset(input_size, output_size);
    }
    
    inline Brain* best() const {
        if(count < 1)
            return NULL;
        
        size_t idx = 0;
        
        for(size_t i = 1; i != count; ++i)
            if(brains[i]->reward > brains[idx]->reward)
                idx = i;
        
        return brains[idx];
    }
    
    void replace(size_t groupsize = 0) {
        if(count < 1)
            return;
        
        groupsize = groupsize < 1 ? default_groupsize : groupsize;
        
        Brain** _brains = new Brain*[count];
        
        for(size_t i = 0; i < count; ++i)
            _brains[i] = brains[i];
        
        for(size_t i = 0; i < count; ++i) {
            size_t index = rand64() % count;
            for(size_t n = 1; n < groupsize; ++n) {
                size_t idx = rand64() % count;
                if(_brains[idx]->reward > _brains[index]->reward)
                    index = idx;
            }
            
            *(brains[i]) = *(_brains[index]);
        }
        
        delete[] _brains;
    }
    
    void produce(size_t groupsize = 0) {
        if(count < 1)
            return;
        
        groupsize = groupsize < 1 ? default_groupsize : groupsize;
        
        Brain** _brains = new Brain*[count];

        for(size_t i = 0; i < count; ++i)
            _brains[i] = brains[i];
        
        for(size_t i = 0; i < count; ++i) {
            size_t index1 = rand64() % count;
            size_t index2 = rand64() % count;
            
            if(_brains[index2]->reward > _brains[index1]->reward)
                std::swap(index1, index2);
            
            for(size_t n = 1; n < groupsize; ++n) {
                size_t idx = rand64() % count;
                float r = _brains[idx]->reward;
                
                if(r > _brains[index1]->reward) {
                    index2 = index1;
                    index1 = idx;
                }else if(r > _brains[index2]->reward) {
                    index2 = idx;
                }
            }
            
            Brain::produce(brains[i], _brains[index1], _brains[index2]);
        }
        
        delete[] _brains;
    }
    
    inline void mutate() {
        size_t half = count >> 1;
        
        for(size_t i = 0; i != half; ++i)
            brains[i]->mutate();
    }
    
    inline void alter(float scl) {
        size_t half = count >> 1;
        
        for(size_t i = 0; i != half; ++i)
            brains[i]->alter(scl);
    }
    
    inline void setRandom(float scl) {
        for(size_t i = 0; i != count; ++i)
            brains[i]->setRandom(scl);
    }
    
    inline Brain* operator [] (size_t i) const {
        return brains[i];
    }
    
    inline size_t size() const {
        return count;
    }
    
    inline Brain** data() const {
        return brains;
    }
    
    inline Brain** begin() const {
        return brains;
    }
    
    inline Brain** end() const {
        return brains + count;
    }
    
    void clear() {
        for(size_t i = 0; i != count; ++i)
            brains[i]->reward = 0.0f;
    }
    
    void write(std::ofstream& os) const {
        os.write((char*)&count, sizeof(count));
        
        for(size_t i = 0; i != count; ++i)
            brains[i]->write(os);
    }
    
    void read(std::ifstream& is) {
        size_t size;
        is.read((char*)&size, sizeof(size));
        
        for(size_t i = 0; i != count; ++i) {
            if(i >= size) {
                *(brains[i]) = *(brains[i % size]);
            }else{
                brains[i]->read(is);
            }
        }
    }
    
    inline void shuffle() {
        std::random_shuffle(brains, brains + count);
    }
    
    void reduce() {
        Brain *B = best();
        for(size_t i = 0; i != count; ++i) {
            if(brains[i] != B)
                *(brains[i]) = *B;
        }
    }
    
protected:
    
    size_t count;
    Brain** brains;
    
};

#endif /* BrainSystem_h */
