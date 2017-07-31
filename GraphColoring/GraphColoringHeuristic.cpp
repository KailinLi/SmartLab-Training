//
//  GraphColoring.cpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/29.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#include "GraphColoringHeuristic.hpp"


std::random_device rd;
std::mt19937 R(rd());


inline void addNew (vector<vector<int>> &E, int v1, int v2);
inline bool check (int *V, vector<vector<int>> &E);
inline void tabuSearchInit (int *VColor, int **adjacent, int **tabu, int number, int key, vector<vector<int>> &E, int *conflict);
inline void tabuSearch (int *VColor, int **adjacent, int **tabu, int number, int key, vector<vector<int>> &E, int *conflict);
const int population = 10;

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
    std::uniform_int_distribution<> randomByNumber(0, number - 1);
    std::uniform_int_distribution<> randomByKey(0, key - 1);
    std::uniform_int_distribution<> randomByFive(0, 4);
    std::uniform_int_distribution<> randomByTwo(0,1);
    std::uniform_int_distribution<> randomByPopulation(0,population - 1);
    
    
    /*
     *make group
     */
    
    int *VColor = new int[number];
    int **adjacent = new int* [number];
    int **tabu = new int* [number];
    
    for (int i = 0; i < number; ++i) {
        adjacent[i] = new int [key];
        tabu[i] = new int [key];
    }
    
    int maxNumber = 0;
    int maxConflict = INT32_MIN;
    int count = 0;
    vector< pair<vector<vector<int>>, int > >elitesGroup;
    
    
    for (int current = 0; current < population; ++current) {
        
        for (int i = 0; i < number; ++i) {
            VColor[i] = randomByKey(R);
        }
        
        int conflict = 0;
        for (int i = 0; i < number; ++i) {
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
        
        tabuSearchInit(VColor, adjacent, tabu, number, key, E, &conflict);
        
        if (conflict == 0) {
            maxConflict = INT32_MAX;
            break;
        }
        else if (conflict > maxConflict) {
            maxConflict = conflict;
            maxNumber = current;
            count = 2;
        }
        else if (conflict == maxConflict &&!(R() % count)) {
            maxConflict = conflict;
            maxNumber = current;
            ++count;
        }
        
        vector<vector<int>> person(key, vector<int>());
        
        for (int i = 0; i < number; ++i) {
            person[VColor[i]].push_back(i);
        }
        elitesGroup.push_back(make_pair(person, conflict));

    }
    
    clock_t begin = clock();
    
    if (maxConflict != INT32_MAX) {
        int time = 0;
        while (time < 50000) {
//            printf("***%d***\n", time);
            int father;
            int mother;
            do {
                father = randomByPopulation(R);
            } while (father == maxNumber);
            do {
                mother = randomByPopulation(R);
            } while (mother == maxNumber || mother == father);
            
//            set<int>hadUse;
            vector<vector<int>>& fatherV = elitesGroup[father].first;
            vector<vector<int>>& motherV = elitesGroup[mother].first;
            
            
            for (int i = 0; i < number; ++i) {
                VColor[i] = -1;
            }
            
            bool flag = true;
            
            for (int c = 0; c < key; ++c) {
                int bestStruct = 0;
                int bestStructCount = INT32_MIN;
                if (flag) {
                    for (int item = 0; item < key; ++item) {
                        int best = static_cast<int>(count_if(fatherV[item].begin(), fatherV[item].end(), [VColor](int n) { return VColor[n] == -1;}));
                        if (best > bestStructCount) {
                            bestStructCount = best;
                            bestStruct = item;
                            count = 2;
                        }
                        else if (best == bestStructCount && !(R() % count)) {
                            bestStructCount = best;
                            bestStruct = item;
                            ++count;
                        }
                    }
                    for (int item : fatherV[bestStruct]) {
//                        hadUse.insert(item);
                        VColor[item] = c;
                    }
                    flag = false;
                }
                else {
                    for (int item = 0; item < key; ++item) {
                        int best = static_cast<int>(count_if(motherV[item].begin(), motherV[item].end(), [VColor](int n) { return VColor[n] == -1;}));
                        if (best > bestStructCount) {
                            bestStructCount = best;
                            bestStruct = item;
                            count = 2;
                        }
                        else if (best == bestStructCount && !(R() % count)) {
                            bestStructCount = best;
                            bestStruct = item;
                            ++count;
                        }
                    }
                    for (int item : motherV[bestStruct]) {
//                        hadUse.insert(item);
                        VColor[item] = c;
                    }
                    flag = true;
                }
            }
            
            for (int i = 0; i < number; ++i) {
                if (VColor[i] == -1) VColor[i] = randomByKey(R);
            }
            
            int conflict = 0;
            for (int i = 0; i < number; ++i) {
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
//            int maxTime = 10000 + (time<<4);
//            if (maxTime > 200000) maxTime = 200000;
            tabuSearch(VColor, adjacent, tabu, number, key, E, &conflict);
            
            if (conflict == 0) {
                break;
            }
            auto tmp = &elitesGroup[maxNumber].first;
            
            for (int i = 0; i < key; ++i) {
                (*tmp)[i].clear();
            }
            
            for (int i = 0; i < number; ++i) {
                ((*tmp)[VColor[i]]).push_back(i);
            }
            
            elitesGroup[maxNumber].second = conflict;
            maxConflict = INT32_MIN;
//            int minconflict = INT32_MAX;
            for (int i = 0; i < population; ++i) {
//                if (elitesGroup[i].second < minconflict) minconflict = elitesGroup[i].second;
                if (elitesGroup[i].second > maxConflict) {
                    maxConflict = elitesGroup[i].second;
                    maxNumber = i;
                    count = 2;
                }
                else if (elitesGroup[i].second == maxConflict && !(R() % count)) {
                    maxConflict = elitesGroup[i].second;
                    maxNumber = i;
                    ++count;
                }
            }
//            printf("%d ** %d\n", maxConflict, minconflict);
            ++time;
        }
        cout << time << endl;
    }
    
    
//    clock_t begin = clock();
//    
//    
    clock_t end = clock();
    double time = (double)(end - begin)/CLOCKS_PER_SEC;
    

    if (check(VColor, E)) {
        cout << "success" << " | ";
    }
    else {
        cout << "  fail " << " | ";
    }
    cout << argv[1] << " | " << argv[2] << " | ";
    
    
    delete [] VColor;
    for (int i = 0; i < number; ++i) {
        delete [] adjacent[i];
        delete [] tabu[i];
    }
    delete [] adjacent;
    delete [] tabu;
    cout<<setiosflags(ios::fixed);
    cout << "time: " << setprecision(4) << time << endl;///< " | ";
    
//    cout << "iterator:" << step << endl;
    
//    cout << historyBest << " | " << bestStep << endl;
    //visual(VColor, E, key);
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
inline void tabuSearchInit (int *VColor, int **adjacent, int **tabu, int number, int key, vector<vector<int>> &E, int *conflict) {
    
    std::uniform_int_distribution<> randomByNumber(0, number - 1);
    std::uniform_int_distribution<> randomByKey(0, key - 1);
    std::uniform_int_distribution<> randomByFive(0, 4);
    std::uniform_int_distribution<> randomByTwo(0,1);
    
    int step = 0;
    while (step != 10000) {
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
                else if (maxChange == tmpTabu) {
                    if (!(R() % count)) {
                        moveV = i;
                        moveC = k;
                        ++count;
                    }
                }
            }
        }
        if (maxChange <= 0) {
            if (maxChange == INT32_MIN) break;
            
            if (!randomByTwo(R)) {
                tabu[moveV][VColor[moveV]] = randomByNumber(R) + step + number;
                moveV = randomByNumber(R);
                moveC = randomByKey(R);
                maxChange = adjacent[moveV][VColor[moveV]] - adjacent[moveV][moveC];
            }
        }
        for (auto neightbour : E[moveV]) {
            --adjacent[neightbour][VColor[moveV]];
            ++adjacent[neightbour][moveC];
        }
        tabu[moveV][VColor[moveV]] = 1 + randomByFive(R) + step;
        VColor[moveV] = moveC;
        *conflict -= 2 * maxChange;
        ++step;
    }
}
inline void tabuSearch (int *VColor, int **adjacent, int **tabu, int number, int key, vector<vector<int>> &E, int *conflict) {
    
    std::uniform_int_distribution<> randomByNumber(0, number - 1);
    std::uniform_int_distribution<> randomByKey(0, key - 1);
    std::uniform_int_distribution<> randomByFive(0, 4);
    std::uniform_int_distribution<> randomByTwo(0,1);
    
    int step = 0;
    int HBConflict = INT32_MAX;
    vector<int> HBVColor(number, 0);
    while (step != 20000) {
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
                else if (maxChange == tmpTabu) {
                    if (!(R() % count)) {
                        moveV = i;
                        moveC = k;
                        ++count;
                    }
                }
            }
        }
        if (maxChange <= 0) {
            if (maxChange == INT32_MIN) break;
            int tmp = *conflict - (maxChange<<1);
            if (HBConflict > tmp) {
                HBConflict = tmp;
                for (int i = 0; i < number; ++i) {
                    HBVColor[i] = VColor[i];
                }
            }
            if (!randomByTwo(R)) {
                tabu[moveV][VColor[moveV]] = randomByNumber(R) + step + number;
                moveV = randomByNumber(R);
                moveC = randomByKey(R);
                maxChange = adjacent[moveV][VColor[moveV]] - adjacent[moveV][moveC];
            }
        }
        for (auto neightbour : E[moveV]) {
            --adjacent[neightbour][VColor[moveV]];
            ++adjacent[neightbour][moveC];
        }
        tabu[moveV][VColor[moveV]] = 1 + randomByFive(R) + step;
        VColor[moveV] = moveC;
        *conflict -= (maxChange<<1);
        ++step;
    }
    if (HBConflict < *conflict) {
        for (int i = 0; i < number; ++i) {
            VColor[i] = HBVColor[i];
        }
    }
    else if (HBConflict == *conflict && randomByTwo(R)) {
        for (int i = 0; i < number; ++i) {
            VColor[i] = HBVColor[i];
        }
    }
}
