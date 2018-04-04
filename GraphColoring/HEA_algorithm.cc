#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <random>
#include <ctime>
using namespace std;
const int INF = 0x3f3f3f3f;
const int MAXK = 100 + 5;
const int MAXN = 500 + 5;
const int MAXS = 9;
struct Edge {
    int to;
    int next;
};
Edge edge[(MAXN + 1) * MAXN];
int head[MAXN];
int eCnt;
void addEdge(int u, int v) {
    edge[++eCnt].next = head[u];
    edge[eCnt].to = v;
    head[u] = eCnt;
}
int K, n;
struct Person {
    int tabuTenure[MAXN][MAXK];
    int colorTable[MAXN][MAXK];
    int c[MAXN];
    int bestC[MAXN];
    int conflict;
    int step;
    int hisBest;
    void init();
    void makeColorTable();
    bool TS(int);
    bool checkRes();
};
Person society[MAXS];
Person son;

void getInput();
int crossStep;
int hadCross[MAXN];
int colorCnt[MAXK];

int main (int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    freopen(argv[1], "r", stdin);
    sscanf(argv[2], "%d", &K);
    getInput();
    clock_t begin = clock();
    for (int i = 0; i < MAXS; ++i) {
        society[i].init();
        society[i].makeColorTable();
        if (society[i].TS(1000000)) // just tabu
            return 0;
    }

    random_device rd;
    default_random_engine R(rd());
    while (crossStep++ != 10000) {
        // Pick father & mother randomly
        int f_num = R() % MAXS;
        int m_num = R() % MAXS;
        while (m_num == f_num) m_num = R() % MAXS;
        Person *father = society + f_num;
        Person *mother = society + m_num;
        int *c;
        memset(hadCross, 0, sizeof(int) * (n + 1));
        for (int k = 0; k < K; ++k) {
            if (k % 2) c = mother->bestC; else c = father->bestC;
            memset(colorCnt, 0, sizeof(int) * K);
            for (int u = 1; u <= n; ++u) 
                if (!hadCross[u])
                    ++colorCnt[c[u]];
            int kCnt, kSet, kMax = -1;
            for (int i = 0; i < K; ++i) {
                if (colorCnt[i] > kMax) {
                    kSet = i; kMax = colorCnt[i]; kCnt = 2;
                }
                else if (colorCnt[i] == kMax && !(R() % kCnt)) {
                    kSet = i; kMax = colorCnt[i]; ++kCnt;
                }
            }
            for (int u = 1; u <= n; ++u) {
                if (c[u] == kSet && !hadCross[u]) {
                    son.c[u] = k; hadCross[u] = 1;
                }
            }
        }
        for (int u = 1; u <= n; ++u)
            if (!hadCross[u]) son.c[u] = R() % K;
        memset(son.tabuTenure, 0, sizeof(son.tabuTenure));
        son.conflict = son.step = 0;
        son.makeColorTable();
        if (son.TS(450000)) break;
        int killCnt = 2, killNum = MAXS, cMax = son.hisBest;
        for (int i = 0; i < MAXS; ++i) {
            if (cMax < society[i].hisBest) {
                cMax = society[i].hisBest; killNum = i; killCnt = 2;
            }
            else if (cMax == society[i].hisBest && !(R() % killCnt)) {
                cMax = society[i].hisBest; killNum = i; ++killCnt;
            }
        }
        if (killNum != MAXS) {
            society[killNum].hisBest = son.hisBest;
            printf("change after cross:%d\n", society[killNum].hisBest);
            memcpy(society[killNum].bestC, son.bestC, sizeof(int) * (n + 1));
        }
    }
    clock_t end = clock();
    double timeCost = (double)(end - begin)/CLOCKS_PER_SEC;
    printf("crossStep: %d\ttime: %lf\n", crossStep, timeCost);
    return 0;
}

void getInput() {
    char buf[100 + 2];
    while (fgets(buf, 100, stdin) && buf[0] != 'p');
    char tmpC, tmpS[10];
    int m, u, v;
    sscanf(buf, "%c %s %d %d", &tmpC, tmpS, &n, &m);
    while (m--) {
        scanf("%c %d %d", &tmpC, &u, &v);
        getchar();
        addEdge(u, v);
        addEdge(v, u);
    }
}

void Person::init() {
    random_device rd;
    default_random_engine R(rd());
    for (int i = 1; i <= n; ++i) {
        c[i] = R() % K;
    }
}
void Person::makeColorTable() {
    for (int u = 1; u <= n; ++u) {
        for (int k = 0; k < K; ++k) {
            int clash = 0;
            for (int e = head[u]; e != 0; e = edge[e].next) {
                if (k == c[edge[e].to]) ++clash;
            }
            if (k == c[u]) conflict += clash;
            colorTable[u][k] = clash;
        }
    }
    conflict >>= 1;
}
bool Person::checkRes() {
    for (int u = 1; u <= n; ++u) {
        for (int e = head[u]; e != 0; e = edge[e].next) {
            if (bestC[u] == bestC[edge[e].to]) return false;
        }
    }
    return true;
}
bool Person::TS(int times) {
    random_device rd;
    default_random_engine R(rd());
    hisBest = INF;
    int mc, mv;
    int tabuMc, tabuMv;
    int tabuBest, nTabuBest;
    int delta, cnt, tabuCnt;

    // clock_t begin = clock();
    while (step++ != times) {
        tabuBest = nTabuBest = INF;
        for (int u = 1; u <= n; ++u) {
            if (!colorTable[u][c[u]]) continue;
            for (int k = 0; k < K; ++k) {
                if (k == c[u]) continue;
                delta = colorTable[u][k] - colorTable[u][c[u]];
                if (tabuTenure[u][k] > step) {
                    // tabu best
                    if (delta < tabuBest) {
                        tabuBest = delta; tabuMv = u; tabuMc = k; tabuCnt = 2;
                    }
                    else if (delta == tabuBest && !(R() % tabuCnt)) {
                        tabuMv = u; tabuMc = k; ++tabuCnt;
                    }
                }
                else {
                    // no_tabu best
                    if (delta < nTabuBest) {
                        nTabuBest = delta; mv = u; mc = k; cnt = 2;
                    }
                    else if (delta == nTabuBest && !(R() % cnt)) {
                        mv = u; mc = k; ++cnt;
                    }
                }
            }
        }

        if (nTabuBest == INF) cout << "hehe" << endl;
        // if (tabuBest == INF && nTabuBest == INF) break; 
        if (tabuBest < nTabuBest && conflict + tabuBest < hisBest) {
            mc = tabuMc; mv = tabuMv; nTabuBest = tabuBest;
        }
        // update
        // cout << nTabuBest << endl;
        conflict += nTabuBest;
        // hisBest = min(hisBest, conflict);
        for (int e = head[mv]; e != 0; e = edge[e].next) {
            --colorTable[edge[e].to][c[mv]];
            ++colorTable[edge[e].to][mc];
        }
        tabuTenure[mv][c[mv]] = conflict + R() % 7 + step;
        c[mv] = mc;
        if (hisBest > conflict) {
            hisBest = conflict;
            memcpy(bestC, c, sizeof(int) * (n + 1));
        }
        if (!conflict) break;
    }
    // clock_t end = clock();
    // double timeCost = (double)(end - begin)/CLOCKS_PER_SEC;
    // printf("step: %d\ttime: %lf\n", step, timeCost);
    if (checkRes()) {
        printf("Finish\n");
        // cout << "Finish" << endl;
        return true;
    }
    else {
        printf("%d\tFailure\n", hisBest);
        // cout << conflict << endl;
        // cout << "Failure" << endl;
        return false;
    }
}