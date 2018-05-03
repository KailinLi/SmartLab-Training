#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <random>
#include <ctime>
using namespace std;
const int MAXN = 900 + 5;

struct SortD {
    int index;
    int dist;
    SortD() : index(0), dist(0) {}
    SortD(int i, int d) : index(i), dist(d) {}
};

struct Pair {
    int addV;
    int deleteV;
    int dist;
    Pair() : addV(0), deleteV(0), dist(0) {}
    Pair(int aV, int dV, int d) : addV(aV), deleteV(dV), dist(d) {}
};

int number, p, answer;
int dist[MAXN][MAXN];
void initDistance();

inline int findMinDijkstra (vector<int> &way, vector<bool> &inPath);
inline int findMinSet (vector<SortD>&, int );
inline void makeTable (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&);
inline int findMax (vector<pair<SortD, SortD>>&, size_t&);
inline void addCenter (vector<pair<SortD, SortD>>&, int(*)[MAXN], int);
inline int simulateDelete(vector<pair<SortD, SortD>>&, int);
inline void deleteCenter (vector<pair<SortD, SortD>>&, list<int>&, vector<vector<SortD>>&, int);
inline SortD findSecond (list<int>&, vector<vector<SortD>>&, int);
int main (int argc, char *argv[]) {
    freopen(argv[1], "r", stdin);
    sscanf(argv[2], "%d", &answer);
    initDistance();
    /*
     *sort the dist
     */
    vector<vector<SortD>>SortDistance;
    for (int i = 0; i < number; ++i) {
        vector<SortD> sortSet;
        for (int j = 0; j < number; ++j) {
            sortSet.emplace_back(j, dist[i][j]);
        }
        stable_sort(sortSet.begin(), sortSet.end(), [](SortD x, SortD y) {return x.dist < y.dist;});
        SortDistance.push_back(sortSet);
    }
    /*
     *initial
     */
    list<int>pCenter;
    std::random_device rd;
    std::mt19937 R(rd());
    std::uniform_int_distribution<> randomByNumber(0, number - 1);
    std::uniform_int_distribution<> randomByP(0, p - 1);
    std::uniform_int_distribution<> randomByTen(0, 9);
    int current = randomByNumber(R);
    pCenter.insert(pCenter.begin(), current);
    while (pCenter.size() < p) {
        auto center = *SortDistance[current].rbegin();
        int size = findMinSet(SortDistance[center.index], center.dist);
        do {
            current = SortDistance[center.index][R() % size].index;
            ++size;
        } while (find(pCenter.begin(), pCenter.end(), current) != pCenter.end());
        pCenter.insert(pCenter.begin(), current);
    }
    /*
     *make d table and f table
     */
    vector<pair<SortD, SortD>>table;
    makeTable(table, pCenter, SortDistance);
    
    /*
     *tabu table
     */
    int *tabuAdd = new int[number]{0};
    int *tabuDelete = new int[number]{0};
    int longest = 0;
    /*
     *save bestHistory
     */
    int historyBest = INT32_MAX;
    list<int> bestPCenter;
    clock_t begin = clock();
#pragma mark iterator
    int step = 0;
    while (step != 10000000) {
        size_t current;
        longest = findMax(table, current);
        int lessCount = findMinSet(SortDistance[current], table[current].first.dist);
        Pair best(-1, -1, INT32_MAX);
        int Pcount = 0;
        
        for (int addV = 0; addV < lessCount; ++addV) {
            /*
             *no conflict and not in the tabu table
             */
            if ((SortDistance[current][addV].index == table[current].first.index) || tabuDelete[SortDistance[current][addV].index] > step) continue;
            /*
             *store the table prevent to recover
             */
            vector<pair<SortD, SortD>>saveTable = table;
            addCenter(saveTable, dist, SortDistance[current][addV].index);
            
            int minLongest = INT32_MAX;
            int deleteV = -1;
            int count = 0;
            for (auto center : pCenter) {
                if (tabuAdd[center] > step) continue;
                int value = simulateDelete(saveTable, center);
                if (minLongest > value) {
                    minLongest = value;
                    deleteV = center;
                    count = 2;
                }
                else if (minLongest == value) {
                    if (!(R() % count)) {
                        deleteV = center;
                        ++count;
                    }
                }
            }
            if (deleteV == -1) continue;
            if (best.dist > minLongest) {
                best.addV = SortDistance[current][addV].index;
                best.deleteV = deleteV;
                best.dist = minLongest;
                Pcount = 2;
            }
            else if (best.dist == minLongest) {
                if (!(R() % count)) {
                    best.addV = SortDistance[current][addV].index;
                    best.deleteV = deleteV;
                    ++Pcount;
                }
            }
        }
        if (best.addV == -1) {
            best.addV = SortDistance[current][0].index;
            best.deleteV = table[current].first.index;
            best.dist = 0;
        }
        
        if (best.dist >= longest) {
            if (historyBest > longest) {
                historyBest = longest;
                bestPCenter = pCenter;
                if (historyBest <= answer) break;
            }
            int randNum = randomByP(R);
            auto dV = pCenter.begin();
            while (randNum > 0) {
                ++dV;
                --randNum;
            }
            int aV = randomByNumber(R);
            while (find(pCenter.begin(), pCenter.end(), aV) != pCenter.end()) {
                aV = randomByNumber(R);
            }
            best.addV = aV;
            best.deleteV = *dV;
            best.dist = dist[aV][*dV];
        }
        
        pCenter.erase(find(pCenter.begin(), pCenter.end(), best.deleteV));
        pCenter.insert(pCenter.begin(), best.addV);
        addCenter(table, dist, best.addV);
        deleteCenter(table, pCenter, SortDistance, best.deleteV);
        tabuAdd[best.addV] = step + randomByTen(R);
        tabuDelete[best.deleteV] = step + randomByTen(R);
        ++step;        
    }
    
    clock_t end = clock();
    double time = (double)(end - begin)/CLOCKS_PER_SEC;
    
    if (historyBest > longest) {
        historyBest = longest;
        bestPCenter = pCenter;
    }
    table.clear();
    makeTable(table, bestPCenter, SortDistance);
    printf("| %s | %d | %d | %lf | %d\n", argv[1], answer, historyBest, time, step);
    delete [] tabuAdd;
    delete [] tabuDelete;
}
void initDistance() {
    int m, u, v, w;
    scanf("%d %d %d", &number, &m, &p);
    for (int i = 0; i < number; ++i)
        for (int j = 0; j < number; ++j)
            if (i == j) dist[i][j] = 0;
            else dist[i][j] = 0x3f3f3f3f;
    while (m--) {
        scanf("%d %d %d", &u, &v, &w);
        dist[u - 1][v - 1] = w;
        dist[v - 1][u - 1] = w;
    }
    for (int k = 0; k < number; ++k)
        for (int i = 0; i < number; ++i)
            for (int j = 0; j < number; ++j)
                dist[i][j] = min(dist[i][k] + dist[k][j], dist[i][j]);
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
inline int findMinSet (vector<SortD>& sort, int current) {
    int begin = 0, end = static_cast<int>(sort.size()) - 1;
    int middle = 0;
    while (begin < end) {
        middle = ((begin + end) >> 1) + 1;
        if (sort[middle].dist > current) end = middle - 1;
        else begin = middle;
    }
    return middle + 1;
}
inline void makeTable (vector<pair<SortD, SortD>>& table, list<int>& pCenter, vector<vector<SortD>>& SortDistance) {
    for (int index = 0; index < SortDistance.size(); ++index) {
        SortD best;
        SortD second;
        int i = 0;
        auto item = SortDistance[index];
        while (i < item.size()) {
            if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
                best = item[i++];
                break;
            }
            ++i;
        }
        while (i < item.size()) {
            if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
                second = item[i];
                break;
            }
            ++i;
        }
        table.push_back(make_pair(best, second));
    }
}
inline int findMax (vector<pair<SortD, SortD>>& table, size_t& current) {
    int tmpMax = INT32_MIN;
    int count = 0;
    for (size_t i = 0; i < table.size(); ++i) {
        if (tmpMax < table[i].first.dist) {
            tmpMax = table[i].first.dist;
            current = i;
            count = 2;
        }
        else if (tmpMax == table[i].first.dist) {
            if (!(rand() % count)) {
                current = i;
                ++count;
            }
        }
    }
    return tmpMax;
}
inline void addCenter (vector<pair<SortD, SortD>>& table, int(*dist)[MAXN], int addV) {
    for (int index = 0; index < table.size(); ++index) {
        if (dist[index][addV] < table[index].first.dist) {
            table[index].second = table[index].first;
            table[index].first = SortD(addV, dist[index][addV]);
        }
        else if (dist[index][addV] < table[index].second.dist) {
            table[index].second = SortD(addV, dist[index][addV]);
            if (table[index].second.dist == table[index].first.dist && table[index].second.index < table[index].first.index) {
                swap(table[index].second, table[index].first);
            }
        }
        else if (dist[index][addV] == table[index].second.dist) {
            if (dist[index][addV] == table[index].first.dist && addV < table[index].first.index) {
                table[index].second.index = table[index].first.index;
                table[index].first.index = addV;
            }
            else if (addV < table[index].second.index) {
                table[index].second.index = addV;
            }
        }
    }
}
inline int simulateDelete(vector<pair<SortD, SortD>>& table, int deleteV) {
    int longest = INT32_MIN;
    for (auto item : table) {
        if (item.first.index == deleteV) {
            longest = max(longest, item.second.dist);
        }
        else {
            longest = max(longest, item.first.dist);
        }
    }
    return longest;
}
inline void deleteCenter (vector<pair<SortD, SortD>>& table, list<int>& pCenter, vector<vector<SortD>>& SortDistance, int deleteV) {
    for (int index = 0; index < table.size(); ++index) {
        if (table[index].first.index == deleteV) {
            table[index].first = table[index].second;
            table[index].second = std::move(findSecond(pCenter, SortDistance, index));
        }
        else if (table[index].second.index == deleteV) {
            table[index].second = std::move(findSecond(pCenter, SortDistance, index));
        }
        if (table[index].first.dist == table[index].second.dist && table[index].first.index > table[index].second.index) {
            swap(table[index].first, table[index].second);
        }
    }
}
inline SortD findSecond (list<int>& pCenter, vector<vector<SortD>>& SortDistance, int index) {
    SortD second;
    int i = 0;
    auto item = SortDistance[index];
    while (i < item.size()) {
        if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
            ++i;
            break;
        }
        ++i;
    }
    while (i < item.size()) {
        if (find(pCenter.begin(), pCenter.end(), item[i].index) != pCenter.end()) {
            second = item[i];
            break;
        }
        ++i;
    }
    return second;
}
