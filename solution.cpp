#include <bits/stdc++.h>
using namespace std;
#define int long long
#define pb push_back
#define ff first
#define ss second
#define all(a) a.begin(),a.end()

/*
 * Solution for Odin's Warrior Cascade with Chain Reaction
 * 
 * Key Algorithm: Greedy approach with DFS timing and interval-based chain simulation
 * 
 * Main Insight: For Odin to win by challenging warrior u, there must exist a warrior v
 * outside the combined removal zone (subtree + chain reaction) such that w[v] > w[u]
 * 
 * Time Complexity: O(n log n)
 * Space Complexity: O(n)
 */

signed main(){
ios_base::sync_with_stdio(false);cin.tie(0);cout.tie(0);
int t = 1;
cin >> t;
while(t--){
    int n;
    cin >> n;
    
    // w[i] = strength of warrior i
    // tin[i] = DFS entry time for warrior i (for subtree queries)
    // tout[i] = DFS exit time for warrior i
    vector<int> w(n + 1), tin(n + 1), tout(n + 1);

    for(int i = 1; i <= n; ++i) cin >> w[i];

    // p[i] = permutation mapping (chain reaction target)
    vector<int> p(n + 1);
    for(int i = 1; i <= n; ++i) cin >> p[i];
    
    // adj[i] = adjacency list for tree structure
    vector<vector<int>> adj(n + 1);

    for(int i = 1 ; i < n; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    // DFS timing for efficient subtree queries
    // Key property: node a is ancestor of b iff tin[a] <= tin[b] <= tout[b] <= tout[a]
    int timer = 0;
    function<void(int, int)> dfs = [&](int v, int u) -> void {
        tin[v] = ++timer;  // Entry time when first visiting node v
        for(auto& i: adj[v]) {
            if(i != u) dfs(i, v);
        }
        tout[v] = ++timer; // Exit time when finishing subtree of v
    };

    dfs(1, 0);  // Start DFS from root (warrior 1)

    // Create (strength, warrior_id) pairs for sorting
    vector<pair<int, int>> vp;
    for(int i = 1; i <= n; ++i) vp.push_back({w[i], i});

    sort(vp.begin(), vp.end());  // Sort by strength (ascending)

    // mxOutTime, mnInTime: track timestamp ranges of warriors with higher strength
    // Used for enforceability check: can Thor find a stronger warrior outside removal zone?
    int mxOutTime = 0, mnInTime = LLONG_MAX; 
    bool found = 0;  // Flag to indicate if we found a winning move

    // vis[i] = 1 if warrior i has been processed in current chain reaction
    vector<int> vis(n + 1, 0);
    // Process warriors from strongest to weakest (greedy approach)
    for(int i = n - 1; i >= 0; --i) {
        int j = vp[i].second;  // Current warrior to test
        bool enforceable = !vis[j];  // Can this warrior be challenged?
        
        // Set to store (tin, tout) intervals of removed subtrees during chain reaction
        // This allows O(log n) overlap detection instead of O(n^2)
        set<pair<int,int>> visited;
        
        // Simulate the complete chain reaction: j -> p[j] -> p[p[j]] -> ...
        while(!vis[j]) {
            // Check if current warrior's subtree overlaps with previously removed ones
            // Find the largest interval with tin <= tin[j]
            auto it = visited.upper_bound({tin[j], LLONG_MAX});
            if (it != visited.begin()) {
                --it;
                // Check if intervals [it->first, it->second] and [tin[j], tout[j]] overlap
                if (it->second >= tout[j]) {
                    enforceable = false;  // Chain tries to remove already removed subtree
                    break;
                }
            }
            
            // Enforceability check: exists warrior v outside removal zone with w[v] > w[j]?
            // Outside subtree of j means: timestamp < tin[j] OR timestamp > tout[j]
            // mxOutTime > tout[j]: some stronger warrior has timestamp > tout[j]
            // mnInTime < tin[j]: some stronger warrior has timestamp < tin[j]
            enforceable &= (mxOutTime > tout[j] || mnInTime < tin[j]);
            
            if(!enforceable) {
                break;  // This warrior cannot guarantee a win
            }
            
            // Mark this subtree as removed in the chain reaction
            visited.insert({tin[j], tout[j]});
            vis[j] = 1;  // Mark warrior as processed

            j = p[j];  // Follow the permutation chain
        }
        if(enforceable) {
            found = 1;
            cout << vp[i].second << endl;  // Output the winning warrior
            break;  // Found optimal solution, no need to check weaker warriors
        }
        
        // Update mxOutTime and mnInTime when transitioning to weaker strength group
        // This maintains the range of timestamps for all warriors stronger than current
        if(i > 0 && vp[i].first != vp[i - 1].first) {
            int curr = vp[i].first;
            int j = i;
            // Process all warriors with current strength level
            while(j < n && curr == vp[j].first) {
                mxOutTime = max(mxOutTime, tout[vp[j].second]);
                mnInTime = min(mnInTime, tin[vp[j].second]);
                ++j;
            }
        }
    } 

    if(!found) {
        cout << 0 << endl;  // No winning move exists for Odin
    }
}
return 0;
}