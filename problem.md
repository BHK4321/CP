# Problem Statement

## Title
Odin's Warrior Cascade

## Problem Description
Odin wants to teach Thor about war strategy.

Odin and Thor are legendary warriors commanding squads in a battle formation, arranged as a hierarchy tree. Each warrior i has a strength wᵢ, and some warriors command subordinates.

Odin moves first.

On his turn, he can challenge any warrior i with strength wᵢ > wⱼ (where j was the last warrior Thor challenged; initially, any warrior is allowed).

When a warrior is challenged:
- The entire squad under him (his subtree in the hierarchy) is removed from the arena.
- Additionally, the warrior assigned to him by the warrior map p[i] triggers a chain reaction, removing that warrior's subtree as well.
- This chain continues recursively: p[p[i]], p[p[p[i]]], and so on, until a warrior is already removed or the chain loops back.

The first player who cannot challenge a warrior wins.

Odin wants to know: which warrior he should challenge first to guarantee a win if both play optimally?

## Input Format
- The first line of input contains a single integer t (1≤t≤10⁵) — the number of test cases.
- The first line of each test case contains one integer n (1≤n≤4⋅10⁵) — the number of warriors.
- The second line contains n integers w₁,w₂,…,wₙ (1≤wᵢ≤n) — the strength of each warrior.
- The third line contains n integers p₁,p₂,...,pₙ (1≤pᵢ≤n) — describing the warrior map. (p is a permutation)
- The next n−1 lines describe the command hierarchy among the warriors. The i-th line contains two integers uᵢ,vᵢ (1≤uᵢ,vᵢ≤n, uᵢ≠vᵢ) — indicating that warrior uᵢ and warrior vᵢ are directly connected in the hierarchy. The hierarchy forms a tree, with Odin's main warrior - 1 at the root.

## Output Format
For each test case, print one line.
- If Odin wins the game, print any possible warrior he may choose in the first turn.
- Otherwise, print "0" (without quotes).

## Constraints
- 1 ≤ t ≤ 10⁵
- 1 ≤ n ≤ 4⋅10⁵
- 1 ≤ wᵢ ≤ n
- 1 ≤ pᵢ ≤ n (p is a permutation)
- 1 ≤ uᵢ, vᵢ ≤ n, uᵢ ≠ vᵢ
- Sum of n over all test cases ≤ 4⋅10⁵
- Time limit: 4 seconds
- Memory limit: 512 MB

## Sample Input
```
1
4
2 1 4 3
2 3 4 1
1 2
1 3
3 4
```

## Sample Output
```
2
```

## Explanation
In this example:
- Warrior 1 has strength 2, warrior 2 has strength 1, warrior 3 has strength 4, warrior 4 has strength 3.
- The permutation map is [2, 3, 4, 1].
- The tree structure: 1 is root, connected to 2 and 3, and 3 is connected to 4.

**Detailed Analysis:**
Choose x=2:
- Delete subtree(2) = {2}. Then
- p[2]=3 — subtree(3) = {3,4} is still present, so delete {3,4}. Next
- p[3]=4, but node 4 is already deleted → stop.

Remaining nodes after the move: {1} only. Now Thor must pick a node with weight > w₂ = 1. The only remaining node is 1 (weight 2), so Thor must pick 1.

If Thor picks 1, he deletes subtree(1) = {1} (then p[1]=2 but 2 already deleted → stop). After Thor's move nothing remains, so Odin has no move — Odin wins (misère).

Therefore Odin can force a win by starting with x=2.

## Notes
- The hierarchy forms a tree with warrior 1 as the root.
- Each pᵢ is distinct (permutation property).
- The game theory aspect requires optimal play from both players.