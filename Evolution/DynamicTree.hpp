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

#include <stack>

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

struct Contact
{
    void* obj1;
    void* obj2;
};

/**
 ** Many algorithms came from Box2D
 ** https://github.com/erincatto/Box2D
 */

class DynamicTree
{
    /**
     ** allocate    =>      free
     ** create      =>      destory
     ** insert      =>      remove
     **/
    
    TreeNode* nodes;
    
    /// count of nodes
    /// pretty much useless
    int count;
    
    /// capacity of the pointer nodes
    int capacity;
    
    /// free list
    int next;
    
    /// root node
    int root;
    
    /// insert a leaf into the tree
    void insertProxy(int proxyId);
    
    /// free a single node
    /// it becomes the free list
    inline void free_node(int node) {
        nodes[node].next = next;
        nodes[node].height = -1;
        next = node;
        --count;
    }
    
    /// allocates a node
    int allocate_node();
    
    void freeFrom(int index) {
        for(int i = index; i != capacity - 1; ++i) {
            nodes[i].next = i + 1;
            nodes[i].height = -1;
        }
        
        nodes[capacity - 1].next = -1;
        nodes[capacity - 1].height = -1;
    }
    
    /// assumes nodes[node].isLeaf() == false
    /// fixes aabb and height
    inline void fix(int index) {
        int child1 = nodes[index].child1;
        int child2 = nodes[index].child2;
        nodes[index].aabb = combine_aabb(nodes[child1].aabb, nodes[child2].aabb);
        nodes[index].height = 1 + std::max(nodes[child1].height, nodes[child2].height);
    }
    
    int balance(int node);
    
    int rotate_left(int index) {
        int n = nodes[index].child2;
        int carry = nodes[n].child1;
        
        nodes[index].child2 = carry;
        nodes[n].child1 = index;
        
        int parent = nodes[index].parent;
        
        if(parent == -1) {
            root = n;
        }else{
            if(nodes[parent].child1 == index) {
                nodes[parent].child1 = n;
            }else{
                nodes[parent].child2 = n;
            }
        }
        
        nodes[n].parent = parent;
        nodes[carry].parent = index;
        nodes[index].parent = n;
        
        fix(index);
        fix(n);
        
        return n;
    }
    
    int rotate_right(int index) {
        int n = nodes[index].child1;
        int carry = nodes[n].child2;
        
        nodes[index].child1 = carry;
        nodes[n].child2 = index;
        
        int parent = nodes[index].parent;
        
        if(parent == -1) {
            root = n;
        }else{
            if(nodes[parent].child1 == index) {
                nodes[parent].child1 = n;
            }else{
                nodes[parent].child2 = n;
            }
        }
        
        nodes[n].parent = parent;
        nodes[carry].parent = index;
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
    
    void removeProxy(int leaf);
    
public:
    
    DynamicTree();
    
    inline ~DynamicTree() {
        Free(nodes);
    }
    
    DynamicTree(const DynamicTree& tree) = delete;
    
    DynamicTree& operator = (const DynamicTree& tree) = delete;
    
    /// creates a proxy and inserts it into the tree
    inline int createProxy(const AABB& aabb, void* data) {
        int node = allocate_node();
        nodes[node].height = 0;
        nodes[node].aabb = aabb;
        nodes[node].data = data;
        insertProxy(node);
        return node;
    }
    
    bool moveProxy(int nodeId, const AABB& aabb);
    
    inline void destoryProxy(int proxyId) {
        removeProxy(proxyId);
        free_node(proxyId);
    }
    
    void validateStructure();
    
    void validateSizes();
    
    void validate() {
        validateStructure();
        validateSizes();
        
        int freeCount = 0;
        int freeIndex = next;
        while (freeIndex != -1) {
            assert(0 <= freeIndex && freeIndex < capacity);
            freeIndex = nodes[freeIndex].next;
            ++freeCount;
        }
        
        assert(count + freeCount == capacity);
    }
    
    void query(std::vector<void*>* list, const AABB& aabb);
    
    void query(std::vector<Contact>* list);
    
};

#endif /* DynamicTree_hpp */
