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
#include <fstream>

class BrainSystem
{
    
public:
    
    BrainSystem() : brains(NULL), count(0) {}
    
    BrainSystem(int size) {
        alloc(size);
        fill();
    }
    
    BrainSystem(const BrainSystem& bs) {
        alloc(bs.size());
        fill();
        
        for(int i = 0; i != count; ++i)
            *(brains[i]) = *(bs[i]);
    }
    
    ~BrainSystem() {
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
    
    inline void sort() {
        for(int i = 0; i != count; ++i) {
            float links = (float)brains[i]->numOfLinks();
            //brains[i]->reward *= 1.0f / (links + M_E);
        }
        
        std::sort(brains, brains + count, Brain::sort);
    }
    
    inline void replace() {
        float rewardsSum = 0.0f;
        for(int i = 0; i != count; ++i)
            rewardsSum += brains[i]->reward;
        
        if(rewardsSum <= FLT_EPSILON) {
            int half = count >> 1;
            
            for(int i = 0; i != half; ++i)
                *(brains[i]) = *(brains[count - i - 1]);
            
            return;
        }
        
        int begin = 0;
        int end = count;
        while(end-- > begin) {
            float frac = brains[end]->reward / rewardsSum;
            int step = frac * count;
            
            for(int i = 0; i != step; ++i) {
                if(begin >= count)
                    return;
                
                *(brains[begin]) = *(brains[end]);
                ++begin;
            }
        }
    }
    
    inline void mutate() {
        for(int i = 0; i != count; ++i)
            brains[i]->mutate();
    }
    
    inline void alter(float scl) {
        for(int i = 0; i != count; ++i)
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
    
    inline Brain* first() const {
        return brains[0];
    }
    
    inline Brain* last() const {
        return brains[count - 1];
    }
    
    inline void clear() {
        for(int i = 0; i != count; ++i)
            brains[i]->reward = 0.0f;
    }
    
    inline void write(std::ofstream& os) const {
        last()->write(os);
    }
    
    inline void read(std::ifstream& is) {
        Brain b;
        b.read(is);
        
        for(int i = 0; i != count; ++i)
            *(brains[i]) = b;
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
