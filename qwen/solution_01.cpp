#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <tuple>
#include <functional>
using namespace std;

typedef long long ll;
typedef pair<int, int> pii;
typedef tuple<int, vector<int>> StateKey; // (last_w, sorted remaining nodes)

struct HashTuple {
    size_t operator()(const StateKey& t) const {
        size_t h = get<0>(t);
        const vector<int>& v = get<1>(t);
        for (int x : v) {
            h ^= hash<int>{}(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

class Solution {
public:
    int n;
    vector<int> w, p;
    vector<vector<int>> children;
    unordered_map<StateKey, bool, HashTuple> memo;
    
    Solution() = default;

    set<int> get_removed_nodes(int start) {
        set<int> removed;
        int cur = start;
        while (removed.find(cur) == removed.end()) {
            // Remove entire subtree of cur
            stack<int> stk;
            stk.push(cur);
            while (!stk.empty()) {
                int node = stk.top(); stk.pop();
                if (removed.find(node) != removed.end()) continue;
                removed.insert(node);
                for (int child : children[node]) {
                    stk.push(child);
                }
            }
            cur = p[cur];
        }
        return removed;
    }

    bool can_current_player_win(set<int> remaining, int last_w) {
        // Create key: (last_w, sorted vector of remaining nodes)
        vector<int> rem_vec(remaining.begin(), remaining.end());
        sort(rem_vec.begin(), rem_vec.end());
        StateKey key = make_tuple(last_w, rem_vec);

        if (memo.count(key)) {
            return memo[key];
        }

        // Find valid moves: nodes v in remaining with w[v] > last_w
        vector<int> moves;
        for (int v : remaining) {
            if (w[v] > last_w) {
                moves.push_back(v);
            }
        }

        if (moves.empty()) {
            memo[key] = true;
            return true;
        }

        for (int v : moves) {
            set<int> removed_v = get_removed_nodes(v);
            set<int> new_remaining;
            for (int node : remaining) {
                if (removed_v.find(node) == removed_v.end()) {
                    new_remaining.insert(node);
                }
            }

            if (!can_current_player_win(new_remaining, w[v])) {
                memo[key] = true;
                return true;
            }
        }

        memo[key] = false;
        return false;
    }

    void solve_case() {
        cin >> n;
        w.resize(n+1);
        p.resize(n+1);
        children.assign(n+1, vector<int>());

        for (int i = 1; i <= n; ++i) {
            cin >> w[i];
        }
        for (int i = 1; i <= n; ++i) {
            cin >> p[i];
        }

        // Build undirected graph
        vector<vector<int>> graph(n+1);
        for (int i = 0; i < n-1; ++i) {
            int u, v;
            cin >> u >> v;
            graph[u].push_back(v);
            graph[v].push_back(u);
        }

        // Build directed tree from root 1
        vector<int> parent(n+1, 0);
        vector<bool> visited(n+1, false);
        queue<int> q;
        q.push(1);
        visited[1] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : graph[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    parent[v] = u;
                    children[u].push_back(v);
                    q.push(v);
                }
            }
        }

        // Clear memo for this test case
        memo.clear();

        int ans = 0;
        for (int x = 1; x <= n; ++x) {
            set<int> removed_x = get_removed_nodes(x);
            set<int> remaining_after_x;
            for (int i = 1; i <= n; ++i) {
                if (removed_x.find(i) == removed_x.end()) {
                    remaining_after_x.insert(i);
                }
            }

            // Thor's turn: if Thor cannot win, Odin wins
            if (!can_current_player_win(remaining_after_x, w[x])) {
                ans = x;
                break;
            }
        }

        cout << ans << '\n';
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        Solution sol;
        sol.solve_case();
    }

    return 0;
}