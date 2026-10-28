#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathHeight(Node* n);

// Returns the height of the subtree rooted at n if all of its
// leaves are at the same depth, or -1 otherwise
int pathHeight(Node* n)
{
    if(n == nullptr) {
        return 0;
    }
    int leftHeight = pathHeight(n->left);
    int rightHeight = pathHeight(n->right);
    if(leftHeight == -1 || rightHeight == -1) {
        return -1;
    }
    // A missing child has no leaves, so only the other side matters
    if(n->left == nullptr) {
        return rightHeight + 1;
    }
    if(n->right == nullptr) {
        return leftHeight + 1;
    }
    if(leftHeight != rightHeight) {
        return -1;
    }
    return leftHeight + 1;
}


bool equalPaths(Node * root)
{
    // Add your code below
    return pathHeight(root) != -1;
}

