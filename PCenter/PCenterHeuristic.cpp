
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
#define p 2

struct SortD {
    int index;
    int distance;
    SortD() : index(0), distance(0) {}
    SortD(int i, int d) : index(i), distance(d) {}
};
struct Pair {
    int addV;
    int deleteV;
    int distance;
    bool operator < (const Pair &x) const {
        return distance > x.distance;
    }
    Pair() : addV(0), deleteV(0), distance(0) {}
    Pair(int aV, int dV, int d) : addV(aV), deleteV(dV), distance(d) {}
};
inline int findMinSet (vector<SortD>&, int );
inline void makeTable (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&);
inline int findMax (vector<pair<SortD, SortD>>&, vector<int>&);
inline int addCenter (vector<pair<SortD, SortD>>&, int**, int);
inline int simulateDelete(vector<pair<SortD, SortD>>&, int);
inline void deleteCenter (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&, int);
inline SortD findSecond (list<int>&, vector<vector<SortD>>&, int);
int main () {
    
    int** distance = new int*[number];
    distance[0] = new int[number]{0,1,3,2,2};
    distance[1] = new int[number]{1,0,3,3,4};
    distance[2] = new int[number]{3,3,0,2,5};
    distance[3] = new int[number]{2,3,2,0,4};
    distance[4] = new int[number]{2,4,5,4,0};
    
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
    
    
#pragma mark iterator
    int step = 0;
    while (step != 100) {
        vector<int> maxVar;
        int longest = findMax(table, maxVar);
        for_each(pCenter.begin(), pCenter.end(), [](int i ) {cout << i << endl;});
        
        for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
        for_each(maxVar.begin(), maxVar.end(), [](int i) {cout << i << endl;});
        
        cout << "***" << endl;
        int current = maxVar[rand() % maxVar.size()];
        int lessCount = findMinSet(SortDistance[current], table[current].first.distance);
        
        priority_queue<Pair> best;
        
        for (int addV = 0; addV < lessCount; ++addV) {
            if (SortDistance[current][addV].index == table[current].first.index) continue;
            vector<pair<SortD, SortD>>saveTable = table;
            addCenter(saveTable, distance, SortDistance[current][addV].index);
            int minLongest = INT32_MAX;
            int deleteV = 0;
            vector<int> bestlist;
            for (auto center : pCenter) {
                int value = simulateDelete(saveTable, center);
                if (minLongest > value) {
                    minLongest = value;
                    bestlist.clear();
                    bestlist.push_back(center);
                }
                else if (minLongest == value) {
                    bestlist.push_back(center);
                }
            }
            deleteV = bestlist[rand() % bestlist.size()];
            best.push(Pair(SortDistance[current][addV].index, deleteV, minLongest));
        }
        
        auto bestPair = best.top();
        if (bestPair.distance >= longest) {
            break;
        }
        pCenter.erase(find(pCenter.begin(), pCenter.end(), bestPair.deleteV));
        pCenter.insert(pCenter.begin(), bestPair.addV);
        addCenter(table, distance, bestPair.addV);
        deleteCenter(table, pCenter, SortDistance, bestPair.deleteV);
        
        ++step;
    }
    vector<int>tmp;
    cout << "Min: " << findMax(table, tmp) << endl;
    for_each(pCenter.begin(), pCenter.end(), [](int i) {cout << i << endl;});
    for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
    
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
inline int findMax (vector<pair<SortD, SortD>>& table, vector<int>& maxVar) {
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
    return max;
}
inline int addCenter (vector<pair<SortD, SortD>>& table, int** distance, int addV) {
    int longest = INT32_MIN;
    for (int index = 0; index < table.size(); ++index) {
        if (distance[index][addV] < table[index].first.distance) {
            if (longest < distance[index][addV]) longest = distance[index][addV];
            table[index].second = table[index].first;
            table[index].first = SortD(addV, distance[index][addV]);
        }
        else if (distance[index][addV] < table[index].second.distance) {
            table[index].second = SortD(addV, distance[index][addV]);
        }
    }
    return longest;
}
inline int simulateDelete(vector<pair<SortD, SortD>>& table, int deleteV) {
    int longest = INT32_MIN;
    for (auto item : table) {
        if (item.first.index == deleteV) {
            longest = max(longest, item.second.distance);
        }
        else {
            longest = max(longest, item.first.distance);
        }
    }
    return longest;
}
inline void deleteCenter (vector<pair<SortD, SortD>>& table, list<int>& pCenter, vector<vector<SortD>>& SortDistance, int deleteV) {
    for (int index = 0; index < table.size(); ++index) {
        if (table[index].first.index == deleteV) {
            table[index].first = table[index].second;
            table[index].second = findSecond(pCenter, SortDistance, index);
        }
        else if (table[index].second.index == deleteV) {
            table[index].second = findSecond(pCenter, SortDistance, index);
        }
    }
}
inline SortD findSecond (list<int>& pCenter, vector<vector<SortD>>& SortDistance, int index) {
    SortD second;
    int i = 0;
    auto item = SortDistance[index];
    while (i < item.size()) {
        if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
            ++i;
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
    return second;
}
