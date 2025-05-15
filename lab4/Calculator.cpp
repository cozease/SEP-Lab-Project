#include "Calculator.h"
#include <cassert>
#include <cmath>

double ManhattanDistanceCalculator::calculateDistance(const TreeNode &nodeA,
                                               const TreeNode &nodeB) const {
  // TODO
  return double(fabs(nodeA[0] - nodeB[0]) + fabs(nodeA[1] - nodeB[1]));
}

double ManhattanDistanceCalculator::verticalDistance(TreeNode &root,
                                                     TreeNode &target,
                                                     int dim) const {
  // TODO
  return double(fabs(root[dim] - target[dim]));
}

double
EuclideanDistanceCalculator::calculateDistance(const TreeNode &nodeA,
                                               const TreeNode &nodeB) const {
  // TODO
  return double(sqrt((nodeA[0] - nodeB[0]) * (nodeA[0] - nodeB[0]) + (nodeA[1] - nodeB[1]) * (nodeA[1] - nodeB[1])));
}

double EuclideanDistanceCalculator::verticalDistance(TreeNode &root,
                                                     TreeNode &target,
                                                     int dim) const {
  // TODO
  return double(fabs(root[dim] - target[dim]));
}

double HaversineDistanceCalculator::deg2rad(double deg) const {
  return (deg * M_PI / 180.0);
}

double
HaversineDistanceCalculator::calculateDistance(const TreeNode &nodeA,
                                               const TreeNode &nodeB) const {
  assert(nodeA.dimension() == 2 && nodeB.dimension() == 2);

  double lat1_rad = this->deg2rad(nodeA[1]);
  double lat2_rad = this->deg2rad(nodeB[1]);
  double lng1_rad = this->deg2rad(nodeA[0]);
  double lng2_rad = this->deg2rad(nodeB[0]);

  double dlat = lat2_rad - lat1_rad;
  double dlng = lng2_rad - lng1_rad;

  double a = sin(dlat / 2) * sin(dlat / 2) +
             cos(lat1_rad) * cos(lat2_rad) * sin(dlng / 2) * sin(dlng / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  return EARTH_RADIUS * c;
}

double HaversineDistanceCalculator::verticalDistance(TreeNode &root,
                                                     TreeNode &target,
                                                     int dim) const {
  assert(root.dimension() == 2 && target.dimension() == 2);
  assert(dim >= 0 && dim < 2);
  double delta = std::fabs(root[dim] - target[dim]);
  double factor = dim ? 1 : cos(deg2rad(target[1]));
  return deg2rad(delta) * EARTH_RADIUS * factor;
}