#include <bits/stdc++.h>
using namespace std;
#define int long long
#define pb push_back
#define ff first
#define ss second
#define all(a) a.begin(),a.end()
signed main(){
ios_base::sync_with_stdio(false);cin.tie(0);cout.tie(0);
int t = 1;
cin >> t;
while(t--){
    int n;
    cin >> n;
    vector<int> w(n + 1), tin(n + 1), tout(n + 1);

    for(int i = 1; i <= n; ++i) cin >> w[i];

    vector<int> p(n + 1);

    for(int i = 1; i <= n; ++i) cin >> p[i];
    vector<vector<int>> adj(n + 1);

    for(int i = 1 ; i < n; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    function<void(set<int>&, int, int, set<int>&)> dfs = [&](set<int>& group, int v, int u, set<int>& leftNodes) -> void {
        if(group.find(v) != group.end()) {
            return ;
        }
        leftNodes.insert(v);
        for(auto& i: adj[v]) {
            if(i != u) dfs(group, i, v, leftNodes);
        }
    };

    vector<int> possibleNodes;
    for(int i = 1; i <= n; ++i) {
        set<int> group;
        int j = i;
        set<int> leftNodes;
        while(true) {
            group.insert(j);
            leftNodes.clear();
            dfs(group, 1, 0, leftNodes);
            j = p[j];
            if(group.find(j) != group.end() || leftNodes.find(j) == leftNodes.end()) break;
        }

        set<int> st;
        for(auto& j : leftNodes) {
            if(w[i] < w[j]) st.insert(w[j]);
        }
        if(st.size() == 1) {
            possibleNodes.push_back(i);
        }
    }
    for(auto& i: possibleNodes) cout << i << " ";
    cout << endl;
}
return 0;
}