//
//  Builder.h
//  Evolution
//
//  Created by Arthur Sun on 6/22/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Builder_h
#define Builder_h

#include "World.hpp"
#include <thread>

#define builder_threads 32

struct Room {
    AABB aabb;
    
    Body* A;
    Body* B;
    
    BodyDef dA;
    BodyDef dB;
    
    float time;
    
    int threshold = 10.0f;
    
    inline void initialize() {
        A->target = B;
        B->target = A;
        time = 0.0f;
    }
    
    inline void solve(float dt) {
        World::solveBodyBody(A, B, dt);
        World::solveBodyStick(A, &A->stick, dt);
        World::solveBodyStick(B, &B->stick, dt);
        World::solveBodyStick(A, &B->stick, dt);
        World::solveBodyStick(B, &A->stick, dt);
        World::solveStickStick(&A->stick, &B->stick, dt);
    }
    
    void step(float dt, int its) {
        time += dt;
        
        A->setInputs();
        B->setInputs();
        
        A->think(dt);
        B->think(dt);
        
        dt /= (float) its;
        
        for(int i = 0; i < its; ++i) {
            solve(dt);
            A->step(dt);
            B->step(dt);
        }
        
        A->constrain(aabb);
        B->constrain(aabb);
        
        if(A->health <= 0.0f) {
            Brain brainA = A->brain;
            Brain brainB = B->brain;
            reset();
            A->brain = brainA;
            B->brain = brainB;
            smallAlter();
            ++B->hits;
        }else if(B->health <= 0.0f) {
            Brain brainA = A->brain;
            Brain brainB = B->brain;
            reset();
            A->brain = brainA;
            B->brain = brainB;
            smallAlter();
            ++A->hits;
        }else if(time >= threshold) {
            Brain brainA = A->brain;
            Brain brainB = B->brain;
            
            if(A->health > B->health)
                ++A->hits;
            else
                ++B->hits;
            
            reset();
            
            A->brain = brainA;
            B->brain = brainB;
            
            smallAlter();
        }
    }
    
    inline void smallAlter() {
        A->brain.alter();
        B->brain.alter();
    }
    
    inline void reset() {
        (*A) = Body(&dA);
        (*B) = Body(&dB);
        A->stick.owner = A;
        B->stick.owner = B;
        initialize();
    }
    
};

class Builder : public BodySystem
{
    
public:
    
    float threshold = 100.0f;
    float time = 0.0f;
    
    Builder(int x, int y, float w, float h, const BodyDef& clone) {
        float hx = x * 0.5f;
        float hy = y * 0.5f;
        vec2 hd = vec2(w * 0.5f, h * 0.5f);
        BodyDef bd = clone;
        for(int i = 0; i < x; ++i) {
            for(int j = 0; j < y; ++j) {
                Room room;
                vec2 p = vec2((i - hx) * w * 2.0f, (j - hy) * h * 2.0f);
                room.aabb = AABB(p - hd, p + hd);
                
                bd.position = p - vec2(w * 0.1666f, 0.0f);
                room.A = new Body(&bd);
                room.dA = bd;
                
                bd.position = p + vec2(w * 0.1666f, 0.0f);
                room.B = new Body(&bd);
                room.dB = bd;
                
                room.A->hits = 0;
                room.B->hits = 0;
                
                room.initialize();
                
                bodies.push_back(room.A);
                bodies.push_back(room.B);
                
                rooms.push_back(room);
            }
        }
    }
    
    ~Builder() {
        for(Body* body : bodies) {
            delete(body);
        }
    }
    
    inline void _step_range(int i, int n, float dt, int its) {
        int end = i + n;
        for(; i != end; ++i)
            rooms[i].step(dt, its);
    }
    
    inline void step_range(int i, int n, float dt, int col, int its) {
        dt /= (float) its;
        for(int k = 0; k != its; ++k)
            _step_range(i, n, dt, col);
    }
    
    void step(float dt, int col, int its) {
        time += dt;
        
        int work = (int)rooms.size();
        int i = 0;
        
        while(work != 0) {
            int chunk = (work - 1) / (builder_threads - i) + 1;
            if(chunk > work) chunk = work;
            work -= chunk;
            threads[i] = std::thread(&Builder::step_range, this, work, chunk, dt, col, its);
            ++i;
        }
        
        for(int j = 0; j != i; ++j)
            threads[j].join();
        
        if(time >= threshold) {
            bodies.sort(Body::to_best);
            
            for(Room& room : rooms) {
                room.reset();
            }
            
            iterator_type begin = this->begin();
            iterator_type end = this->end();
            
            int i = size() / 2;
            
            while(i-- > 0) {
                --end;
                Body* A = *begin;
                Body* B = *end;
                Brain::alter(&A->brain, &B->brain);
                ++begin;
            }
            
            time = 0.0f;
        }
    }
    
private:
    
    std::vector<Room> rooms;
    
    std::thread threads[builder_threads];
    
};

#endif /* Builder_h */
