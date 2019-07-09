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

#define builder_threads 8

struct Room {
    AABB aabb;
    
    Body* A;
    Body* B;
    
    BodyDef dA;
    BodyDef dB;
    
    float time;
    
    inline void initialize() {
        A->target = B;
        B->target = A;
    }
    
    void solve(float dt) {
        AABB bA = A->aabb();
        AABB bB = B->aabb();
        AABB bAs = A->stick.aabb();
        AABB bBs = B->stick.aabb();
        
        if(touches(bA, bB)) World::solveBodyBody(A, B, dt);
        if(touches(bA, bBs)) World::solveBodyStick(A, &B->stick, dt);
        if(touches(bB, bAs)) World::solveBodyStick(B, &A->stick, dt);
        if(touches(bAs, bBs)) World::solveStickStick(&A->stick, &B->stick, dt);
    }
    
    void step(float dt, int its) {
        A->setInputs(aabb);
        B->setInputs(aabb);
        
        A->think(dt);
        B->think(dt);
        
        dt /= (float) its;
        
        for(int i = 0; i < its; ++i) {
            solve(dt);
            A->step(dt);
            B->step(dt);
        }
        
        //A->constrain(aabb);
        //B->constrain(aabb);
    }
    
    void copyStatistics(Body* body, const BodyDef* def) {
        body->position = def->position;
        body->velocity = def->velocity;
        
        body->stick = def->stick;
        body->stick.owner = body;
        
        body->stick.position += body->position;
        body->stick.velocity += body->velocity;
        
        body->health = def->maxHealth;
    }
    
    inline void reset() {
        copyStatistics(A, &dA);
        copyStatistics(B, &dB);
    }
    
};

class Builder : public BodySystem
{
    
public:
    
    float subThreshold = 15.0f;
    float threshold = subThreshold * 8.0f;
    
    float subTime = 0.0f;
    float time = 0.0f;
    
    int generation = 0;
    
    Builder(int x, int y, float w, float h, const BodyDef& clone) {
        assert(x != 0 && y != 0);
        
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
                
                bodies.push_back(room.A);
                bodies.push_back(room.B);
                
                rooms.push_back(room);
            }
        }
        
        bs.resize(size());
        //bs.resize((uint)rooms.size() + 1);
        
        bs.reset(Body::input_size, Body::output_size);
        
        assign();

        bs.clear();
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
    
    inline void assign() {
        int i = 0;
        for(Room& room : rooms) {
            room.A->brain = bs[i];
            ++i;
            
            room.B->brain = bs[i];
            ++i;
            
            room.initialize();
        }
    }
    
    float step(float dt, int col, int its) {
        assert(dt <= subThreshold);
        
        time += dt;
        subTime += dt;
        
        float score = 0.0f;
        
        if(time >= threshold) {
            score = bs.best()->reward;
            
            bs.step();
            
            time = 0.0f;
            ++generation;
            
            bs.clear();
        }
        
        if(subTime >= subThreshold) {
            for(Room& R : rooms) {
                float K = (R.B->health - R.A->health);// * (1.0f + (R.A->brain->reward/(R.B->brain->reward + 1.0f)));
                R.A->brain->reward -= K;
                R.B->brain->reward += K;
            }
            
            bs.shuffle();
            
            assign();
            
            for(Room& R : rooms) {
                R.reset();
                
                //R.A->brain->mutate();
                //R.B->brain->mutate();
                
                //R.A->brain->setShared(randomf(-2.0f, 2.0f));
                //R.B->brain->setShared(randomf(-2.0f, 2.0f));
            }
            
            subTime = 0.0f;
        }
        
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
        
        return score;
    }
    
    inline Brain* getBestBrain() const {
        return bs.best();
    }
    
    inline void write(FILE* os) const {
        bs.write(os);
    }
    
    inline void write(std::ofstream& os) const {
        bs.write(os);
    }
    
    inline void read(FILE* is) {
        bs.read(is);
    }
    
    inline void read(std::ifstream& is) {
        bs.read(is);
    }
    
private:
        
    std::vector<Room> rooms;
    
    std::thread threads[builder_threads];
    
};

#endif /* Builder_h */
