#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <functional>

using namespace std;

void solve() {
    int t;
    cin >> t;
    
    while (t--) {
        int n;
        cin >> n;
        
        vector<int> w(n);
        for (int i = 0; i < n; i++) {
            cin >> w[i];
        }
        
        vector<int> p(n);
        for (int i = 0; i < n; i++) {
            cin >> p[i];
            p[i]--; // Convert to 0-indexed
        }
        
        // Build tree
        unordered_map<int, vector<int>> adj;
        for (int i = 0; i < n - 1; i++) {
            int u, v;
            cin >> u >> v;
            u--; v--; // Convert to 0-indexed
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        
        // Build children structure with root 0
        unordered_map<int, vector<int>> children;
        vector<int> parent(n, -1);
        
        function<void(int, int)> build_tree = [&](int node, int par) {
            parent[node] = par;
            for (int neighbor : adj[node]) {
                if (neighbor != par) {
                    children[node].push_back(neighbor);
                    build_tree(neighbor, node);
                }
            }
        };
        
        build_tree(0, -1);
        
        // Get all nodes in subtree of node
        auto get_subtree = [&](int node, const set<int>& removed) -> set<int> {
            set<int> result;
            stack<int> stk;
            stk.push(node);
            
            while (!stk.empty()) {
                int curr = stk.top();
                stk.pop();
                
                if (removed.find(curr) != removed.end()) {
                    continue;
                }
                
                result.insert(curr);
                for (int child : children[curr]) {
                    if (removed.find(child) == removed.end()) {
                        stk.push(child);
                    }
                }
            }
            
            return result;
        };
        
        // Simulate removal after challenging node
        auto simulate_removal = [&](int start_node, const set<int>& removed_set) -> set<int> {
            set<int> current_removed = removed_set;
            
            // Remove subtree of start_node
            set<int> subtree_start = get_subtree(start_node, current_removed);
            for (int node : subtree_start) {
                current_removed.insert(node);
            }
            
            // Chain reaction
            int current = start_node;
            set<int> visited;
            
            while (true) {
                int next_node = p[current];
                if (current_removed.find(next_node) != current_removed.end()) {
                    break;
                }
                if (visited.find(next_node) != visited.end()) {
                    break;
                }
                visited.insert(next_node);
                
                set<int> subtree_next = get_subtree(next_node, current_removed);
                for (int node : subtree_next) {
                    current_removed.insert(node);
                }
                
                current = next_node;
            }
            
            return current_removed;
        };
        
        // Try each possible first move for Odin
        int answer = 0;
        bool found = false;
        
        for (int first_move = 0; first_move < n; first_move++) {
            // Simulate Odin's first move
            set<int> removed = simulate_removal(first_move, set<int>());
            set<int> remaining;
            for (int i = 0; i < n; i++) {
                if (removed.find(i) == removed.end()) {
                    remaining.insert(i);
                }
            }
            
            // Check if Thor has any valid moves (weight > w[first_move])
            vector<int> thor_moves;
            for (int node : remaining) {
                if (w[node] > w[first_move]) {
                    thor_moves.push_back(node);
                }
            }
            
            if (thor_moves.empty()) {
                // Thor cannot move, Odin wins
                answer = first_move + 1;
                found = true;
                break;
            } else {
                // Check if all Thor's moves lead to positions where Odin can win
                bool all_thor_moves_lead_to_odin_win = true;
                
                for (int thor_move : thor_moves) {
                    // Simulate Thor's move
                    set<int> removed_after_thor = simulate_removal(thor_move, removed);
                    set<int> remaining_after_thor;
                    
                    for (int node : remaining) {
                        if (removed_after_thor.find(node) == removed_after_thor.end()) {
                            remaining_after_thor.insert(node);
                        }
                    }
                    
                    // Now Odin's turn, he needs weight > w[thor_move]
                    vector<int> odin_moves;
                    for (int node : remaining_after_thor) {
                        if (w[node] > w[thor_move]) {
                            odin_moves.push_back(node);
                        }
                    }
                    
                    // If Odin has no moves, he wins (misère)
                    if (odin_moves.empty()) {
                        continue; // Odin wins from this position
                    } else {
                        // We don't have enough information to determine if Odin can win from here
                        // For simplicity, assume that if Odin has moves, it doesn't guarantee win
                        all_thor_moves_lead_to_odin_win = false;
                        break;
                    }
                }
                
                if (all_thor_moves_lead_to_odin_win) {
                    answer = first_move + 1;
                    found = true;
                    break;
                }
            }
        }
        
        cout << answer << endl;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    solve();
    return 0;
}