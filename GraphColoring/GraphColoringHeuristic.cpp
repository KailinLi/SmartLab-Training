//
//  GraphColoring.cpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/29.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#include "GraphColoringHeuristic.hpp"
inline void init (vector<vector<int>> &E, int *V, int k, int number);
inline void addNew (vector<vector<int>> &E, int v1, int v2);
inline bool check (int *V, vector<vector<int>> &E);
int main (int argc, char *argv[]) {
    
    /*
     *read file and init
     */
    vector<vector<int>>E;
    
    string getBuf = argv[1];
    string getColor = argv[2];
    
    int key = stoi(getColor);

    ifstream in("DSJC" + getBuf + ".col.txt");
    int number = 0;
    int v1, v2;
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
    std::random_device rd;
    std::mt19937 R(rd());
    std::uniform_int_distribution<> randomByNumber(0, number - 1);
    std::uniform_int_distribution<> randomByKey(0, key - 1);
    std::uniform_int_distribution<> randomByFive(0, 4);
    std::uniform_int_distribution<> randomByTwo(0,1);
    
    /*
     *make table
     */
    
    int *VColor = new int[number];
    
    init(E, VColor, key, number);
    
    
    int conflict = 0;
    int **adjacent = new int* [number];
    int **tabu = new int* [number];
    for (int i = 0; i < number; ++i) {
        adjacent[i] = new int [key];
        tabu[i] = new int [key];
        for (int k = 0; k < key; ++k) {
            int cfs = 0;
            for (auto otherV : E[i]) {
                if (k == VColor[otherV]) ++cfs;
            }
            if (k == VColor[i]) conflict += cfs;
            adjacent[i][k] = cfs;
            tabu[i][k] = 0;
        }
    }
    
    clock_t begin = clock();
    
    int step = 0;
    
    /*
     *begin iterator
     */
    
    
    int historyBest = conflict;
    int bestStep = 0;
    
    while (step != 90000000) {
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
            if (historyBest > conflict) {
                historyBest = conflict;
                bestStep = step;
            }
//            else if (bestStep < step - 500000 && !(R() % 2)){
//                printf("%s\n", "reset");
//                conflict = 0;
//                bestStep = step;
//                for (int i = 0; i < number; ++i) {
//                    VColor[i] = R() % key;
//                }
//                for (int i = 0; i < number; ++i) {
//                    for (int k = 0; k < key; ++k) {
//                        int cfs = 0;
//                        for (auto otherV : E[i]) {
//                            if (k == VColor[otherV]) ++cfs;
//                        }
//                        if (k == VColor[i]) conflict += cfs;
//                        adjacent[i][k] = cfs;
//                        tabu[i][k] = 0;
//                    }
//                }
//                ++step;
//                continue;
//            }
            if (!randomByTwo(R)) {
                tabu[moveV][VColor[moveV]] = randomByNumber(R) + step + number;
                moveV = randomByNumber(R);
                moveC = randomByKey(R);
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
        tabu[moveV][VColor[moveV]] = 1 + randomByFive(R) + step;
        VColor[moveV] = moveC;
        conflict -= 2 * maxChange;
        ++step;
    }
    
    clock_t end = clock();
    double time = (double)(end - begin)/CLOCKS_PER_SEC;
    

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
    
    cout << historyBest << " | " << bestStep << endl;
    //visual(VColor, E, key);
}


/*
 *greed init
 */
inline void init (vector<vector<int>> &E, int *V, int k, int number) {
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
inline bool check (int *V, vector<vector<int>> &E) {
    for (int i = 0; i < E.size(); ++i) {
        for (auto n : E[i]) {
            if (V[i] == V[n]) {
                return false;
            }
        }
    }
    return true;
}
