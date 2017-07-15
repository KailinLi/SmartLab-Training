//
//  GraphColoring.cpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/29.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#include "GraphColoringHeuristic.hpp"

inline void init (vector<vector<int>> &E, vector<int> &V, int k, int number);
inline void addNew (vector<vector<int>> &E, int v1, int v2);
inline bool check (vector<int> &V, vector<vector<int>> &E);
int main (int argc, char *argv[]) {
    
    /*
     *read file and init
     */
    vector<vector<int>>E;
    
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
                E = vector<vector<int>>(number);
                continue;
            }
            in >> v1;
            in >> v2;
            --v1;
            --v2;
            addNew(E, v1, v2);
            addNew(E, v2, v1);
        }
    }
    
    /*
     *get random
     */
    random_device rd;
    default_random_engine R(rd());

    
    /*
     *make table
     */
    
    vector<int> VColor(number, 0);
    
    init(E, VColor, key, number);
    
    
//    int conflict = 0;
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
//            if (k == VColor[i]) conflict += cfs;
            Vconflict.push_back(cfs);
            initTabu.push_back(0);
        }
        adjacent.push_back(Vconflict);
        tabu.push_back(initTabu);
    }
    
    clock_t begin = clock();
    
    int step = 0;
    
    /*
     *begin iterator
     */
    
    while (step != 900000000) {
        int moveV = 0, moveC = 0, tmpTabu = 0;
        int maxChange = INT32_MIN;
        int count = 0;
        for (int i = 0; i < number; ++i) {
            if (adjacent[i][VColor[i]] == 0) continue;
            for (int k = 0; k < key; ++k) {
                if (VColor[i] == k || tabu[i][k] > step) continue;
                tmpTabu = adjacent[i][VColor[i]] - adjacent[i][k];
                if (maxChange < tmpTabu) {
                    maxChange = tmpTabu;
                    moveV = i;
                    moveC = k;
                    count = 2;
                }
                //pick max improvement randomly
                else if (maxChange == tmpTabu) {
                    if (!(R() % count)) {
                        moveV = i;
                        moveC = k;
                        ++count;
                    }
                }
            }
        }
        /*
         *handle local best
         */
//        tabu[moveV][VColor[moveV]] = R() % number + step + number;
//        moveV = R() % number;
//        moveC = R() % key;
//        maxChange = adjacent[moveV][VColor[moveV]] - adjacent[moveV][moveC];
        if (maxChange <= 0) {// && !(R() % 4)) {
            if (maxChange == INT32_MIN) break;
            if (!(R() % 2)) {
                tabu[moveV][VColor[moveV]] = R() % number + step + number;
                moveV = R() % number;
                moveC = R() % key;
                maxChange = adjacent[moveV][VColor[moveV]] - adjacent[moveV][moveC];
            }
        }
        /*
         *update table
         */
        for (auto neightbour : E[moveV]) {
            --adjacent[neightbour][VColor[moveV]];
            ++adjacent[neightbour][moveC];
        }
        /*
         *update
         */
        tabu[moveV][VColor[moveV]] = 1 + R() % 5 + step;
        VColor[moveV] = moveC;
        //conflict -= 2 * maxChange;
        ++step;
    }
    
    clock_t end = clock();
    double time = (double)(end - begin)/CLOCKS_PER_SEC;
    
//    for (int i = 0; i < number; ++i) {
//        cout << i + 1 << ": ";
//        cout << VColor[i] << "    ";
//        if (i % 6 == 0 && i != 0) cout << endl;
//    }
//    cout << endl;
    if (check(VColor, E)) {
        cout << "success" << " | ";
    }
    else {
        cout << "  fail " << " | ";
    }
    cout << argv[1] << " | " << argv[2] << " | ";
    cout<<setiosflags(ios::fixed);
    cout << "time: " << setprecision(4) << time << " | ";
    
    cout << "iterator:" << step << endl;
    visual(VColor, E, key);
}
/*
 *greed init
 */
inline void init (vector<vector<int>> &E, vector<int> &V, int k, int number) {
    random_device rd;
    default_random_engine R(rd());
    for (int i = 0; i < number; ++i) {
        V[i] = -1;
    }
    size_t maxDegree = 0;
    int maxV = -1;
    int countD = 0;
    vector<int>color(k, 0);
    for (int i = 0; i < E.size(); ++i) {
        if (maxDegree < E[i].size()) {
            maxDegree = static_cast<int>(E[i].size());
            maxV = i;
            countD = 2;
        }
        else if (maxDegree == E[i].size()) {
            if (!(R() % countD)) {
                maxV = i;
                ++countD;
            }
        }
    }
    V[maxV] = 0;
    ++color[0];
    int time = 1;
    while (time < number) {
        vector<int>saturation;
        for (int i = 0; i < number; ++i) {
            if (V[i] != -1) {
                saturation.push_back(-1);
                continue;
            }
            vector<int> difference;
            for (auto item : E[i]) {
                if (V[item] == -1) continue;
                difference.push_back(V[item]);
            }
            auto newEnd = unique(difference.begin(), difference.end());
            saturation.push_back(static_cast<int>(distance(difference.begin(), newEnd)));
        }
        int maxS = INT32_MIN;
        int point = -1;
        int countS = 0;
        for (int i = 0; i < number; ++i) {
            if (maxS < saturation[i]) {
                maxS = saturation[i];
                point = i;
                countS = 2;
            }
            else if (maxS == saturation[i]) {
                if (E[point].size() < E[i].size()) {
                    point = i;
                    countS = 2;
                }
                else if (E[point].size() == E[i].size()) {
                    if (!(R() % countS)) {
                        point = i;
                        ++countS;
                    }
                }
            }
        }
        if (point == -1) break;
        int used = INT32_MAX;
        int countC = 0;
        int chooseColor = -1;
        for (int i = 0; i < k; ++i) {
            if (color[i] < used) {
                used = color[i];
                chooseColor = i;
                countC = 2;
            }
            else if (color[i] == used) {
                if (!(R() % countC)) {
                    chooseColor = i;
                    ++countC;
                }
            }
        }
        V[point] = chooseColor;
        ++color[chooseColor];
        ++time;
    }
    
}
inline void addNew (vector<vector<int>> &E, int v1, int v2) {
    E[v1].push_back(v2);
}
inline bool check (vector<int> &V, vector<vector<int>> &E) {
    for (int i = 0; i < E.size(); ++i) {
        for (auto n : E[i]) {
            if (V[i] == V[n]) {
                return false;
            }
        }
    }
    return true;
}
