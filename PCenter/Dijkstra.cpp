//
//  Dijkstra.cpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/30.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#include "Dijkstra.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;


inline int findMin (vector<int> &way, vector<bool> &inPath);
int main () {
    int number = 0;
    int p;
    
    ifstream in("/Users/likailin/Desktop/Gurobi/Gurobi/pmed1.txt");
    
    int in1, in2, in3;
    in >> in1 >> in2 >> in3;
    
    number = in1;
    int** distance = new int*[number];
    for(int i = 0; i < number; ++i)
        distance[i] = new int[number];
    p = in3;
    
    while (!in.eof()) {
        in >> in1 >> in2 >> in3;
        distance[in1 - 1][in2 - 1] = in3;
        distance[in2 - 1][in1 - 1] = in3;
    }
    
    for (int current = 0; current < number; ++current) {
        vector<int>way(number, INT32_MAX);
        vector<bool>inPath(number, false);
        way[current] = 0;
        int time = 0;
        while (time < number - 1) {
            int item = findMin(way, inPath);
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

}
inline int findMin (vector<int> &way, vector<bool> &inPath) {
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