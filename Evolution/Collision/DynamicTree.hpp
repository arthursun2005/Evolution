//
//  DynamicTree.hpp
//  Evolution
//
//  Created by Arthur Sun on 6/16/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef DynamicTree_hpp
#define DynamicTree_hpp

#include "Collision.h"

#include <stack>

#define null_node -1

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
        return child1 == null_node;
    }
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
        nodes[node].height = null_node;
        next = node;
        --count;
    }
    
    /// allocates a node
    int allocate_node();
    
    void freeFrom(int index) {
        for(int i = index; i != capacity - 1; ++i) {
            nodes[i].next = i + 1;
            nodes[i].height = null_node;
        }
        
        nodes[capacity - 1].next = null_node;
        nodes[capacity - 1].height = null_node;
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
    
    int rotate_left(int index);
    
    int rotate_right(int index);
    
    inline int getBalance(int node) const {
        int child1 = nodes[node].child1;
        int child2 = nodes[node].child2;
        return nodes[child2].height - nodes[child1].height;
    }
    
    void removeProxy(int leaf);
    
    int computeHeight(int nodeId) const {
        assert(0 <= nodeId && nodeId < capacity);
        TreeNode* node = nodes + nodeId;
        
        if (node->isLeaf())
            return 0;
        
        int height1 = computeHeight(node->child1);
        int height2 = computeHeight(node->child2);
        return 1 + std::max(height1, height2);
    }
    
public:
    
    struct Collector
    {
        std::vector<Contact>* contacts;
        
        void* current;
        
        bool callback(void* data) {
            if(data > current) {
                Contact contact;
                contact.obj1 = current;
                contact.obj2 = data;
                contacts->push_back(contact);
            }
            return true;
        }
    };
    
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
    
    bool moveProxy(int nodeId, const AABB& aabb, const vec2& displacement);
    
    inline void destoryProxy(int proxyId) {
        removeProxy(proxyId);
        free_node(proxyId);
    }
    
    void validateStructure();
    
    void validateSizes();
    
    void validate();
    
    int getMaxBalance() const;
    
    float getAreaRatio() const;
    
    template <class T>
    void query(T* callback, const AABB& aabb);
    
    void query(std::vector<Contact>* list);
    
};

template <class T>
void DynamicTree::query(T *callback, const AABB &aabb) {
    std::stack<int> stack;
    
    stack.push(root);
    
    while(!stack.empty()) {
        int node = stack.top();
        stack.pop();
        
        if(node == null_node)
            continue;
        
        if(nodes[node].isLeaf()) {
            if(touches(aabb, nodes[node].aabb)) {
                if(!callback->callback(nodes[node].data))
                    return;
            }
            
            continue;
        }
        
        int child1 = nodes[node].child1;
        int child2 = nodes[node].child2;
        
        if(touches(aabb, nodes[child1].aabb))
            stack.push(child1);
        
        if(touches(aabb, nodes[child2].aabb))
            stack.push(child2);
    }
}

#endif /* DynamicTree_hpp */
