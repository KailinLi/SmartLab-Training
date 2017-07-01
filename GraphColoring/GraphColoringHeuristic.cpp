//
//  GraphColoring.cpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/29.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#include "GraphColoringHeuristic.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <ctime>
using namespace std;

//#define count 4
#define key 15

void addNew (map<int, vector<int>> *E, int v1, int v2);
bool check (vector<int> *V, map<int, vector<int>> *E);
int sum (vector<vector<int>> *adjacent);
int main () {
    
    map<int, vector<int>>E;

    ifstream in("DSJC250.1.col.txt");
    int v1, v2;
    int number = 0;
    string buf;
    if (in) {
        while (!in.eof()) {
            do {
                in >> buf;
            } while (buf != "e" && buf != "p");
            if (buf == "p") {
                in >> buf >> number;
                continue;
            }
            in >> v1;
            in >> v2;
            --v1;
            --v2;
            addNew(&E, v1, v2);
            addNew(&E, v2, v1);
        }
    }
    srand(static_cast<unsigned int>(time(NULL)));
    vector<int>V(number, 0);
    for_each(V.begin(), V.end(), [](int &item) {
        item = rand() % key;
    });
    vector<vector<int>>adjacent;
    vector<vector<int>>tabu;
    for (int i = 0; i < number; ++i) {
        vector<int> conflict;
        vector<int> initTabu;
        for (int k = 0; k < key; ++k) {
            int cfs = 0;
            for (auto otherV : E[i]) {
                if (k == V[otherV]) {
                    cfs++;
                }
            }
            conflict.push_back(cfs);
            initTabu.push_back(0);
        }
        adjacent.push_back(conflict);
        tabu.push_back(initTabu);
    }
    int step = 0;
    while (step != INT32_MAX) {
        int moveV = 0, moveC = 0, tmpTabu = 0;
        int maxChange = INT32_MIN;
        int tmp = 0;
        for (int i = 0; i < number; ++i) {
            tmp |= adjacent[i][V[i]];
            for (int k = 0; k < key; ++k) {
                if (tabu[i][k] > step) continue;
                tmpTabu = adjacent[i][V[i]] - adjacent[i][k];
                if (maxChange < tmpTabu) {
                    maxChange = tmpTabu;
                    moveV = i;
                    moveC = k;
                }
            }
        }
        if (maxChange == 0) {
            if (!tmp) break;
            else {
                tabu[moveV][V[moveV]] = rand() % number + step + number;
                moveV = rand() % number;
                moveC = rand() % key;
            }
        }
        for (auto neightbour : E[moveV]) {
            --adjacent[neightbour][V[moveV]];
            ++adjacent[neightbour][moveC];
        }
        
        tabu[moveV][V[moveV]] = 1 + rand() % 5 + step;
        V[moveV] = moveC;
        cout << step << " " << maxChange << endl;
        ++step;
    }
    int i = 0;
    for_each(V.begin(), V.end(), [&i](int item) {
        cout << ++i << ": ";
        cout << item << "    ";
        if (i % 6 == 0 && i != 1) cout << endl;
    });
    cout << endl;
    if (check(&V, &E)) {
        cout << "success" << endl;
    }
    else {
        cout << "fail" << endl;//sum(&adjacent) << endl;
    }
}
void addNew (map<int, vector<int>> *E, int v1, int v2) {
    auto item = (*E).find(v1);
    if (item == (*E).end()) {
        vector<int>insert{v2};
        (*E).insert(make_pair(v1, insert));
    }
    else {
        (item->second).push_back(v2);
    }
}
bool check (vector<int> *V, map<int, vector<int>> *E) {
    for (int i = 0; i < (*E).size(); ++i) {
        for (auto n : (*E)[i]) {
            if ((*V)[i] == (*V)[n]) {
                return false;
            }
        }
    }
    return true;
}
//int sum (vector<vector<int>> *adjacent) {
//    int sum = 0;
//    for (auto i : (*adjacent)) {
//        for (auto j : i) {
//            sum += j;
//        }
//    }
//    return sum;
//}

