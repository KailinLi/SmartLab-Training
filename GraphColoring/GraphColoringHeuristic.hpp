//
//  GraphColoring.hpp
//  Gurobi
//
//  Created by 李恺林 on 2017/6/29.
//  Copyright © 2017年 李恺林. All rights reserved.
//

#ifndef GraphColoring_hpp
#define GraphColoring_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <ctime>
#include <iomanip>
#include <random>

using namespace std;

void visual (vector<int> &V, vector<vector<int>> &E, int key) {
    vector<string> colorList = {
        "aliceblue", "antiquewhite", "aquamarine", "azure", "beige",
        "bisque", "blanchedalmond", "blue", "blueviolet",
        "brown", "burlywood", "cadetblue", "chartreuse", "chocolate",
        "coral", "cornflowerblue", "cornsilk", "crimson", "cyan",
        "darkblue", "darkcyan", "darkgoldenrod", "darkgreen",
        "darkgrey", "darkkhaki", "darkmagenta", "darkolivegreen", "darkorange",
        "darkorchid", "darksalmon", "darkseagreen", "darkslateblue",
        "darkslategray", "darkslategrey", "darkturquoise", "darkviolet", "deeppink",
        "deepskyblue", "dodgerblue", "firebrick",
        "floralwhite", "forestgreen", "fuchsia", "gainsboro", "ghostwhite",
        "gold", "goldenrod", "green",
        "greenyellow", "honeydew", "hotpink", "indianred", "indigo",
        "ivory", "khaki", "lavender", "lavenderblush", "lawngreen",
        "lemonchiffon", "lightblue", "lightcoral", "lightcyan", "lightgoldenrodyellow",
        "lightgray", "lightgreen", "lightgrey", "lightpink", "lightsalmon",
        "lightseagreen", "lightskyblue", "lightslategray", "lightslategrey", "lightsteelblue",
        "lightyellow", "lime", "limegreen", "linen", "magenta",
        "maroon", "mediumaquamarine", "mediumblue", "mediumorchid", "mediumpurple",
        "mediumseagreen", "mediumslateblue", "mediumspringgreen", "mediumturquoise", "mediumvioletred",
        "midnightblue", "mintcream", "mistyrose", "moccasin", "navajowhite",
        "navy", "oldlace", "olive", "olivedrab", "orange",
        "orangered", "orchid", "palegoldenrod", "palegreen", "paleturquoise",
        "palevioletred", "papayawhip", "peachpuff", "peru", "pink",
        "plum", "powderblue", "purple", "red", "rosybrown",
        "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen",
        "seashell", "sienna", "skyblue", "slateblue", "snow", "springgreen", "steelblue",
        "tan", "teal", "thistle", "tomato", "turquoise", 
        "violet", "wheat", "yellow",
        "yellowgreen"
    };
    srand(static_cast<unsigned>(time(NULL)));
    vector<size_t> randomColor;
    while (randomColor.size() < key) {
        size_t getNew = rand() % colorList.size();
        if (find(randomColor.begin(), randomColor.end(), getNew) == randomColor.end())
            randomColor.push_back(getNew);
    }
    for_each(randomColor.begin(), randomColor.end(), [](size_t i){cout << i << endl;});
    ofstream out("/Users/likailin/Desktop/Graph/simple.dot");
    out << "graph G {\nnode [shape=\"circle\"];" << endl;
    for (int i = 0; i < V.size(); ++i) {
        out << (i+1) << "[style=\"filled\", color=\"" << colorList[randomColor[V[i]]] << "\"];" << endl;
    }
    for (int i = 0; i < E.size(); ++i) {
        auto begin = find_if(E[i].begin(), E[i].end(), [i](int n) {return n > i;});
        while (begin != E[i].end()) {
            out << (i+1) << "--" << (*begin + 1) << ";" << endl;
            ++begin;
        }
    }
    out << "}" << endl;
}

#endif /* GraphColoring_hpp */
