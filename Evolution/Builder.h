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
    static const int winScore = 2 * complexity_tolerence;
    static const int killScore = 4;
    static constexpr float alterRate = 0.2f;
    
    int threshold = 10.0f;
    
    AABB aabb;
    
    Body* A;
    Body* B;
    
    BodyDef dA;
    BodyDef dB;
    
    float time;
    
    inline void initialize() {
        A->target = B;
        B->target = A;
        time = 0.0f;
    }
    
    void solve(float dt) {
        World::solveBodyBody(A, B, dt);
        World::solveBodyStick(A, &A->stick, dt);
        World::solveBodyStick(B, &B->stick, dt);
        World::solveBodyStick(A, &B->stick, dt);
        World::solveBodyStick(B, &A->stick, dt);
        World::solveStickStick(&A->stick, &B->stick, dt);
    }
    
    void step(float dt, int its) {
        time += dt;
        
        if(A->health <= 0.0f) {
            B->hits += roundf(killScore * winScore * (B->health / B->maxHealth));
            reset();
            smallAlter();
            time = 0.0f;
        }else if(B->health <= 0.0f) {
            A->hits += roundf(killScore * winScore * (A->health / A->maxHealth));
            reset();
            smallAlter();
            time = 0.0f;
        }else if(time >= threshold) {
            if(A->health > B->health)
                A->hits += roundf(winScore * ((A->health - B->health) / A->maxHealth));
            else
                B->hits += roundf(winScore * ((B->health - A->health) / B->maxHealth));
            
            reset();
            smallAlter();
            time = 0.0f;
        }
        
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
        
        A->constrain(aabb);
        B->constrain(aabb);
    }
    
    inline void smallAlter() {
        A->brain.alter(alterRate);
        B->brain.alter(alterRate);
    }
    
    void copyStatistics(Body* body, const BodyDef* def) {
        body->position = def->position;
        body->velocity = def->velocity;
        
        body->stick = def->stick;
        body->stick.owner = body;
        
        body->stick.position += body->position;
        body->stick.velocity += body->velocity;
        
        body->health = def->maxHealth;
        body->wound = 0.0f;
    }
    
    inline void reset() {
        copyStatistics(A, &dA);
        copyStatistics(B, &dB);
    }
    
};

class Builder : public BodySystem
{
    
public:
    
    float threshold = 600.0f;
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
                
                room.A->hits = 0;
                room.B->hits = 0;
                
                room.initialize();
                
                bodies.push_back(room.A);
                bodies.push_back(room.B);
                
                rooms.push_back(room);
            }
        }
        
        last = bodies.back();
        next = bodies.back()->color;
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
    
    int step(float dt, int col, int its) {
        time += dt;
        int score;
        
        score = -1;
        
        if(time >= threshold) {
            last->color = next;
            bodies.sort(Body::to_best);
            
            last = bodies.back();
            last->color = best;
            
            score = last->hits;
            
            iterator_type begin = this->begin();
            iterator_type end = this->end();
            
            int n = size();

            int i = n / 2;
            
            Body* A;
            while(i-- > 0) {
                --end;
                
                A = *begin;
                const Body* B = *end;
                
                Brain::alter(&A->brain, &B->brain);
                ++begin;
            }
            
            assert((n % 2) == 0);
            
            begin = this->begin();
            
            for(Room& room : rooms) {
                room.A = *begin;
                ++begin;
                
                room.B = *begin;
                ++begin;
                
                room.initialize();
                room.reset();
                room.A->hits = 0;
                room.B->hits = 0;
            }
            
            time = 0.0f;
            ++generation;
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
    
    inline int getBestBrainComplexity() const {
        return bodies.back()->brain.totalSize();
    }
    
    inline void write(std::ofstream& os) const {
        bodies.back()->brain.write(os);
    }
    
    inline void read(std::ifstream& is) {
        Brain B;
        B.read(is);
        for(Body* A : bodies)
            A->brain = B;
    }
    
    Body* last;
    Colorf next;
    Colorf best = Colorf(1.0f, 0.0f, 0.0f);
    
private:
    
    std::vector<Room> rooms;
    
    std::thread threads[builder_threads];
    
};

#endif /* Builder_h */
