#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

const int MAXK = 200 + 5;
const int MAXN = 1000 + 5;
int tabuTenure[MAXN][MAXK];
int colorTable[MAXN][MAXK];
int vColor[MAXN];
int k, n;

struct Edge {
    int to;
    int next;
};
Edge edge[(MAXN + 1) * MAXN >> 1];
int head[MAXN];
int eCnt;
void addEdge(int u, int v) {
    edge[++eCnt].next = head[u];
    edge[eCnt].to = v;
    head[u] = eCnt;
}

void getInput();

int main (int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    freopen(argv[1], "r", stdin);
    sscanf(argv[2], "%d", &k);
    
    getInput();


    return 0;
}

void getInput() {
    char buf[100 + 1];
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