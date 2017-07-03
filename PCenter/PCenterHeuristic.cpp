
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
    auto cmp = [] (ShortD x, ShortD y) {return x.distance < y.distance;};
    vector<multiset<ShortD, decltype(cmp)>>SortDistance;
    for (int i = 0; i < number; ++i) {
        multiset<ShortD, decltype(cmp)>sort(cmp);
        for (int j = 0; j < number; ++j) {
            sort.insert(ShortD(j, distance[i][j]));
        }
        SortDistance.push_back(sort);
    }
    
    for_each(SortDistance[2].begin(), SortDistance[2].end(), [](ShortD i) {cout << i.index << endl;});
}
