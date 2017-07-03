
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
    SortD() : index(0), distance(0) {}
    SortD(int i, int d) : index(i), distance(d) {}
};
inline int findMinSet (vector<SortD>&, int );
inline void makeTable (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&);
inline void findMax (vector<pair<SortD, SortD>>&, vector<int>&);
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
    
    /*
     *make d table and f table
     */
    vector<pair<SortD, SortD>>table;
    makeTable(table, pCenter, SortDistance);
    
    for_each(pCenter.begin(), pCenter.end(), [](int i ) {cout << i << endl;});
    
    for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
    
    int step = 0;
    while (step != 100) {
        vector<int> maxVar;
        findMax(table, maxVar);
        for_each(maxVar.begin(), maxVar.end(), [](int i) {cout << i << endl;});
        break;
        //++step;
    }
    
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
inline void makeTable (vector<pair<SortD, SortD>>& table, list<int>& pCenter, vector<vector<SortD>>& SortDistance) {
    for (int index = 0; index < SortDistance.size(); ++index) {
        SortD best;
        SortD second;
        int i = 0;
        auto item = SortDistance[index];
        while (i < item.size()) {
            if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
                best = item[i++];
                break;
            }
            ++i;
        }
        while (i < item.size()) {
            if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
                second = item[i];
                break;
            }
            ++i;
        }
        table.push_back(make_pair(best, second));
    }
}
inline void findMax (vector<pair<SortD, SortD>>& table, vector<int>& maxVar) {
    int max = INT32_MIN;
    for (int index = 0; index < table.size(); ++index) {
        if (table[index].first.distance == max) {
            maxVar.push_back(index);
        }
        else if (table[index].first.distance > max) {
            max = table[index].first.distance;
            maxVar.clear();
            maxVar.push_back(index);
        }
    }
}
