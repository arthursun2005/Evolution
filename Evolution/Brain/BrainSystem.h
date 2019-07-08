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

#define default_groupsize 32

class BrainSystem
{
    
public:
    
    inline BrainSystem() : brains(NULL), count(0) {
        resize(0);
    }
    
    inline BrainSystem(uint size) : brains(NULL), count(0) {
        resize(size);
    }
    
    inline BrainSystem(const BrainSystem& bs) : brains(NULL), count(0) {
        resize(bs.size());
        
        for(uint i = 0; i != count; ++i)
            *(brains[i]) = *(bs[i]);
    }
    
    inline BrainSystem& operator = (const BrainSystem& bs) {
        resize(bs.size());
        
        for(uint i = 0; i != count; ++i)
            *(brains[i]) = *(bs[i]);
        
        return *this;
    }
    
    ~BrainSystem() {
        for(uint i = 0; i != count; ++i)
            delete brains[i];
        
        Free(brains);
    }
    
    void resize(uint size) {
        if(size == count) {
            for(uint i = 0; i != count; ++i)
                brains[i]->clear();
        }else{
            Brain** oldBrains = brains;
            uint oldCount = count;
            
            count = size;
            brains = new Brain*[size];
            
            if(count > oldCount) {
                for(uint i = 0; i != oldCount; ++i)
                    brains[i] = oldBrains[i];
                
                for(uint i = oldCount; i != count; ++i)
                    brains[i] = new Brain();
                
                if(oldBrains != NULL)
                    delete[] oldBrains;
            }else if(count < oldCount) {
                for(uint i = 0; i != count; ++i)
                    brains[i] = oldBrains[i];
                
                for(uint i = count; i != oldCount; ++i)
                    delete oldBrains[i];
                
                delete[] oldBrains;
            }
        }
    }
    
    inline void reset(uint input_size, uint output_size) {
        for(uint i = 0; i != count; ++i)
            brains[i]->reset(input_size, output_size);
    }
    
    inline Brain* best() const {
        if(count < 1)
            return NULL;
        
        uint idx = 0;
        
        for(uint i = 1; i != count; ++i)
            if(brains[i]->reward > brains[idx]->reward)
                idx = i;
        
        return brains[idx];
    }
    
    void step(uint groupsize = 0) {
        if(count < 1)
            return;
        
        groupsize = groupsize < 1 ? default_groupsize : groupsize;
        
        BrainSystem bs(*this);
        
        for(uint i = 0; i < count; ++i) {
            uint index = rand32(count);
            for(uint n = 1; n < groupsize; ++n) {
                uint idx = rand32(count);
                if(bs[idx]->reward > bs[index]->reward)
                    index = idx;
            }
            
            *(brains[i]) = *(bs[index]);
        }
        
        for(uint i = 0; i != count; ++i) {
            brains[i]->grow();
            
            if(rand32() & 1) brains[i]->mutate();
            if(rand32() & 1) brains[i]->generate();
        }
    }
    
    inline Brain* operator [] (uint i) const {
        return brains[i];
    }
    
    inline uint size() const {
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
        for(uint i = 0; i != count; ++i)
            brains[i]->reward = 0.0f;
    }
    
    void write(FILE* os) const {
        fwrite(&count, sizeof(count), 1, os);
        
        for(uint i = 0; i != count; ++i)
            brains[i]->write(os);
    }
    
    void read(FILE* is) {
        if(is == NULL) {
            throw std::invalid_argument("file not yet opened");
            return;
        }
        
        uint size;
        fread(&size, sizeof(size), 1, is);
        
        for(uint i = 0; i != count; ++i) {
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
    
protected:
    
    uint count;
    Brain** brains;
    
};

#endif /* BrainSystem_h */
