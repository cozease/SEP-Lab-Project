#include <stdexcept>

#include "TreeNode.h"

TreeNode::TreeNode(initializer_list<double> coords) {
  // TODO
  coordinates.assign(coords);
  left = nullptr;
  right = nullptr;
}

const double &TreeNode::operator[](int index) const {
  // TODO
  return coordinates[index];
}

int TreeNode::dimension() const {
  // TODO
  return 2;
}

const vector<double> &TreeNode::getCoordinates() const {
  // TODO
  return coordinates;
}

TreeNode::~TreeNode() {} // Even though the implementation might be empty, it's
                         // a good practice to define it.