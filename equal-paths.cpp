#ifndef RECCHECK
// if you want to add any #includes like <iostream> you must do them here
// (before the next endif)
#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here

int getHeight(Node *root) {
  if (root == nullptr) {
    return 0;
  }
  int heightLeft = getHeight(root->left);
  int heightRight = getHeight(root->right);
  return 1 + (heightLeft > heightRight ? heightLeft : heightRight);
}

bool equalPaths(Node *root) {
  // Add your code below

  if (root == nullptr) {
    return true;
  }

  int leftHeight = getHeight(root->left);
  int rightHeight = getHeight(root->right);

  if (leftHeight == 0) {
    return equalPaths(root->right);
  } else if (rightHeight == 0) {
    return equalPaths(root->left);
  } else if (leftHeight != rightHeight) {
    return false;
  }

  return equalPaths(root->left) && equalPaths(root->right);
}
