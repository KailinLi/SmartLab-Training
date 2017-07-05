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
#include <sys/timeb.h>
#include <ctime>
using namespace std;

//#define count 4
//#define key 8

struct bestMove {
    int moveV;
    int moveC;
    int gap;
    bestMove(int v, int c, int g) : moveV(v), moveC(c), gap(g) {}
};

void addNew (map<int, vector<int>> *E, int v1, int v2);
bool check (vector<int> *V, map<int, vector<int>> *E);
int sum (vector<vector<int>> *adjacent);
int main (int argc, char *argv[]) {
    
    map<int, vector<int>>E;
    
    string getBuf = argv[1];
    string getColor = argv[2];
    
    int key = stoi(getColor);

    ifstream in("DSJC" + getBuf + ".col.txt");
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
    vector<vector<int>>adjacent;
    vector<vector<int>>tabu;
    for (int i = 0; i < number; ++i) {
        vector<int> conflict;
        vector<int> initTabu;
        for (int k = 0; k < key; ++k) {
            int cfs = 0;
            for (auto otherV : E[i]) {
                if (k == VColor[otherV]) {
                    cfs++;
                }
            }
            conflict.push_back(cfs);
            initTabu.push_back(0);
        }
        adjacent.push_back(conflict);
        tabu.push_back(initTabu);
    }
    
    struct timeb begin, end;
    ftime(&begin);
    int step = 0;
    
    int historyBest = INT32_MAX;
    while (step != 9999999) {
//        int moveV = 0, moveC = 0, tmpTabu = 0;
//        int maxChange = INT32_MIN;
        int conflict = 0;
        vector<bestMove> move;
        for (int i = 0; i < number; ++i) {
            conflict += adjacent[i][VColor[i]];
            for (int k = 0; k < key; ++k) {
                if (tabu[i][k] > step) continue;
                if (move.empty()) {
                    move.push_back(bestMove(i, k, adjacent[i][VColor[i]] - adjacent[i][k]));
                }
                else if (move.back().gap > adjacent[i][VColor[i]] - adjacent[i][k]) {
                    continue;
                }
                else if (move.back().gap < adjacent[i][VColor[i]] - adjacent[i][k]) {
                    move.clear();
                }
                move.push_back(bestMove(i, k, adjacent[i][VColor[i]] - adjacent[i][k]));
//                tmpTabu = adjacent[i][VColor[i]] - adjacent[i][k];
//                if (maxChange < tmpTabu) {
//                    maxChange = tmpTabu;
//                    moveV = i;
//                    moveC = k;
//                }
            }
        }
        auto item = move[rand() % move.size()];
        if (item.gap == 0) {
            if (!conflict) break;
            else {
                historyBest = min(historyBest, conflict);
                tabu[item.moveV][VColor[item.moveV]] = conflict + rand() % 7 + step;
                int disturb = 1 + rand() % 2;
                for (int i = 0; i < disturb; ++i) {
                    int randV = rand() % number;
                    int randC = rand() % key;
                    tabu[randV][VColor[randC]] = rand() % 7 + step + conflict;
                    for (auto neightbour : E[randV]) {
                        --adjacent[neightbour][VColor[randV]];
                        ++adjacent[neightbour][randC];
                    }
                    VColor[randV] = randC;
                }
                cout << step << " " << conflict << endl;
                ++step;
                continue;
            }
        }
        for (auto neightbour : E[item.moveV]) {
            --adjacent[neightbour][VColor[item.moveV]];
            ++adjacent[neightbour][item.moveC];
        }
        
        tabu[item.moveV][VColor[item.moveV]] = item.gap + rand() % 7 + step;
        VColor[item.moveV] = item.moveC;
        cout << step << " " << conflict << endl;
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
    cout << "History Best: " << historyBest << endl;
    cout << "time: " << (end.time - begin.time)*1000 + (end.millitm - begin.millitm)  << endl;
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

