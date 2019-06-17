//
//  DynamicTree.cpp
//  Evolution
//
//  Created by Arthur Sun on 6/16/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "DynamicTree.hpp"

#include <stack>

DynamicTree::DynamicTree() {
    capacity = 256;
    count = 0;
    next = 0;
    root = -1;
    
    nodes = (TreeNode*)Alloc(sizeof(TreeNode) * capacity);
    
    freeFrom(0);
}

int DynamicTree::allocate_node() {
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
        freeFrom(count);
        
        next = count;
    }
    
    int node = next;
    
    nodes[node].child1 = -1;
    nodes[node].child2 = -1;
    
    next = nodes[node].next;
    
    return node;
}

void DynamicTree::insertProxy(int proxyId) {
    if(root == -1) {
        root = proxyId;
        nodes[root].parent = -1;
        return;
    }
    
    const AABB& aabb = nodes[proxyId].aabb;
    
    /// find the best sibling
    int sibling = root;
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
        if (nodes[child1].isLeaf()) {
            cost1 = aabb1.perimeter() + inheritanceCost;
        }else{
            float oldArea = nodes[child1].aabb.perimeter();
            float newArea = aabb1.perimeter();
            cost1 = (newArea - oldArea) + inheritanceCost;
        }
        
        /// Cost of descending into child2
        if (nodes[child2].isLeaf()) {
            cost2 = aabb2.perimeter() + inheritanceCost;
        }else{
            float oldArea = nodes[child2].aabb.perimeter();
            float newArea = aabb2.perimeter();
            cost2 = newArea - oldArea + inheritanceCost;
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
    nodes[newParent].child2 = proxyId;
    nodes[sibling].parent = newParent;
    nodes[proxyId].parent = newParent;
    
    /// Walk back up the tree fixing heights and AABBs
    sibling = nodes[proxyId].parent;
    while (sibling != -1) {
        sibling = balance(sibling);
        fix(sibling);
        sibling = nodes[sibling].parent;
    }
}

bool DynamicTree::moveProxy(int nodeId, const AABB &aabb) {
    assert(nodes[nodeId].isLeaf());
    
    if(nodes[nodeId].aabb.contains(aabb)) {
        return false;
    }
    
    removeProxy(nodeId);
    
    nodes[nodeId].aabb = aabb;
    
    insertProxy(nodeId);
    
    return true;
}

void DynamicTree::removeProxy(int leaf) {    
    if (leaf == root) {
        root = -1;
        free_node(leaf);
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
    
    if (grandParent != -1) {
        /// Destroy parent and connect sibling to grandParent.
        if (nodes[grandParent].child1 == parent) {
            nodes[grandParent].child1 = sibling;
        }else{
            nodes[grandParent].child2 = sibling;
        }
        
        nodes[sibling].parent = grandParent;
        free_node(parent);
        
        /// Adjust ancestor bounds.
        int index = grandParent;
        while (index != -1) {
            index = balance(index);
            fix(index);
            index = nodes[index].parent;
        }
    }else{
        root = sibling;
        nodes[sibling].parent = -1;
        free_node(parent);
    }
}

int DynamicTree::balance(int node) {
    if(nodes[node].height < 2) {
        return node;
    }
    
    int balance = getBalance(node);
    
    if(balance > +1) {
        int rightChild = nodes[node].child2;
        int subBalance = getBalance(rightChild);
        
        if(subBalance < 0)
            rotate_right(rightChild);
        
        return rotate_left(node);
    }
    
    if(balance < -1) {
        int leftChild = nodes[node].child1;
        int subBalance = getBalance(leftChild);
        
        if(subBalance > 0)
            rotate_left(leftChild);
        
        return rotate_right(node);
    }
    
    return node;
}

void DynamicTree::query(std::vector<void *> *list, const AABB &aabb) {
    std::stack<int> stack;
    
    stack.push(root);
    
    while(!stack.empty()) {
        int node = stack.top();
        stack.pop();
        
        if(node == -1)
            continue;
        
        if(nodes[node].isLeaf()) {
            if(touches(aabb, nodes[node].aabb))
                list->push_back(nodes[node].data);
            
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

void DynamicTree::query(std::vector<std::pair<void *, void *>> *list) {
    std::stack<std::pair<int, int>> stack;
    
    stack.push(std::pair<int, int>(root, root));
    
    while(!stack.empty()) {
        std::pair<int, int> node = stack.top();
        stack.pop();
        
        if(node.first == -1)
            continue;
        
        if(node.second == -1)
            continue;
        
        bool touch = touches(nodes[node.first].aabb, nodes[node.second].aabb);
        
        if(!touch) continue;
        
        if(nodes[node.first].isLeaf() && nodes[node.second].isLeaf()) {
            list->push_back(std::pair<void*, void*>(nodes[node.first].data, nodes[node.second].data));
            continue;
        }
        
        int child1 = nodes[node.first].child1;
        int child2 = nodes[node.first].child2;
        int child3 = nodes[node.second].child1;
        int child4 = nodes[node.second].child2;
        
        if(nodes[node.first].isLeaf()) {
            stack.push(std::pair<int, int>(node.first, child3));
            stack.push(std::pair<int, int>(node.first, child4));
            continue;
        }
        
        if(nodes[node.second].isLeaf()) {
            stack.push(std::pair<int, int>(node.second, child1));
            stack.push(std::pair<int, int>(node.second, child2));
            continue;
        }
        
        stack.push(std::pair<int, int>(child1, child1));
        stack.push(std::pair<int, int>(child1, child2));
        stack.push(std::pair<int, int>(child1, child3));
        stack.push(std::pair<int, int>(child1, child4));
        stack.push(std::pair<int, int>(child2, child2));
        stack.push(std::pair<int, int>(child2, child3));
        stack.push(std::pair<int, int>(child2, child4));
        stack.push(std::pair<int, int>(child3, child3));
        stack.push(std::pair<int, int>(child3, child4));
        stack.push(std::pair<int, int>(child4, child4));
    }
}
