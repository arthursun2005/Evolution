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

struct TreeNode
{
    int child1;
    int child2;
    
    int height;
    
    /// data to identify leaves for users
    void* data;
    
    /// `next` is only useful if the node is NOT in the tree ...
    union
    {
        /// parent
        int parent;
        
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
    
    void insertProxy(int proxyId);
    
    inline void free_node(int node) {
        nodes[node].next = next;
        nodes[node].height = -1;
        next = node;
    }
    
    int allocate_node();
    
    inline void freeFrom(int index) {
        for(int i = 0; i != capacity - 1; ++i) {
            nodes[i].next = i + 1;
            nodes[i].height = -1;
        }
        
        nodes[capacity - 1].next = -1;
        nodes[capacity - 1].height = -1;
    }
    
    /// nodes[node].isLeaf() == false
    inline void fix(int index) {
        int child1 = nodes[index].child1;
        int child2 = nodes[index].child2;
        nodes[index].aabb = combine_aabb(nodes[child1].aabb, nodes[child2].aabb);
        nodes[index].height = 1 + std::max(nodes[child1].height, nodes[child2].height);
    }
    
    int balance(int node);
    
    inline int rotate_left(int index) {
        int n = nodes[index].child2;
        nodes[index].child2 = nodes[n].child1;
        nodes[n].child1 = index;
        
        nodes[n].parent = nodes[index].parent;
        nodes[nodes[n].child1].parent = index;
        nodes[index].parent = n;
        
        fix(index);
        fix(n);
        
        return n;
    }
    
    inline int rotate_right(int index) {
        int n = nodes[index].child1;
        nodes[index].child1 = nodes[n].child2;
        nodes[n].child2 = index;
        
        nodes[n].parent = nodes[index].parent;
        nodes[nodes[n].child2].parent = index;
        nodes[index].parent = n;
        
        fix(index);
        fix(n);
        
        return n;
    }
    
    inline int getBalance(int node) const {
        int child1 = nodes[node].child1;
        int child2 = nodes[node].child2;
        
        return nodes[child2].height - nodes[child1].height;
    }
    
public:
    
    DynamicTree();
    
    inline ~DynamicTree() {
        Free(nodes);
    }
    
    DynamicTree(const DynamicTree& tree) = delete;
    
    DynamicTree& operator = (const DynamicTree& tree) = delete;
    
    inline int createProxy(const AABB& aabb, void* data) {
        int node = allocate_node();
        nodes[node].height = 0;
        nodes[node].aabb = aabb;
        nodes[node].data = data;
        insertProxy(node);
        return node;
    }
    
    bool moveProxy(int nodeId, const AABB& aabb);
    
    void removeProxy(int leaf);
    
    inline void destoryProxy(int proxyId) {
        removeProxy(proxyId);
        free_node(proxyId);
    }
    
    void query(std::vector<void*>* list, const AABB& aabb);
    
    void query(std::vector<std::pair<void *, void *>>* list);
    
};

#endif /* DynamicTree_hpp */
