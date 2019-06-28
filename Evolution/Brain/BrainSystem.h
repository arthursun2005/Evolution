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
    
    inline BrainSystem() : brains(NULL), count(0) {}
    
    inline BrainSystem(int size) {
        alloc(size);
        fill();
    }
    
    inline BrainSystem(const BrainSystem& bs) {
        alloc(bs.size());
        fill();
        
        for(int i = 0; i != count; ++i)
            *(brains[i]) = *(bs[i]);
    }
    
    inline ~BrainSystem() {
        unfill();
        free();
    }
    
    inline void resize(int size) {
        if(size > count) {
            Brain** oldBrains = brains;
            int oldCount = count;
            
            alloc(size);
            
            for(int i = 0; i != oldCount; ++i)
                brains[i] = oldBrains[i];
            
            for(int i = oldCount; i != count; ++i)
                brains[i] = new Brain();
            
            if(oldCount > 0)
                Free(oldBrains);
        }else if(size < count) {
            Brain** oldBrains = brains;
            int oldCount = count;
            
            alloc(size);
            
            for(int i = 0; i != size; ++i)
                brains[i] = oldBrains[i];
            
            Free(oldBrains);
            
            for(int i = size; i != oldCount; ++i)
                delete oldBrains[i];
        }else{
            for(int i = 0; i != count; ++i)
                brains[i]->clear();
        }
    }
    
    inline void reset(int input_size, int output_size) {
        for(int i = 0; i != count; ++i)
            brains[i]->reset(input_size, output_size);
    }
    
    inline Brain* best() const {
        if(count < 1) return NULL;
        
        int idx = 0;
        
        for(int i = 1; i != count; ++i)
            if(brains[i]->reward > brains[idx]->reward)
                idx = i;
        
        return brains[idx];
    }
    
    inline void replace(int groupsize = 0) {
        if(count < 1) return;
        
        groupsize = groupsize < 1 ? default_groupsize : groupsize;
        
        Brain** _brains = (Brain**)Alloc(sizeof(Brain*) * count);
        
        for(int i = 0; i < count; ++i)
            _brains[i] = brains[i];
        
        for(int i = 0; i < count; ++i) {
            int index = rand() % count;
            for(int n = 1; n < groupsize; ++n) {
                int idx = rand() % count;
                if(_brains[idx]->reward > _brains[index]->reward)
                    index = idx;
            }
            
            *(brains[i]) = *(_brains[index]);
        }
        
        Free(_brains);
    }
    
    inline void produce(int groupsize = 0) {
        if(count < 1) return;
        
        groupsize = groupsize < 1 ? default_groupsize : groupsize;
        
        Brain** _brains = (Brain**)Alloc(sizeof(Brain*) * count);
        
        for(int i = 0; i < count; ++i)
            _brains[i] = brains[i];
        
        for(int i = 0; i < count; ++i) {
            int index1 = rand() % count;
            int index2 = rand() % count;
            
            if(_brains[index2]->reward > _brains[index1]->reward)
                std::swap(index1, index2);
            
            for(int n = 1; n < groupsize; ++n) {
                int idx = rand() % count;
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
        
        Free(_brains);
    }
    
    inline void mutate() {
        int half = count >> 1;
        
        for(int i = 0; i != half; ++i)
            brains[i]->mutate();
    }
    
    inline void alter(float scl) {
        int half = count >> 1;
        
        for(int i = 0; i != half; ++i)
            brains[i]->alter(scl);
    }
    
    inline void setRandom(float scl) {
        for(int i = 0; i != count; ++i)
            brains[i]->setRandom(scl);
    }
    
    inline Brain* operator [] (int i) const {
        return brains[i];
    }
    
    inline int size() const {
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
    
    inline void clear() {
        for(int i = 0; i != count; ++i)
            brains[i]->reward = 0.0f;
    }
    
    inline void write(std::ofstream& os) const {
        os.write((char*)&count, sizeof(count));
        
        for(int i = 0; i != count; ++i)
            brains[i]->write(os);
    }
    
    inline void read(std::ifstream& is) {
        int size;
        is.read((char*)&size, sizeof(size));
        
        for(int i = 0; i != count; ++i) {
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
    
    inline void reduce() {
        Brain *B = best();
        for(int i = 0; i != count; ++i) {
            if(brains[i] != B)
                *(brains[i]) = *B;
        }
    }
    
protected:
    
    inline void fill() {
        for(int i = 0; i != count; ++i)
            brains[i] = new Brain();
    }
    
    inline void unfill() {
        for(int i = 0; i != count; ++i)
            delete brains[i];
    }
    
    inline void free() {
        if(count > 0)
            Free(brains);
    }
    
    inline void alloc(int size) {
        count = size;
        if(count > 0)
            brains = (Brain**)Alloc(sizeof(Brain*) * count);
    }
    
    int count;
    Brain** brains;
    
};

#endif /* BrainSystem_h */
