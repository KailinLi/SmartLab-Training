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
#include <sys/timeb.h>
using namespace std;


void addNew (map<int, vector<int>> *E, int v1, int v2);
bool check (vector<int> *V, map<int, vector<int>> *E);
int sum (vector<vector<int>> *adjacent);
int main (int argc, char *argv[]) {
    
    map<int, vector<int>>E;
    
//    string getBuf = argv[1];
//    string getColor = argv[2];
//    
//    int key = stoi(getColor);
//
    string getBuf = "125.1";
    ifstream in("/Users/likailin/Desktop/Gurobi/SmartLab Training/GraphColoring/DSJC" + getBuf + ".col.txt");
    int key = 6;
    
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
    
    vector<int>VColor(number, 0);
    for_each(VColor.begin(), VColor.end(), [key](int &item) {
        item = rand() % key;
    });
    
    
    int conflict = 0;
    vector<vector<int>>adjacent;
    vector<vector<int>>tabu;
    for (int i = 0; i < number; ++i) {
        vector<int> Vconflict;
        vector<int> initTabu;
        for (int k = 0; k < key; ++k) {
            int cfs = 0;
            for (auto otherV : E[i]) {
                if (k == VColor[otherV]) {
                    ++cfs;
                }
            }
            if (k == VColor[i]) conflict += cfs;
            Vconflict.push_back(cfs);
            initTabu.push_back(0);
        }
        adjacent.push_back(Vconflict);
        tabu.push_back(initTabu);
    }
    
    struct timeb begin, end;
    ftime(&begin);
    
    int step = 0;
    
    //int historyBest = INT32_MAX;
    
    
    while (step != INT32_MAX) {
        int moveV = 0, moveC = 0, tmpTabu = 0;
        int maxChange = INT32_MIN;
        int count = 0;
        for (int i = 0; i < number; ++i) {
            for (int k = 0; k < key; ++k) {
                if (VColor[i] == k || tabu[i][k] > step) continue;
                tmpTabu = adjacent[i][VColor[i]] - adjacent[i][k];
                if (maxChange < tmpTabu) {
                    maxChange = tmpTabu;
                    moveV = i;
                    moveC = k;
                    count = 2;
                }
                else if (maxChange == tmpTabu) {
                    if (!(rand() % count)) {
                        moveV = i;
                        moveC = k;
                        ++count;
                    }
                }
            }
        }
        if (maxChange <= 0) {
            if (conflict == 2 * maxChange) break;
            else {
                tabu[moveV][VColor[moveV]] = rand() % number + step + number;
                moveV = rand() % number;
                moveC = rand() % key;
                maxChange = adjacent[moveV][VColor[moveV]] - adjacent[moveV][moveC];
            }
        }
        for (auto neightbour : E[moveV]) {
            --adjacent[neightbour][VColor[moveV]];
            ++adjacent[neightbour][moveC];
        }
        
        tabu[moveV][VColor[moveV]] = 1 + rand() % 5 + step;
        VColor[moveV] = moveC;
        conflict -= 2 * maxChange;
        //cout << step << " " << conflict << endl;
        ++step;
    }
    
    ftime(&end);
    int i = 0;
    for_each(VColor.begin(), VColor.end(), [&i](int item) {
        cout << ++i << ": ";
        cout << item << "    ";
        if (i % 6 == 0 && i != 1) cout << endl;
    });
    cout << endl;
    if (check(&VColor, &E)) {
        cout << "success" << endl;
    }
    else {
        cout << "fail" << endl;//sum(&adjacent) << endl;
    }
    
    cout << "time: " << (end.time - begin.time)*1000 + (end.millitm - begin.millitm)  << endl;
    
    cout << "iterator:" << step << endl;
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
