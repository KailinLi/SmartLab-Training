
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

//#define number 5
//#define p 3

#undef DEBUG

struct SortD {
    int index;
    int distance;
    SortD() : index(0), distance(0) {}
    SortD(int i, int d) : index(i), distance(d) {}
};
struct OrderSort {
    bool operator() (SortD const &a, SortD const &b) { return a.distance < b.distance; }
};
struct Pair {
    int addV;
    int deleteV;
    int distance;
    Pair() : addV(0), deleteV(0), distance(0) {}
    Pair(int aV, int dV, int d) : addV(aV), deleteV(dV), distance(d) {}
};
struct OrderPairDistance {
    bool operator() (Pair const &a, Pair const &b) { return a.distance > b.distance; }
};

inline int findMinDijkstra (vector<int> &way, vector<bool> &inPath);
inline int findMinSet (vector<SortD>&, int );
inline void makeTable (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&);
inline int findMax (vector<pair<SortD, SortD>>&, vector<int>&);
inline void addCenter (vector<pair<SortD, SortD>>&, int**, int);
inline int simulateDelete(vector<pair<SortD, SortD>>&, int);
inline void deleteCenter (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&, int);
inline SortD findSecond (list<int>&, vector<vector<SortD>>&, int);
int main () {
    
    /*
     *read file and init the 2nd array
     */
    int number = 0;
    int p;
    ifstream in("/Users/likailin/Desktop/Gurobi/SmartLab Training/PCenter/pmed21.txt");
    
    int in1, in2, in3;
    in >> in1 >> in2 >> in3;
    
    number = in1;
    int** distance = new int*[number];
    for(int i = 0; i < number; ++i)
        distance[i] = new int[number]{0};
    p = in3;
    
    while (!in.eof()) {
        in >> in1 >> in2 >> in3;
        distance[in1 - 1][in2 - 1] = in3;
        distance[in2 - 1][in1 - 1] = in3;
    }
    /*
     *Dijkstra
     */
    for (int current = 0; current < number; ++current) {
        vector<int>way(number, INT32_MAX);
        vector<bool>inPath(number, false);
        way[current] = 0;
        int time = 0;
        while (time < number - 1) {
            int item = findMinDijkstra(way, inPath);
            inPath[item] = true;
            for (int i = 0; i < number; ++i) {
                if (!inPath[i] && distance[item][i] != 0 && distance[item][i] + way[item] < way[i]) {
                    way[i] = distance[item][i] + way[item];
                }
            }
            ++time;
        }
        for (int index = 0; index < number; ++index) {
            distance[current][index] = way[index];
        }
    }
    
    /*
     *sort the distance
     */
    vector<vector<SortD>>SortDistance;
    for (int i = 0; i < number; ++i) {
        vector<SortD> sortSet;
        for (int j = 0; j < number; ++j) {
            sortSet.push_back(SortD(j, distance[i][j]));
        }
        stable_sort(sortSet.begin(), sortSet.end(), [](SortD x, SortD y) {return x.distance < y.distance;});
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
    
    /*
     *tabu table
     */
    int *tabuAdd = new int[number]{0};
    int *tabuDelete = new int[number]{0};
    int longest = 0;
    
    /*
     *save bestHistory
     */
    int historyBest = INT32_MAX;
    list<int> bestPCenter;
    
#pragma mark iterator
    int step = 0;
    while (step != 80000) {
        
        vector<int> maxVar;
        longest = findMax(table, maxVar);
        
#ifdef DEBUG
        for_each(pCenter.begin(), pCenter.end(), [](int i ) {cout << i << endl;});
        
        for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
        for_each(maxVar.begin(), maxVar.end(), [](int i) {cout << i << endl;});
        
        cout << "***" << endl;
#endif
        int current = maxVar[rand() % maxVar.size()];
        int lessCount = findMinSet(SortDistance[current], table[current].first.distance);
        
        priority_queue<Pair, vector<Pair>, OrderPairDistance> best;
        
        for (int addV = 0; addV < lessCount; ++addV) {
            /*
             *no conflict and not in the tabu table
             */
            if ((SortDistance[current][addV].index == table[current].first.index) || tabuDelete[SortDistance[current][addV].index] > step) continue;
            /*
             *store the table prevent to recover
             */
            vector<pair<SortD, SortD>>saveTable = table;
            addCenter(saveTable, distance, SortDistance[current][addV].index);
            
            int minLongest = INT32_MAX;
            vector<int> bestlist;
            for (auto center : pCenter) {
                if (tabuAdd[center] > step) continue;
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
            if (bestlist.empty()) continue;
            int deleteV = bestlist[rand() % bestlist.size()];
            best.push(Pair(SortDistance[current][addV].index, deleteV, minLongest));
        }
        if (best.empty()) {
            cout << "empty: " << step << endl;
            best.push(Pair(SortDistance[current][0].index, table[current].first.index, 0));
        }
        auto bestPair = best.top();
        best.pop();
        if (bestPair.distance >= longest) {
            if (historyBest > longest) {
                historyBest = longest;
                bestPCenter = pCenter;
            }
            int randNum = rand() % pCenter.size();
            auto dV = pCenter.begin();
            while (randNum > 0) {
                ++dV;
                --randNum;
            }
            int aV = rand() % number;
            while (find(pCenter.begin(), pCenter.end(), aV) != pCenter.end()) {
                aV = rand() % number;
            }
            bestPair = Pair(aV, *dV, distance[aV][*dV]);
        }
        else {
            vector<Pair>bestCandidate{bestPair};
            while (!best.empty() && bestPair.distance >= best.top().distance) {
                bestCandidate.push_back(best.top());
                best.pop();
            }
            bestPair = bestCandidate[rand() % bestCandidate.size()];
        }
        pCenter.erase(find(pCenter.begin(), pCenter.end(), bestPair.deleteV));
        pCenter.insert(pCenter.begin(), bestPair.addV);
        addCenter(table, distance, bestPair.addV);
        deleteCenter(table, pCenter, SortDistance, bestPair.deleteV);
        tabuAdd[bestPair.addV] = step + rand() % 10;
        tabuDelete[bestPair.deleteV] = step + rand() % 10;
        ++step;
        cout << step << endl;
    }
    if (historyBest > longest) {
        historyBest = longest;
        bestPCenter = pCenter;
    }
    table.clear();
    makeTable(table, bestPCenter, SortDistance);
    for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
    cout << "Min: " << historyBest  << "\nCenter: " << endl;
    for_each(bestPCenter.begin(), bestPCenter.end(), [](int i) {cout << i << endl;});
}

inline int findMinDijkstra (vector<int> &way, vector<bool> &inPath) {
    int min = INT32_MAX;
    int res = -1;
    for (int index = 0; index < way.size(); ++index) {
        if (!inPath[index] && way[index] < min) {
            min = way[index];
            res = index;
        }
    }
    return res;
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
    priority_queue<SortD, vector<SortD>, OrderSort>maxQ;
    for (int index = 0; index < table.size(); ++index) {
        maxQ.push(SortD(index, table[index].first.distance));
    }
    auto item = maxQ.top();
    maxQ.pop();
    maxVar.push_back(item.index);
    while (!maxQ.empty() && item.distance <= maxQ.top().distance) {
        maxVar.push_back(maxQ.top().index);
        maxQ.pop();
    }
    return item.distance;
}
inline void addCenter (vector<pair<SortD, SortD>>& table, int** distance, int addV) {
    for (int index = 0; index < table.size(); ++index) {
        if (distance[index][addV] < table[index].first.distance) {
            table[index].second = table[index].first;
            table[index].first = SortD(addV, distance[index][addV]);
        }
        else if (distance[index][addV] < table[index].second.distance) {
            table[index].second = SortD(addV, distance[index][addV]);
            if (table[index].second.distance == table[index].first.distance && table[index].second.index < table[index].first.index) {
                swap(table[index].second, table[index].first);
            }
        }
        else if (distance[index][addV] == table[index].second.distance) {
            if (distance[index][addV] == table[index].first.distance && addV < table[index].first.index) {
                table[index].second.index = table[index].first.index;
                table[index].first.index = addV;
            }
            else if (addV < table[index].second.index) {
                table[index].second.index = addV;
            }
        }
    }
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
        if (table[index].first.distance == table[index].second.distance && table[index].first.index > table[index].second.index) {
            swap(table[index].first, table[index].second);
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
