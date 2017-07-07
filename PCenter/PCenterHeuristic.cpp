
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
#include <sys/timeb.h>
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
//struct OrderSort {
//    bool operator() (SortD const &a, SortD const &b) { return a.distance < b.distance; }
//};
struct Pair {
    int addV;
    int deleteV;
    int distance;
    Pair() : addV(0), deleteV(0), distance(0) {}
    Pair(int aV, int dV, int d) : addV(aV), deleteV(dV), distance(d) {}
};
//struct OrderPairDistance {
//    bool operator() (Pair const &a, Pair const &b) { return a.distance > b.distance; }
//};

inline int findMinDijkstra (vector<int> &way, vector<bool> &inPath);
inline int findMinSet (vector<SortD>&, int );
inline void makeTable (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&);
inline int findMax (vector<pair<SortD, SortD>>&, size_t&);
inline void addCenter (vector<pair<SortD, SortD>>&, int**, int);
inline int simulateDelete(vector<pair<SortD, SortD>>&, int);
inline void deleteCenter (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&, int);
inline SortD findSecond (list<int>&, vector<vector<SortD>>&, int);
int main (int argc, char *argv[]) {
    
    /*
     *read file and init the 2nd array
     */
    int number = 0;
    int p;
    //ifstream in("/Users/likailin/Desktop/Gurobi/SmartLab Training/PCenter/pmed2.txt");
    
    string file = argv[1];
    
    int answer = stoi(argv[2]);
    
    
    ifstream in("pmed" + file + ".txt");
    
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
            sortSet.emplace_back(j, distance[i][j]);
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
    
    struct timeb begin, end;
    ftime(&begin);
#pragma mark iterator
    int step = 0;
    while (step != 9000000) {
        
        //vector<int> maxVar;
        size_t current;
        longest = findMax(table, current);
        
#ifdef DEBUG
        for_each(pCenter.begin(), pCenter.end(), [](int i ) {cout << i << endl;});
        
        for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
        for_each(maxVar.begin(), maxVar.end(), [](int i) {cout << i << endl;});
        
        cout << "***" << endl;
#endif
        int lessCount = findMinSet(SortDistance[current], table[current].first.distance);
        
//        priority_queue<Pair, vector<Pair>, OrderPairDistance> best;
        Pair best(-1, -1, INT32_MAX);
        int Pcount = 0;
        
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
            int deleteV = -1;
            int count = 0;
            for (auto center : pCenter) {
                if (tabuAdd[center] > step) continue;
                int value = simulateDelete(saveTable, center);
                if (minLongest > value) {
                    minLongest = value;
                    deleteV = center;
                    count = 2;
                }
                else if (minLongest == value) {
                    if (!(rand() % count)) {
                        deleteV = center;
                        ++count;
                    }
                }
            }
            if (deleteV == -1) continue;
            if (best.distance > minLongest) {
                best.addV = SortDistance[current][addV].index;
                best.deleteV = deleteV;
                best.distance = minLongest;
                Pcount = 2;
            }
            else if (best.distance == minLongest) {
                if (!(rand() % count)) {
                    best.addV = SortDistance[current][addV].index;
                    best.deleteV = deleteV;
                    ++Pcount;
                }
            }
        }
        if (best.addV == -1) {
            //cout << "empty: " << step << endl;
            best.addV = SortDistance[current][0].index;
            best.deleteV = table[current].first.index;
            best.distance = 0;
        }
        
        if (best.distance >= longest) {
            if (historyBest > longest) {
                historyBest = longest;
                bestPCenter = pCenter;
                if (historyBest <= answer) break;
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
            best.addV = aV;
            best.deleteV = *dV;
            best.distance = distance[aV][*dV];
        }

        pCenter.erase(find(pCenter.begin(), pCenter.end(), best.deleteV));
        pCenter.insert(pCenter.begin(), best.addV);
        addCenter(table, distance, best.addV);
        deleteCenter(table, pCenter, SortDistance, best.deleteV);
        tabuAdd[best.addV] = step + rand() % 10;
        tabuDelete[best.deleteV] = step + rand() % 10;
        ++step;
        //cout << step << endl;
        
    }
    ftime(&end);
    if (historyBest > longest) {
        historyBest = longest;
        bestPCenter = pCenter;
    }
    table.clear();
    makeTable(table, bestPCenter, SortDistance);
    for_each(table.begin(), table.end(), [](pair<SortD, SortD> item) {cout << item.first.index << " " << item.first.distance << " | " << item.second.index << " " << item.second.distance << endl;});
    cout << "Center: " << endl;
    for_each(bestPCenter.begin(), bestPCenter.end(), [](int i) {cout << i << endl;});
    cout << "Min: " << historyBest  << endl;
    cout << "time: " << (end.time - begin.time)*1000 + (end.millitm - begin.millitm)  << endl;
    cout << "iteration: " << step << endl;
    for (int i = 0; i < number; ++i) {
        delete distance[i];
    }
    delete[] distance;
    delete [] tabuAdd;
    delete [] tabuDelete;
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
inline int findMax (vector<pair<SortD, SortD>>& table, size_t& current) {
    int tmpMax = INT32_MIN;
    int count = 0;
    for (size_t i = 0; i < table.size(); ++i) {
        if (tmpMax < table[i].first.distance) {
            tmpMax = table[i].first.distance;
            current = i;
            count = 2;
        }
        else if (tmpMax == table[i].first.distance) {
            if (!(rand() % count)) {
                current = i;
                ++count;
            }
        }
    }
    return tmpMax;
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
