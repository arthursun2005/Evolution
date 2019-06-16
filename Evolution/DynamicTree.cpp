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
    root = -1;
    
    nodes = (TreeNode*)Alloc(sizeof(TreeNode) * capacity);
    
    for(int i = 0; i != capacity - 1; ++i) {
        nodes[i].next = i + 1;
    }
    
    nodes[capacity - 1].next = -1;
}
