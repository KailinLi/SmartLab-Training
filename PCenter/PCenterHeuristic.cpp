
//
//  PCenterHeuristic.cpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/30.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#include "PCenterHeuristic.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <queue>
using namespace std;

#define number 5
#define p 3

struct ShortD {
    int index;
    int distance;
    ShortD(int i, int d) : index(i), distance(d) {}
};

int main () {
    int distance[number][number] = {
        {0,1,3,2,2},
        {1,0,3,3,4},
        {3,3,0,2,1},
        {2,3,2,0,1},
        {2,4,1,1,0}
    };
    auto cmp = [] (ShortD x, ShortD y) {return x.distance > y.distance;};
    set<ShortD, decltype(cmp)>SortDistance(cmp);
    
    
}
