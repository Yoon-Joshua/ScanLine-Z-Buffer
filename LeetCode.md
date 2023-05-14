## 1615 最大网络秩

[1615. 最大网络秩 - 力扣（Leetcode）](https://leetcode.cn/problems/maximal-network-rank/description/)

贪心算法。设图中度最大的顶点为a，度次大的顶点为b。如果a和b相连，那么最大网络秩为degree(a)+degree(b)-1，否则最大网络秩为degree(a)+degree(b)。

对于图中除a和b之外的任意一个顶点c，degree(c)<degree(b)，那么对于一个包含c的节点对，其网络秩必定小于等于degree(c)+degree(a)，又因为degree(c)<degree(b)，所以degree(c)+degree(a)<degree(b)+degree(a)，所以贪心算法正确。

```c++
class Solution {
public:
    int maximalNetworkRank(int n, vector<vector<int>>& roads) {
        vector<vector<bool>> connect(n, vector<bool>(n, false));
        vector<int> degree(n);
        for (auto road : roads) {
            int x = road[0], y = road[1];
            connect[x][y] = true;
            connect[y][x] = true;
            degree[x]++;
            degree[y]++;
        }

        int first = -1, second = -2;
        vector<int> firstArr, secondArr;
        for (int i = 0; i < n; ++i) {
            if (degree[i] > first) {
                second = first;
                secondArr = firstArr;
                first = degree[i];
                firstArr.clear();
                firstArr.emplace_back(i);
            } else if (degree[i] == first) {
                firstArr.emplace_back(i);
            } else if (degree[i] > second){
                secondArr.clear();
                second = degree[i];
                secondArr.emplace_back(i);
            } else if (degree[i] == second) {
                secondArr.emplace_back(i);
            }
        }
        if (firstArr.size() == 1) {
            int u = firstArr[0];
            for (int v : secondArr) {
                if (!connect[u][v]) {
                    return first + second;
                }
            }
            return first + second - 1;
        } else {
            int m = roads.size();
            if (firstArr.size() * (firstArr.size() - 1) / 2 > m) {
                return first * 2;
            }
            for (int u: firstArr) {
                for (int v: firstArr) {
                    if (u != v && !connect[u][v]) {
                        return first * 2;
                    }
                }
            }
            return first * 2 - 1;
        }
    }
};
```

