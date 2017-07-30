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


inline void init (vector<vector<int>> &E, int *V, int k, int number);
inline void addNew (vector<vector<int>> &E, int v1, int v2);
inline bool check (int *V, vector<vector<int>> &E);
inline void tabuSearch (int *VColor, int **adjacent, int **tabu, int number, int key, vector<vector<int>> &E, int *conflict);

const int population = 20;

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
    std::uniform_int_distribution<> randomByTwenty(0,19);
    
    
    /*
     *make group
     */
    
    int *VColor = nullptr;
    int **adjacent = nullptr;
    int **tabu = nullptr;
    int maxNumber = 0;
    int maxConflict = INT32_MIN;
    vector< pair<vector<vector<int>>, int > >elitesGroup;
    
    
    for (int current = 0; current < population; ++current) {
        
        VColor = new int[number];
        for (int i = 0; i < number; ++i) {
            VColor[i] = randomByKey(R);
        }
        int conflict = 0;
        adjacent = new int* [number];
        tabu = new int* [number];
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
        tabuSearch(VColor, adjacent, tabu, number, key, E, &conflict);
        
        if (conflict == 0) {
            maxConflict = INT32_MAX;
        }
        else if (conflict > maxConflict) {
            maxConflict = conflict;
            maxNumber = current;
        }
        //shit
        vector<vector<int>> person(key, vector<int>());
        
        for (int i = 0; i < number; ++i) {
            person[VColor[i]].push_back(i);
        }
        elitesGroup.push_back(make_pair(person, conflict));
        delete [] VColor;
        for (int i = 0; i < number; ++i) {
            delete [] adjacent[i];
            delete [] tabu[i];
        }
        delete [] adjacent;
        delete [] tabu;
    }
    
    if (maxConflict != INT32_MAX) {
        int time = 0;
        while (time < 10000) {
            int father;
            int mother;
            do {
                father = randomByTwenty(R);
            } while (father == maxNumber);
            do {
                mother = randomByTwenty(R);
            } while (mother == maxNumber || mother == father);
            
            set<int>hadUse;
            vector<vector<int>>& fatherV = elitesGroup[father].first;
            vector<vector<int>>& motherV = elitesGroup[mother].first;
            VColor = new int[number];
            bool flag = true;
            
            for (int c = 0; c < key; ++c) {
                int bestStruct = 0;
                int bestStructCount = INT32_MIN;
                if (flag) {
                    for (int item = 0; item < fatherV.size(); ++item) {
                        int best = static_cast<int>(count_if(fatherV[item].begin(), fatherV[item].end(), [hadUse](int n) { return hadUse.find(n) == hadUse.end();}));
                        if (best > bestStructCount) {
                            bestStructCount = best;
                            bestStruct = item;
                        }
                    }
                    for (int item : fatherV[bestStruct]) {
                        hadUse.insert(item);
                        VColor[item] = c;
                    }
                }
                else {
                    for (int item = 0; item < motherV.size(); ++item) {
                        int best = static_cast<int>(count_if(motherV[item].begin(), motherV[item].end(), [hadUse](int n) { return hadUse.find(n) == hadUse.end();}));
                        if (best > bestStructCount) {
                            bestStructCount = best;
                            bestStruct = item;
                        }
                    }
                    for (int item : motherV[bestStruct]) {
                        hadUse.insert(item);
                        VColor[item] = c;
                    }
                }
            }
            
            for (auto item : fatherV) {
                for (int i = 0; i < item.size(); ++i) {
                    if (hadUse.find(item[i]) == hadUse.end()) {
                        VColor[item[i]] = randomByKey(R);
                    }
                }
            }
            int conflict = 0;
            adjacent = new int* [number];
            tabu = new int* [number];
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
            tabuSearch(VColor, adjacent, tabu, number, key, E, &conflict);
            
            if (conflict == 0) {
                break;
            }
            
            elitesGroup[maxNumber].first.clear();
            vector<vector<int>> person(key, vector<int>());
            
            for (int i = 0; i < number; ++i) {
                person[VColor[i]].push_back(i);
            }
            elitesGroup[maxNumber].first = person;
            
            elitesGroup[maxNumber].second = conflict;
            maxConflict = INT32_MIN;
            for (int i = 0; i < population; ++i) {
                if (elitesGroup[i].second > maxConflict) {
                    maxConflict = elitesGroup[i].second;
                    maxNumber = i;
                }
            }
            delete [] VColor;
            for (int i = 0; i < number; ++i) {
                delete [] adjacent[i];
                delete [] tabu[i];
            }
            delete [] adjacent;
            delete [] tabu;
        }
    }
    
    
    
    init(E, VColor, key, number);
    
    
    int conflict = 0;
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
    
    while (step != 1000) {
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

        if (maxChange <= 0) {// && !(R() % 4)) {
            if (maxChange == INT32_MIN) break;
            if (historyBest > conflict) {
                historyBest = conflict;
                bestStep = step;
            }

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
inline void tabuSearch (int *VColor, int **adjacent, int **tabu, int number, int key, vector<vector<int>> &E, int *conflict) {
    
    std::uniform_int_distribution<> randomByNumber(0, number - 1);
    std::uniform_int_distribution<> randomByKey(0, key - 1);
    std::uniform_int_distribution<> randomByFive(0, 4);
    std::uniform_int_distribution<> randomByTwo(0,1);
    
    int step = 0;
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
        conflict -= 2 * maxChange;
        ++step;
    }
}
