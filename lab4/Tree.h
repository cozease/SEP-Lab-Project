#pragma once

#include <iostream>
#include <vector>

#include "Calculator.h"
#include "Comparator.h"
#include "TreeNode.h"

using namespace std;

class BinaryDimenTree {
  /* DO NOT CHANGE SIGNATURE */
  friend istream &operator>>(istream &in, BinaryDimenTree &tree);

private:
  /* data */
  int sz;
  DistanceCalculator *calculator;
  TreeNode *root;
//  void insert(TreeNode *&x, TreeNode *tmp, int level);
  void clear(TreeNode *x);
  void find(TreeNode *cur, TreeNode *&guess, double &bestDist, TreeNode *target, int level);
  void buildtree(vector<TreeNode*> &points, int left, int right, TreeNode *&x, int level);

public:
  /* methods */
  /* DO NOT CHANGE SIGNATURE */
  BinaryDimenTree(DistanceCalculator *calculator);

  /* DO NOT CHANGE SIGNATURE */
  TreeNode *find_nearest_node(TreeNode *target); 

  /* DO NOT CHANGE SIGNATURE */
  ~BinaryDimenTree(); /* DO NOT CHANGE */
};
