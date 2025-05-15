#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>

#include "Tree.h"

using namespace std;

bool do_run(ifstream &testcase, DistanceCalculator *calculator) {
  auto tree = new BinaryDimenTree(calculator);
  testcase >> *tree;

  int testNum;
  testcase >> testNum;
  bool res = true;
  for (int i = 0; i < testNum; i++) {
    double x, y;
    double z, w;
    testcase >> x;
    testcase >> y;
    testcase >> z;
    testcase >> w;
    auto target = new TreeNode({x, y});
    auto node = tree->find_nearest_node(target);
    auto coordinates = node->getCoordinates();
    auto answer = new TreeNode({z, w});
    if (answer->getCoordinates() != node->getCoordinates()) {
      cout << "case:" << x << " " << y << ",";
      cout << "expect:" << z << " " << w << ",";
      cout << "actual:" << coordinates[0] << " " << coordinates[1] << endl;
      res = false;
      break;
    }
  }
  delete tree;
  return res;
}

void run(string name) {
  ifstream testcase;
  testcase.open(name);

  string type;
  testcase >> type;
  bool ret = false;
  if (type == "Manhattan") {
    ManhattanDistanceCalculator calc;
    ret = do_run(testcase, &calc);
  } else if (type == "Euclidean") {
    EuclideanDistanceCalculator calc;
    ret = do_run(testcase, &calc);
  } else if (type == "Earth") {
    HaversineDistanceCalculator calc;
    ret = do_run(testcase, &calc);
  } else {
    cout << "Unknown test type!\n" << endl;
    ret = false;
  }
  if (ret) {
    cout << "pass at " << name << endl;
  } else {
    cout << "Failed at " << name << endl;
  }
  testcase.close();
}

int main() {
  /* You can change the testcase path as you like :) */
  /* run_testcase(<test_file_path>); */
  run("1.txt");
  run("2.txt");

  /* You are supposed to pass all of those ten testcases to get full grade */
  for (int i = 1; i <= 30; ++i) {
    string grade_test_file = "tests/c" + to_string(i);
    run(grade_test_file);
  }

  return 0;
}
