#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

void dfs(int node, int parent, vector<vector<int>>& adj, vector<int>& subtree) {
    subtree.push_back(node);
    for (int child : adj[node]) {
        if (child != parent) {
            dfs(child, node, adj, subtree);
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int t;
    cin >> t;
    
    while (t--) {
        int n;
        cin >> n;
        
        vector<int> strengths(n);
        for (int i = 0; i < n; i++) {
            cin >> strengths[i];
        }
        
        vector<int> p(n);
        for (int i = 0; i < n; i++) {
            cin >> p[i];
        }
        
        vector<vector<int>> adj(n + 1);
        for (int i = 0; i < n - 1; i++) {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        
        int winningMove = 0;
        
        // Try each possible first move for Odin
        for (int firstWarrior = 1; firstWarrior <= n; firstWarrior++) {
            set<int> removed;
            
            // Remove subtree of firstWarrior
            vector<int> subtreeFirst;
            dfs(firstWarrior, -1, adj, subtreeFirst);
            for (int node : subtreeFirst) {
                removed.insert(node);
            }
            
            // Trigger chain reaction
            int current = p[firstWarrior - 1];
            while (removed.find(current) == removed.end()) {
                removed.insert(current);
                vector<int> subtreeCurrent;
                dfs(current, -1, adj, subtreeCurrent);
                for (int node : subtreeCurrent) {
                    removed.insert(node);
                }
                current = p[current - 1];
            }
            
            // Check remaining warriors
            vector<int> remaining;
            for (int i = 1; i <= n; i++) {
                if (removed.find(i) == removed.end()) {
                    remaining.push_back(i);
                }
            }
            
            // Thor's turn: must challenge a warrior with strength > strengths[firstWarrior-1]
            int lastStrength = strengths[firstWarrior - 1];
            bool thorCanMove = false;
            vector<int> thorMoves;
            
            for (int warrior : remaining) {
                if (strengths[warrior - 1] > lastStrength) {
                    thorCanMove = true;
                    thorMoves.push_back(warrior);
                }
            }
            
            // If Thor cannot move, Odin wins immediately
            if (!thorCanMove) {
                winningMove = firstWarrior;
                break;
            }
            
            // If Thor can move, check if there's a way for Odin to win after Thor's optimal play
            bool odinCanWin = false;
            
            for (int thorWarrior : thorMoves) {
                set<int> removedAfterThor = removed;
                
                // Remove subtree of thorWarrior
                vector<int> subtreeThor;
                dfs(thorWarrior, -1, adj, subtreeThor);
                for (int node : subtreeThor) {
                    removedAfterThor.insert(node);
                }
                
                // Trigger chain reaction for Thor's move
                int currentThor = p[thorWarrior - 1];
                while (removedAfterThor.find(currentThor) == removedAfterThor.end()) {
                    removedAfterThor.insert(currentThor);
                    vector<int> subtreeCurrent;
                    dfs(currentThor, -1, adj, subtreeCurrent);
                    for (int node : subtreeCurrent) {
                        removedAfterThor.insert(node);
                    }
                    currentThor = p[currentThor - 1];
                }
                
                // Check if Odin has any valid moves after Thor's move
                vector<int> remainingAfterThor;
                for (int i = 1; i <= n; i++) {
                    if (removedAfterThor.find(i) == removedAfterThor.end()) {
                        remainingAfterThor.push_back(i);
                    }
                }
                
                // Odin's turn: must challenge a warrior with strength > strengths[thorWarrior-1]
                int thorStrength = strengths[thorWarrior - 1];
                bool odinCanMove = false;
                
                for (int warrior : remainingAfterThor) {
                    if (strengths[warrior - 1] > thorStrength) {
                        odinCanMove = true;
                        break;
                    }
                }
                
                // If Odin cannot move, then Odin wins (misère game)
                if (!odinCanMove) {
                    odinCanWin = true;
                    break;
                }
            }
            
            if (odinCanWin) {
                winningMove = firstWarrior;
                break;
            }
        }
        
        cout << winningMove << endl;
    }
    
    return 0;
}