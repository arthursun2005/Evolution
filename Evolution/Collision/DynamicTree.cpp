//
//  DynamicTree.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/16/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "DynamicTree.hpp"

DynamicTree::DynamicTree() {
    capacity = 256;
    count = 0;
    next = 0;
    root = null_node;
    
    nodes = (TreeNode*)Alloc(sizeof(TreeNode) * capacity);

    freeFrom(0);
}

int DynamicTree::allocate_node() {
    if(next == null_node) {
        /// not enough space for now ...
        
        /// current situation ...
        
        /**
         ** next     1   2   3   4   5 ... capacity - 2     capacity - 1
         ** index    0   1   2   3   4     capacity - 1     null_node
         **/
        
        assert(capacity == count);
        
        TreeNode* oldNodes = nodes;
        
        /// normally having memory in blocks of powers of 2 is better
        capacity *= 2;
        
        nodes = (TreeNode*)Alloc(sizeof(TreeNode) * capacity);
        
        /// copy old memory into new
        memcpy(nodes, oldNodes, sizeof(TreeNode) * count);
        
        /// free old
        ::operator delete (oldNodes);
        
        /// we dont't need to reset nodes[count - 1].next to count ...
        /// because the next we use it, it's going to be called by free_node ...
        /// which sets the .next member
        freeFrom(count);
        
        next = count;
    }
    
    int node = next;
    
    nodes[node].child1 = null_node;
    nodes[node].child2 = null_node;
    
    next = nodes[node].next;
    
    ++count;
    
    return node;
}

void DynamicTree::insertProxy(int proxyId) {
    if(root == null_node) {
        root = proxyId;
        nodes[root].parent = null_node;
        return;
    }
    
    AABB aabb = nodes[proxyId].aabb;
    
    /// find the best sibling
    int sibling = root;
    while(!nodes[sibling].isLeaf()) {
        int child1 = nodes[sibling].child1;
        int child2 = nodes[sibling].child2;
        
        float area = nodes[sibling].aabb.area();
        
        AABB combinedAABB = combine_aabb(nodes[sibling].aabb, aabb);
        
        float combinedArea = combinedAABB.area();
        
        /// Cost of creating a new parent for this node and the new leaf
        float cost = 2.0f * combinedArea;
        
        /// Minimum cost of pushing the leaf further down the tree
        float inheritanceCost = 2.0f * (combinedArea - area);
        
        AABB aabb1 = combine_aabb(nodes[child1].aabb, aabb);
        AABB aabb2 = combine_aabb(nodes[child2].aabb, aabb);
        
        float cost1, cost2;
        
        /// Cost of descending into child1
        if (nodes[child1].isLeaf()) {
            cost1 = aabb1.area() + inheritanceCost;
        }else{
            float oldArea = nodes[child1].aabb.area();
            float newArea = aabb1.area();
            cost1 = (newArea - oldArea) + inheritanceCost;
        }
        
        /// Cost of descending into child2
        if (nodes[child2].isLeaf()) {
            cost2 = aabb2.area() + inheritanceCost;
        }else{
            float oldArea = nodes[child2].aabb.area();
            float newArea = aabb2.area();
            cost2 = (newArea - oldArea) + inheritanceCost;
        }
        
        /// Descend according to the minimum cost.
        if (cost < cost1 && cost < cost2) {
            break;
        }
        
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
    int oldParent = nodes[sibling].parent;
    int newParent = allocate_node();
    nodes[newParent].parent = oldParent;
    nodes[newParent].aabb = combine_aabb(aabb, nodes[sibling].aabb);
    nodes[newParent].height = nodes[sibling].height + 1;
    
    if (oldParent != null_node)
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
    nodes[newParent].child2 = proxyId;
    nodes[sibling].parent = newParent;
    nodes[proxyId].parent = newParent;
    
    /// Walk back up the tree fixing heights and AABBs
    while (newParent != null_node) {
        newParent = balance(newParent);
        fix(newParent);
        newParent = nodes[newParent].parent;
    }
}

bool DynamicTree::moveProxy(int nodeId, const AABB &aabb, const vec2& displacement) {
    assert(nodes[nodeId].isLeaf());
    
    if(nodes[nodeId].aabb.contains(aabb))
        return false;
    
    removeProxy(nodeId);
    
    vec2 d = aabb_multipiler * vec2(fabs(displacement.x), fabs(displacement.y));
    
    AABB proxyAABB = aabb;
    proxyAABB.lowerBound -= d;
    proxyAABB.upperBound += d;
    nodes[nodeId].aabb = extendAABB(proxyAABB);
    
    insertProxy(nodeId);
    
    return true;
}

void DynamicTree::removeProxy(int leaf) {    
    if (leaf == root) {
        root = null_node;
        return;
    }
    
    int parent = nodes[leaf].parent;
    int grandParent = nodes[parent].parent;
    
    /// get sibling
    int sibling;
    if (nodes[parent].child1 == leaf) {
        sibling = nodes[parent].child2;
    }else{
        sibling = nodes[parent].child1;
    }
    
    if (grandParent != null_node) {
        /// Destroy parent and connect sibling to grandParent.
        if (nodes[grandParent].child1 == parent) {
            nodes[grandParent].child1 = sibling;
        }else{
            nodes[grandParent].child2 = sibling;
        }
        
        nodes[sibling].parent = grandParent;
        free_node(parent);
        
        /// Fix everything
        int index = grandParent;
        while (index != null_node) {
            index = balance(index);
            fix(index);
            index = nodes[index].parent;
        }
    }else{
        root = sibling;
        nodes[sibling].parent = null_node;
        free_node(parent);
    }
}

int DynamicTree::balance(int node) {
    if(nodes[node].height < 2)
        return node;

    int balance = getBalance(node);
    
    if(balance > 1) {
        int rightChild = nodes[node].child2;
        int subBalance = getBalance(rightChild);
        
        if(subBalance < 0)
            fix(rotate_right(rightChild));
        
        return rotate_left(node);
    }
    
    if(balance < null_node) {
        int leftChild = nodes[node].child1;
        int subBalance = getBalance(leftChild);
        
        if(subBalance > 0)
            fix(rotate_left(leftChild));
        
        return rotate_right(node);
    }
    
    return node;
}

void DynamicTree::query(std::vector<Contact> *list) {
    if(root == null_node) return;
    
    if(nodes[root].isLeaf()) return;
    
    Collector collector;
    
    collector.contacts = list;
    
    for(int i = 0; i < capacity; ++i) {
        if(nodes[i].height == 0) {
            collector.current = nodes[i].data;
            query(&collector, nodes[i].aabb);
        }
    }
}

void DynamicTree::validateStructure() {
    if(root == null_node) return;
    
    std::stack<int> stack;
    
    stack.push(root);
    
    assert(nodes[root].parent == null_node);
    
    while(!stack.empty()) {
        int node = stack.top();
        stack.pop();
        
        if(node == null_node)
            continue;
        
        int child1 = nodes[node].child1;
        int child2 = nodes[node].child2;
        
        if(nodes[node].isLeaf()) {
            assert(child1 == null_node);
            assert(child2 == null_node);
            assert(nodes[node].height == 0);
            continue;
        }
        
        assert(nodes[child1].parent == node);
        assert(nodes[child2].parent == node);
        
        stack.push(child1);
        stack.push(child2);
    }
}

void DynamicTree::validateSizes() {
    if(root == null_node) return;
    
    std::stack<int> stack;
    
    stack.push(root);
    
    assert(nodes[root].parent == null_node);
    
    while(!stack.empty()) {
        int node = stack.top();
        stack.pop();
        
        if(node == null_node)
            continue;
        
        int child1 = nodes[node].child1;
        int child2 = nodes[node].child2;
        
        if(nodes[node].isLeaf()) {
            continue;
        }
        
        AABB aabb = combine_aabb(nodes[child1].aabb, nodes[child2].aabb);
        int height = 1 + std::max(nodes[child1].height, nodes[child2].height);
        
        /// each bit in each float should be the same
        assert(aabb.lowerBound == nodes[node].aabb.lowerBound);
        assert(aabb.upperBound == nodes[node].aabb.upperBound);
        
        assert(height == nodes[node].height);
        
        stack.push(child1);
        stack.push(child2);
    }
}

float DynamicTree::getAreaRatio() const {
    if (root == null_node)
        return 0.0f;
    
    float rootArea = nodes[root].aabb.area();
    
    float totalArea = 0.0f;
    for (int i = 0; i < capacity; ++i) {
        if (nodes[i].height < 0)
            continue;
        
        totalArea += nodes[i].aabb.area();
    }
    
    return totalArea / rootArea;
}

int DynamicTree::rotate_left(int index) {
    int n = nodes[index].child2;
    int carry = nodes[n].child1;
    
    nodes[index].child2 = carry;
    nodes[n].child1 = index;
    
    int parent = nodes[index].parent;
    
    if(parent == null_node) {
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
    
    return n;
}

int DynamicTree::rotate_right(int index) {
    int n = nodes[index].child1;
    int carry = nodes[n].child2;
    
    nodes[index].child1 = carry;
    nodes[n].child2 = index;
    
    int parent = nodes[index].parent;
    
    if(parent == null_node) {
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
    
    return n;
}

void DynamicTree::validate() {
    validateStructure();
    validateSizes();
    
    int freeCount = 0;
    int freeIndex = next;
    while (freeIndex != null_node) {
        assert(0 <= freeIndex && freeIndex < capacity);
        freeIndex = nodes[freeIndex].next;
        ++freeCount;
    }
    
    assert(count + freeCount == capacity);
    
    if(root != null_node)
        assert(nodes[root].height == computeHeight(root));
}

int DynamicTree::getMaxBalance() const  {
    int maxBalance = 0;
    for (int i = 0; i < capacity; ++i) {
        if (nodes[i].height <= 1)
            continue;
        
        assert(nodes[i].isLeaf() == false);
        
        maxBalance = std::max(maxBalance, getBalance(i));
    }
    
    return maxBalance;
}
