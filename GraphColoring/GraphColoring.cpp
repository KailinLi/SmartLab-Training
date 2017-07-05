#include "gurobi_c++.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sys/timeb.h>
using namespace std;

//#define K 84
bool check (vector<int> *V, vector<pair<int, int>> *E);
int main (int argc, char *argv[]) {
    try {
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);
        
        string getBuf = argv[1];
        int K = stoi(argv[2]);
        
        
        ifstream in("DSJC"+ getBuf + ".col.txt");
        int v1, v2;
        int number = 0;
        vector<pair<int, int>>E;
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
                E.push_back(make_pair(v1, v2));
            }
        }
        
        GRBVar coloring[number][K];
        
        for (int i = 0; i < number; ++i) {
            for (int k = 0; k < K; ++k) {
                coloring[i][k] = model.addVar(0, 1, 0, GRB_BINARY, "X" + to_string(i) + "_"+ to_string(k));
            }
        }
        
        for (int i = 0; i < number; ++i) {
            GRBLinExpr line = 0;
            for (int k = 0; k < K; ++k) {
                line += coloring[i][k];
            }
            model.addConstr(line == 1);
        }
        
        for (auto edge : E) {
            for (int k = 0; k < K; ++k) {
                model.addConstr(coloring[edge.first - 1][k] + coloring[edge.second - 1][k] <= 1);
            }
        }
        
        struct timeb begin, end;
        
        ftime(&begin);
        model.optimize();
        ftime(&end);
        
        model.write("solve.lp");
        
        vector<int>V;
        
        cout << "Solution:" << endl;
        for (int i = 0; i < number; ++i) {
            for (int k = 0; k < K; ++k) {
                if (coloring[i][k].get(GRB_DoubleAttr_X)) {
                    V.push_back(k);
                    cout << "V" << i + 1 << ":" << " ";
                    cout << k + 1 << "  ";
                    break;
                }
            }
            if (i % 6 == 0 && i != 0) {
                cout << endl;
            }
        }
        cout << endl;
        
        if (check(&V, &E)) {
            cout << "success" << endl;
        }
        else cout << "fail" << endl;
        cout << "time: " << (end.time - begin.time)*1000 + (end.millitm - begin.millitm)  << endl;
        
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
}
bool check (vector<int> *V, vector<pair<int, int>> *E) {
    for (auto item : *E) {
        if ((*V)[item.first - 1] == (*V)[item.second - 1]) {
            return false;
        }
    }
    return true;
}
