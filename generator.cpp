#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <set>
#include <algorithm>
using namespace std;

// Test case generator for Odin's Warrior Cascade problem
// Generates input/output pairs for testing

class WarriorCascadeGenerator {
private:
    mt19937 rng;
    
public:
    WarriorCascadeGenerator() : rng(random_device{}()) {}
    
    // Generate a permutation of size n
    vector<int> generatePermutation(int n) {
        vector<int> perm(n);
        for (int i = 0; i < n; i++) {
            perm[i] = i + 1;
        }
        shuffle(perm.begin(), perm.end(), rng);
        return perm;
    }
    
    // Generate a random tree with n nodes
    vector<pair<int, int>> generateRandomTree(int n) {
        vector<pair<int, int>> edges;
        
        if (n == 1) return edges;
        
        // Generate a random tree using Prufer sequence
        vector<int> degree(n + 1, 1);
        vector<int> prufer(n - 2);
        
        for (int i = 0; i < n - 2; i++) {
            prufer[i] = uniform_int_distribution<int>(1, n)(rng);
            degree[prufer[i]]++;
        }
        
        // Convert Prufer sequence to tree
        int ptr = 1;
        while (degree[ptr] != 1) ptr++;
        
        int leaf = ptr;
        for (int i = 0; i < n - 2; i++) {
            int v = prufer[i];
            edges.push_back({leaf, v});
            degree[leaf]--;
            degree[v]--;
            
            if (degree[v] == 1 && v < ptr) {
                leaf = v;
            } else {
                ptr++;
                while (degree[ptr] != 1) ptr++;
                leaf = ptr;
            }
        }
        
        // Add last edge
        vector<int> remaining;
        for (int i = 1; i <= n; i++) {
            if (degree[i] == 1) {
                remaining.push_back(i);
            }
        }
        
        if (remaining.size() >= 2) {
            edges.push_back({remaining[0], remaining[1]});
        }
        
        return edges;
    }
    
    // Generate a test case for the warrior cascade problem
    void generateTestCase(int n, const string& inputFile, const string& outputFile) {
        ofstream input(inputFile);
        ofstream output(outputFile);
        
        input << "1" << endl; // Single test case
        input << n << endl;
        
        // Generate warrior strengths (1 to n)
        vector<int> strengths(n);
        for (int i = 0; i < n; i++) {
            strengths[i] = uniform_int_distribution<int>(1, n)(rng);
        }
        
        for (int i = 0; i < n; i++) {
            input << strengths[i];
            if (i < n - 1) input << " ";
        }
        input << endl;
        
        // Generate permutation
        vector<int> permutation = generatePermutation(n);
        for (int i = 0; i < n; i++) {
            input << permutation[i];
            if (i < n - 1) input << " ";
        }
        input << endl;
        
        // Generate tree edges (ensuring node 1 is root)
        vector<pair<int, int>> edges = generateRandomTree(n);
        
        for (auto& edge : edges) {
            input << edge.first << " " << edge.second << endl;
        }
        
        // Calculate expected output using brute force solution
        string result = calculateExpectedOutput(n, strengths, permutation, edges);
        output << result << endl;
        
        input.close();
        output.close();
    }
    
    // Generate multiple test cases in one file
    void generateMultipleTestCases(const vector<int>& testSizes, const string& inputFile, const string& outputFile) {
        ofstream input(inputFile);
        ofstream output(outputFile);
        
        input << testSizes.size() << endl;
        
        for (int n : testSizes) {
            input << n << endl;
            
            // Generate warrior strengths
            vector<int> strengths(n);
            for (int i = 0; i < n; i++) {
                strengths[i] = uniform_int_distribution<int>(1, n)(rng);
            }
            
            for (int i = 0; i < n; i++) {
                input << strengths[i];
                if (i < n - 1) input << " ";
            }
            input << endl;
            
            // Generate permutation
            vector<int> permutation = generatePermutation(n);
            for (int i = 0; i < n; i++) {
                input << permutation[i];
                if (i < n - 1) input << " ";
            }
            input << endl;
            
            // Generate tree edges
            vector<pair<int, int>> edges = generateRandomTree(n);
            for (auto& edge : edges) {
                input << edge.first << " " << edge.second << endl;
            }
            
            // Calculate expected output for this test case using brute force
            string result = calculateExpectedOutput(n, strengths, permutation, edges);
            output << result << endl;
        }
        
        input.close();
        output.close();
    }
    
    // Generate edge cases
    void generateEdgeCases() {
        // Single warrior
        generateTestCase(1, "test_cases/edge_single.in", "test_cases/edge_single.out");
        
        // Small cases
        generateTestCase(2, "test_cases/edge_small_2.in", "test_cases/edge_small_2.out");
        generateTestCase(3, "test_cases/edge_small_3.in", "test_cases/edge_small_3.out");
        
        // Medium case
        generateTestCase(100, "test_cases/edge_medium.in", "test_cases/edge_medium.out");
        
        // Multiple small test cases
        generateMultipleTestCases({2, 3, 4, 5}, "test_cases/edge_multiple.in", "test_cases/edge_multiple.out");
    }
    
private:
    // Use brute force solution to calculate correct output
    string calculateExpectedOutput(int n, const vector<int>& strengths, 
                                 const vector<int>& permutation, 
                                 const vector<pair<int, int>>& edges) {
        // Create temporary input file
        string tempInput = "temp_input.txt";
        string tempOutput = "temp_output.txt";
        
        ofstream temp(tempInput);
        temp << "1" << endl;  // Single test case
        temp << n << endl;
        
        // Write strengths
        for (int i = 0; i < n; i++) {
            temp << strengths[i];
            if (i < n - 1) temp << " ";
        }
        temp << endl;
        
        // Write permutation
        for (int i = 0; i < n; i++) {
            temp << permutation[i];
            if (i < n - 1) temp << " ";
        }
        temp << endl;
        
        // Write edges
        for (auto& edge : edges) {
            temp << edge.first << " " << edge.second << endl;
        }
        temp.close();
        
        // Compile and run brute force solution
        string compileCmd = "g++ -o solution_bf_temp solution_bf.cpp";
        string runCmd = "./solution_bf_temp < " + tempInput + " > " + tempOutput;
        
        system(compileCmd.c_str());
        system(runCmd.c_str());
        
        // Read output
        ifstream result(tempOutput);
        string output;
        getline(result, output);
        result.close();
        
        // Cleanup
        remove(tempInput.c_str());
        remove(tempOutput.c_str());
        remove("solution_bf_temp");
        
        return output.empty() ? "0" : output;
    }
};

int main() {
    WarriorCascadeGenerator generator;
    
    cout << "Generating Odin's Warrior Cascade test cases..." << endl;
    
    // Generate various test cases
    generator.generateTestCase(4, "test_cases/gen_1.in", "test_cases/gen_1.out");
    generator.generateTestCase(6, "test_cases/gen_2.in", "test_cases/gen_2.out");
    generator.generateTestCase(10, "test_cases/gen_3.in", "test_cases/gen_3.out");
    
    // Generate multiple test cases
    generator.generateMultipleTestCases({3, 5, 7}, "test_cases/gen_multi.in", "test_cases/gen_multi.out");
    
    // Generate edge cases
    generator.generateEdgeCases();
    
    cout << "Test case generation completed!" << endl;
    
    return 0;
}