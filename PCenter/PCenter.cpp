#include "gurobi_c++.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
using namespace std;


inline int findMinDijkstra (vector<int> &way, vector<bool> &inPath);
int main () {
    try {
        
        /*
         *read file and init the 2nd array
         */
        
        int number = 0;
        int p;
        ifstream in("pmed2.txt");
        
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
        
        /*
         *Dijkstra
         */
        for (int current = 0; current < number; ++current) {
            cout << current << endl;
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
         *init Gurobi
         */
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);
        GRBVar x[number];
        GRBVar y[number][number];
        
        
        GRBVar max;
        max = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "max");
        
        for (int i = 0; i < number; ++i) {
            x[i] = model.addVar(0, 1, 0, GRB_BINARY, "X" + to_string(i));
            for (int j = 0; j < number; ++j) {
                y[i][j] = model.addVar(0, 1, 0, GRB_BINARY, "Y" + to_string(i) + "_" + to_string(j));
            }
        }
        
        /*
         *add constraint
         */
        GRBLinExpr c0;
        for (int i = 0; i < number; ++i) {
            c0 += x[i];
            GRBLinExpr c2;
            for (int j = 0; j < number; ++j) {
                model.addConstr(max - distance[i][j]*y[i][j] >= 0);
                model.addConstr(x[i] - y[i][j] >= 0);
                c2 += y[j][i];
            }
            model.addConstr(c2 == 1);
        }
        model.addConstr(c0 == p);

        /*
         *add target
         */
        GRBLinExpr getMax = max;
        model.setObjective(getMax, GRB_MINIMIZE);
        
        
        model.optimize();
        model.write("solve.lp");
        /*
         *print result
         */
        cout << "Solution:" << endl;
        for (int i = 0; i < number; ++i) {
            if (x[i].get(GRB_DoubleAttr_X)) {
                cout << i << "   ";
                if (i % 8 == 0 && i != 0) cout << endl;
            }
        }
        cout << endl;
        cout << "Answer: " << max.get(GRB_DoubleAttr_X) << endl;
        
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
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
