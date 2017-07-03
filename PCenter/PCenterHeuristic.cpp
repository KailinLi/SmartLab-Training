
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
#include <list>
#include <queue>
#include <ctime>
using namespace std;

#define number 5
#define p 3

struct SortD {
    int index;
    int distance;
    SortD(int i, int d) : index(i), distance(d) {}
};
inline int findMinSet (vector<SortD>&, int );
int main () {
    int distance[number][number] = {
        {0,1,3,2,2},
        {1,0,3,3,4},
        {3,3,0,2,1},
        {2,3,2,0,1},
        {2,4,1,1,0}
    };
    
    /*
     *sort the distance
     */
    vector<vector<SortD>>SortDistance;
    for (int i = 0; i < number; ++i) {
        vector<SortD> sortSet;
        for (int j = 0; j < number; ++j) {
            sortSet.push_back(SortD(j, distance[i][j]));
        }
        sort(sortSet.begin(), sortSet.end(), [](SortD x, SortD y) {return x.distance < y.distance;});
        SortDistance.push_back(sortSet);
    }
    
    /*
     *initial
     */
    list<int>pCenter;
    srand(static_cast<unsigned int>(time(NULL)));
    int current = rand() % number;
    pCenter.insert(pCenter.begin(), current);
    while (pCenter.size() < p) {
        auto center = *SortDistance[current].rbegin();
        int size = findMinSet(SortDistance[center.index], center.distance);
        do {
            current = SortDistance[center.index][rand() % size].index;
        } while (find(pCenter.begin(), pCenter.end(), current) != pCenter.end());
        pCenter.insert(pCenter.begin(), current);
    }
    for_each(pCenter.begin(), pCenter.end(), [](int i) {cout << i << endl;});
}
inline int findMinSet (vector<SortD>& sort, int current) {
    int begin = 0;
    int end = static_cast<int>(sort.size());
    int middle = (begin + end) / 2;
    for (; middle != begin && middle != end; middle = (begin + end) / 2) {
        if (sort[middle].distance <= current) {
            begin = middle;
        }
        else {
            end = middle;
        }
    }
    return (sort[middle].distance == current) ? middle + 1 : middle;
}
