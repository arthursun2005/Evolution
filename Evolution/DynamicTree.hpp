//
//  DynamicTree.hpp
//  Evolution
//
//  Created by Arthur Sun on 6/16/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef DynamicTree_hpp
#define DynamicTree_hpp

#include "vec2.h"
#include "common.h"

#define aabb_extension 0.005f

/// axis aligned bounding box
struct AABB
{
    vec2 lowerBound;
    vec2 upperBound;
    
    inline AABB() {}
    
    inline AABB(const vec2& lowerBound, const vec2& upperBound) : lowerBound(lowerBound), upperBound(upperBound) {}
    
    inline float area() const {
        return (upperBound.x - lowerBound.x) * (upperBound.y - lowerBound.y);
    }
    
    inline float perimeter() const {
        return 2.0f * (upperBound.x - lowerBound.x + upperBound.y - lowerBound.y);
    }
};

inline vec2 min(const vec2& a, const vec2& b) {
    return vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}

inline vec2 max(const vec2& a, const vec2& b) {
    return vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}

inline AABB combine_aabb(const AABB& a, const AABB& b) {
    return AABB(min(a.lowerBound, b.lowerBound), max(a.upperBound, b.upperBound));
}

inline bool touches(const AABB& a, const AABB& b) {
    vec2 d1 = b.upperBound - a.lowerBound;
    vec2 d2 = a.upperBound - b.lowerBound;
    return firstbit(d1.x) * firstbit(d1.y) * firstbit(d2.x) * firstbit(d2.y);
}

inline AABB extendAABB(const AABB& aabb) {
    static const vec2 extension = vec2(aabb_extension, aabb_extension);
    return AABB(aabb.lowerBound - extension, aabb.upperBound + extension);
}

struct TreeNode
{
    int child1;
    int child2;
    
    /// `next` is only useful if the node is NOT in the tree ...
    union
    {
        /// an id to identify leaves for users
        int id;
        
        /// used as a linked list element ...
        int next;
    };
    
    AABB aabb;
    
    inline bool isLeaf() const {
        return child1 == -1;
    }
};

/**
 ** Many algorithms came from Box2D
 ** https://github.com/erincatto/Box2D
 */

class DynamicTree
{
    
    TreeNode* nodes;
    
    int count;
    int capacity;
    
    int next;
    int root;
    
    int insert_proxy(const AABB& aabb, int id) {
        int node = allocate_node();
        nodes[node].aabb = aabb;
        nodes[node].id = id;
        
        if(root == -1) {
            root = node;
            return node;
        }
        
        /// find the best sibling
        int sibling = root;
        int oldParent = -1;
        
        while(!nodes[sibling].isLeaf()) {
            int child1 = nodes[sibling].child1;
            int child2 = nodes[sibling].child2;
            
            float area = nodes[sibling].aabb.perimeter();
            
            AABB combinedAABB = combine_aabb(nodes[sibling].aabb, aabb);
            
            float combinedArea = combinedAABB.perimeter();
            
            /// Cost of creating a new parent for this node and the new leaf
            float cost = 2.0f * combinedArea;
            
            /// Minimum cost of pushing the leaf further down the tree
            float inheritanceCost = 2.0f * (combinedArea - area);
            
            AABB aabb1 = combine_aabb(nodes[child1].aabb, aabb);
            AABB aabb2 = combine_aabb(nodes[child2].aabb, aabb);
            
            float cost1, cost2;
            
            /// Cost of descending into child1
            if (nodes[child1].isLeaf())
            {
                cost1 = aabb1.perimeter() + inheritanceCost;
            }
            else
            {
                float oldArea = nodes[child1].aabb.perimeter();
                float newArea = aabb1.perimeter();
                cost1 = (newArea - oldArea) + inheritanceCost;
            }
            
            /// Cost of descending into child2
            if (nodes[child2].isLeaf())
            {
                cost2 = aabb2.perimeter() + inheritanceCost;
            }
            else
            {
                float oldArea = nodes[child2].aabb.perimeter();
                float newArea = aabb2.perimeter();
                cost2 = newArea - oldArea + inheritanceCost;
            }
            
            /// Descend according to the minimum cost.
            if (cost < cost1 && cost < cost2)
            {
                break;
            }
            
            oldParent = sibling;
            
            nodes[oldParent].aabb = combine_aabb(nodes[oldParent].aabb, aabb);
            
            /// Descend
            if (cost1 < cost2)
            {
                sibling = child1;
            }
            else
            {
                sibling = child2;
            }
        }
        
        /// Create a new parent.
        int newParent = allocate_node();
        nodes[newParent].aabb = combine_aabb(aabb, nodes[sibling].aabb);
        
        if (oldParent != -1)
        {
            /// The sibling was not the root.
            if (nodes[oldParent].child1 == sibling)
            {
                nodes[oldParent].child1 = newParent;
            }
            else
            {
                nodes[oldParent].child2 = newParent;
            }
        }
        else
        {
            /// The sibling was the root.
            root = newParent;
        }
        
        nodes[newParent].child1 = sibling;
        nodes[newParent].child2 = node;
        
        nodes[newParent].aabb = combine_aabb(aabb, nodes[sibling].aabb);
        
        return node;
    }
    
    inline void free_node(int node) {
        nodes[node].next = next;
        next = node;
    }
    
    inline int allocate_node() {
        if(next == -1) {
            /// not enough space for now ...
            
            /// current situation ...
            
            /**
             ** next     1   2   3   4   5 ... capacity - 2     capacity - 1
             ** index    0   1   2   3   4     capacity - 1     -1
             **/
            
            assert(capacity == count);
            TreeNode* oldNodes = nodes;
            
            /// normally having memory in blocks of powers of 2 is better
            capacity *= 2;
            
            nodes = (TreeNode*)Alloc(sizeof(TreeNode) * capacity);
            
            /// copy old memory into new
            memcpy(nodes, oldNodes, sizeof(TreeNode) * count);
            
            /// free old
            Free(oldNodes);
            
            /// we dont't need to reset nodes[count - 1].next to count ...
            /// because the next we use it, it's going to be called by free_node ...
            /// which sets the .next member
            for(int i = count; i != capacity - 1; ++i) {
                nodes[i].next = i + 1;
            }
            
            nodes[capacity - 1].next = -1;
            
            next = count;
        }
        
        int node = next;
        
        next = nodes[node].next;
        
        return node;
    }
    
public:
    
    DynamicTree();
    
    inline ~DynamicTree() {
        Free(nodes);
    }
    
    DynamicTree(const DynamicTree& tree) = delete;
    
    DynamicTree& operator = (const DynamicTree& tree) = delete;
    
    inline int insertProxy(const AABB& aabb, int id) {
        return insert_proxy(aabb, id);
    }
    
};

#endif /* DynamicTree_hpp */
