#include "Tree.h"
#include <algorithm>
#include <cassert>
#include <limits>

#include "Calculator.h"
#include "Comparator.h"
#include "TreeNode.h"

using namespace std;

/*
 * You can use this struct to compare a certain dimension of the tree nodes!
 * For example, if you want to compare the first dimension, you can simply call
 * DimComparator(0), such as DimComparator(0)(node_a, node_b).
 */
struct DimComparator {
  int dim;
  DimComparator(int d) : dim(d) {}
  bool operator()(TreeNode *a, TreeNode *b) const {
    assert(a->dimension() == b->dimension() && a->dimension() > dim);
    return isLessThan((*a)[dim], (*b)[dim]);
  }
};

BinaryDimenTree::BinaryDimenTree(DistanceCalculator *calculator) : calculator(calculator) {
  // TODO
  root = nullptr;
}

BinaryDimenTree::~BinaryDimenTree() {
  // TODO
  clear(root);
}

istream &operator>>(istream &in, BinaryDimenTree &tree) {
  // TODO
  in >> tree.sz;
  vector<TreeNode*> points(tree.sz, nullptr);
  for (int i = 0; i < tree.sz; ++i) {
      double x, y;
      in >> x >> y;
      points[i] = new TreeNode ({x, y});
  }
  tree.buildtree(points, 0, tree.sz - 1, tree.root, 0);
  return in;
}

void BinaryDimenTree::buildtree(vector<TreeNode*> &points, int left, int right, TreeNode *&x, int level) {
    if (left > right) return;
    sort(points.begin() + left, points.begin() + right + 1, DimComparator(level % 2));
    int mid = (left + right) / 2;
    x = points[mid];
    buildtree(points, left, mid - 1, x->left, level + 1);
    buildtree(points, mid + 1, right, x->right, level + 1);
}

void BinaryDimenTree::clear(TreeNode *x) {
    if (x == nullptr) return;
    clear(x->left);
    clear(x->right);
    delete x;
}

/*
void BinaryDimenTree::insert(TreeNode *&x, TreeNode *tmp, int level) {
    if (x == nullptr) {
        x = tmp;
        return;
    }
    if (DimComparator(level % 2)(tmp, x)) insert(x->left, tmp, level + 1);
    else insert(x->right, tmp, level + 1);
}
*/

TreeNode *BinaryDimenTree::find_nearest_node(TreeNode *target) {
    // TODO
    TreeNode *guess = nullptr;
    double bestDist = numeric_limits<double>::max();
    find(root, guess, bestDist, target, 0);
    return guess;
}

void BinaryDimenTree::find(TreeNode *cur, TreeNode *&guess, double &bestDist, TreeNode *target, int level) {
    if (cur == nullptr) return;
    if (isLessThan(calculator->calculateDistance(*cur, *target), bestDist)) {
        guess = cur;
        bestDist = calculator->calculateDistance(*cur, *target);
    } else if (isEqual(calculator->calculateDistance(*cur, *target), bestDist)) {
        if (DimComparator(0)(cur, guess)) {
            guess = cur;
            bestDist = calculator->calculateDistance(*cur, *target);
        } else if ((*cur)[0] == (*guess)[0]) {
            if (DimComparator(1)(cur, guess)) {
                guess = cur;
                bestDist = calculator->calculateDistance(*cur, *target);
            }
        }
    }
    if (DimComparator(level % 2)(target, cur)) {
        find(cur->left, guess, bestDist, target, level + 1);
        if (cur->right && isLessThanOrEqual(calculator->verticalDistance(*cur, *target, level % 2), bestDist))
            find(cur->right, guess, bestDist, target, level + 1);
    } else {
        find(cur->right, guess, bestDist, target, level + 1);
        if (cur->left && isLessThanOrEqual(calculator->verticalDistance(*cur, *target, level % 2), bestDist))
            find(cur->left, guess, bestDist, target, level + 1);
    }
}