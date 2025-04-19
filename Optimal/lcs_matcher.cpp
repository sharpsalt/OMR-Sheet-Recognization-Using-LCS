#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>

using namespace std;

double duration_ns;

struct Run {
    int symbol;
    int length;
};

vector<Run> runLengthEncode(const vector<int>& arr) {
    vector<Run> runs;
    if (arr.empty()) return runs;

    int current = arr[0];
    int count = 1;

    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] == current) {
            count++;
        } else {
            runs.push_back({current, count});
            current = arr[i];
            count = 1;
        }
    }
    runs.push_back({current, count});
    return runs;
}

int lcsRunLength(const vector<Run>& X, const vector<Run>& Y) {
    int k = Y.size();
    int l = X.size();

    map<int, vector<int>> LEFT, TOP;
    set<int> symbols;

    for (const auto& run : X) symbols.insert(run.symbol);
    for (const auto& run : Y) symbols.insert(run.symbol);

    for (int alpha : symbols) {
        LEFT[alpha].resize(k + 1, 0);
        TOP[alpha].resize(l + 1, 0);
    }

    for (int i = 1; i <= k; ++i) {
        for (int alpha : symbols)
            LEFT[alpha][i] = LEFT[alpha][i - 1];
        int curr = Y[i - 1].symbol;
        LEFT[curr][i] += Y[i - 1].length;
    }

    for (int j = 1; j <= l; ++j) {
        for (int alpha : symbols)
            TOP[alpha][j] = TOP[alpha][j - 1];
        int curr = X[j - 1].symbol;
        TOP[curr][j] += X[j - 1].length;
    }

    vector<vector<int>> M(k + 1, vector<int>(l + 1, 0));
    map<int, set<pair<int, int>>> pathTrees;

    for (int i = 1; i <= k; ++i) {
        for (int j = 1; j <= l; ++j) {
            if (Y[i - 1].symbol != X[j - 1].symbol) {
                M[i][j] = max(M[i - 1][j], M[i][j - 1]);
            } else {
                int alpha = Y[i - 1].symbol;
                int d = min(Y[i - 1].length, X[j - 1].length);

                int C = 0, R = 0;

                int O_col = TOP[alpha][j] - LEFT[alpha][i];
                auto it_col = pathTrees[alpha].lower_bound({O_col, 0});
                if (it_col != pathTrees[alpha].begin()) {
                    --it_col;
                    C = it_col->second + (TOP[alpha][j] - TOP[alpha][j - 1]);
                }

                int O_row = TOP[alpha][j - 1] - LEFT[alpha][i - 1];
                auto it_row = pathTrees[alpha].lower_bound({O_row, 0});
                if (it_row != pathTrees[alpha].begin()) {
                    --it_row;
                    R = it_row->second + (LEFT[alpha][i] - LEFT[alpha][i - 1]);
                }

                M[i][j] = max({M[i - 1][j - 1] + d, C, R, M[i - 1][j], M[i][j - 1]});

                int ORDER = TOP[alpha][j - 1] - LEFT[alpha][i - 1];
                int value = M[i - 1][j - 1];

                pathTrees[alpha].insert({ORDER, value});
            }
        }
    }

    return M[k][l]; // This is the total LCS length in original units
}

vector<int> readFlattenedArray(const string& filePath) {
    vector<int> array;
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open '" << filePath << "'" << endl;
        return array;
    }

    int val;
    try {
        array.reserve(1000000);
        while (inFile >> val) {
            array.push_back(val);
        }
    } catch (const bad_alloc& e) {
        cerr << "Memory error reading '" << filePath << "': " << e.what() << endl;
        array.clear();
    }
    inFile.close();
    return array;
}

int main() {
    try {
        vector<int> arr1 = readFlattenedArray("Sample.txt");
        vector<int> arr2 = readFlattenedArray("Test.txt");

        if (arr1.empty() || arr2.empty()) {
            cerr << "Error: Input arrays are empty" << endl;
            return 1;
        }

        cout << "Array sizes: " << arr1.size() << " and " << arr2.size() << " elements\n";

        auto X = runLengthEncode(arr1);
        auto Y = runLengthEncode(arr2);

        cout << "Compressed X size: " << X.size() << " runs\n";
        cout << "Compressed Y size: " << Y.size() << " runs\n";

        auto start = chrono::high_resolution_clock::now();
        int lcsLength = lcsRunLength(X, Y);
        auto end = chrono::high_resolution_clock::now();
        duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        int totalLength = arr1.size() + arr2.size();
        float similarity = (2.0f * lcsLength / totalLength) * 100.0f;

        cout << "LCS Length (in original elements): " << lcsLength << endl;
        cout << "Total Combined Length: " << totalLength << endl;
        cout << "Similarity: " << similarity << "%" << endl;
        cout << "Execution Time: " << duration_ns << " ns (" 
             << duration_ns / 1e6 << " ms)" << endl;

    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
