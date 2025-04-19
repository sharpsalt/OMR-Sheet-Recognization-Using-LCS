#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <bitset>
#include <unordered_map>
#include <queue>

using namespace std;
using namespace chrono;

// ================ CONFIGURATION ================
const int FOUR_RUSSIANS_BLOCK = 6;       // Block size for bit-parallel
const int WAVELET_THRESHOLD = 1000;      // Switch to wavelet above this

// ================ DATA STRUCTURES ================
struct Run {
    int symbol;
    int length;
};

// ================ WAVELET MATRIX ================
class WaveletMatrix {
    vector<vector<bool>> levels;
    vector<vector<int>> rank0;
    
public:
    void build(const vector<Run>& runs) {
        vector<int> S;
        for (const auto& run : runs) 
            S.insert(S.end(), run.length, run.symbol);
        
        int max_symbol = *max_element(S.begin(), S.end());
        int height = max_symbol > 0 ? 32 - __builtin_clz(max_symbol) : 0;
        
        levels.resize(height);
        rank0.resize(height);
        
        for (int h = height-1; h >= 0; --h) {
            levels[h].resize(S.size());
            rank0[h].resize(S.size()+1);
            
            for (size_t i = 0; i < S.size(); ++i) 
                levels[h][i] = (S[i] >> h) & 1;
                
            for (size_t i = 0; i < S.size(); ++i)
                rank0[h][i+1] = rank0[h][i] + (levels[h][i] == 0);
            
            vector<int> S0, S1;
            for (int v : S)
                ((v >> h) & 1) ? S1.push_back(v) : S0.push_back(v);
            S = move(S0);
            S.insert(S.end(), S1.begin(), S1.end());
        }
    }

    int range_query(int l, int r, int c) {
        int result = 0;
        for (int h = levels.size()-1; h >= 0; --h) {
            bool bit = (c >> h) & 1;
            int cnt0 = rank0[h][r] - rank0[h][l];
            if (bit) {
                result += cnt0;
                l = rank0[h][l] + (levels.size() - rank0[h].size())/2;
                r = rank0[h][r] + (levels.size() - rank0[h].size())/2;
            } else {
                l = rank0[h][l];
                r = rank0[h][r];
            }
        }
        return r - l - result;
    }
};

// ================ SUFFIX AUTOMATON ================
class SuffixAutomaton {
    struct State {
        int len, link;
        map<int, int> next;
    };
    vector<State> st;
    int last;

public:
    SuffixAutomaton() : last(0) {
        st.push_back({0, -1});
    }

    void sa_extend(int c) {
        int p = last;
        st.push_back({st[last].len + 1, -1});
        last = st.size() - 1;
        
        while (p >= 0 && !st[p].next.count(c)) {
            st[p].next[c] = last;
            p = st[p].link;
        }
        
        if (p >= 0) {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[last].link = q;
            } else {
                st.push_back({st[p].len + 1, st[q].link, st[q].next});
                int clone = st.size() - 1;
                while (p >= 0 && st[p].next[c] == q) {
                    st[p].next[c] = clone;
                    p = st[p].link;
                }
                st[q].link = clone;
                st[last].link = clone;
            }
        }
    }

    void build(const vector<Run>& runs) {
        for (const auto& run : runs) {
            for (int i = 0; i < run.length; ++i)
                sa_extend(run.symbol);
        }
    }

    int match(const vector<Run>& pattern) {
        int current = 0, length = 0, max_len = 0;
        for (const auto& run : pattern) {
            if (st[current].next.count(run.symbol)) {
                current = st[current].next[run.symbol];
                length += run.length;
                max_len = max(max_len, length);
            } else {
                while (current != -1 && !st[current].next.count(run.symbol))
                    current = st[current].link;
                if (current == -1) {
                    current = 0;
                    length = 0;
                } else {
                    length = st[current].len;
                    current = st[current].next[run.symbol];
                    length += run.length;
                    max_len = max(max_len, length);
                }
            }
        }
        return max_len;
    }
};

// ================ FOUR RUSSIANS LCS ================
int four_russians_lcs(const vector<Run>& X, const vector<Run>& Y) {
    size_t m = X.size();
    size_t n = Y.size();
    
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    
    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (X[i-1].symbol == Y[j-1].symbol) {
                int match_len = min(X[i-1].length, Y[j-1].length);
                dp[i][j] = dp[i-1][j-1] + match_len;
            } else {
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    
    return dp[m][n];
}
// ================ HYBRID MATCHER ================
int hybrid_lcs(const vector<Run>& X, const vector<Run>& Y) {
    // For small sequences, use DP with run length handling
    if (X.size() < WAVELET_THRESHOLD && Y.size() < WAVELET_THRESHOLD) {
        size_t m = X.size();
        size_t n = Y.size();
        
        vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
        
        for (size_t i = 1; i <= m; ++i) {
            for (size_t j = 1; j <= n; ++j) {
                if (X[i-1].symbol == Y[j-1].symbol) {
                    int match_len = min(X[i-1].length, Y[j-1].length);
                    dp[i][j] = dp[i-1][j-1] + match_len;
                } else {
                    dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
                }
            }
        }
        
        return dp[m][n];
    } 
    // For larger sequences, use suffix automaton with run length handling
    else {
        SuffixAutomaton sa;
        sa.build(X);
        return sa.match(Y);
    }
}

// ================ UTILITIES ================
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

vector<int> readFlattenedArray(const string& filePath) {
    vector<int> array;
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open '" << filePath << "'" << endl;
        return array;
    }

    int val;
    while (inFile >> val) {
        array.push_back(val);
    }
    inFile.close();
    return array;
}

// ================ MAIN ================
int main() {
    try {
        auto arr1 = readFlattenedArray("Sample.txt");
        auto arr2 = readFlattenedArray("Test.txt");

        if (arr1.empty() || arr2.empty()) {
            cerr << "Error: Input arrays are empty" << endl;
            return 1;
        }

        cout << "Array sizes: " << arr1.size() << " and " << arr2.size() << " elements\n";

        auto X = runLengthEncode(arr1);
        auto Y = runLengthEncode(arr2);

        cout << "Compressed X size: " << X.size() << " runs\n";
        cout << "Compressed Y size: " << Y.size() << " runs\n";

        auto start = high_resolution_clock::now();
        int lcs = hybrid_lcs(X, Y);
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        double similarity = (2.0 * lcs) / (arr1.size() + arr2.size());
        cout << "LCS Length: "<<lcs<<endl;
        cout << "Similarity: "<<similarity * 100 <<"%"<<endl;
        cout << "Execution Time: "<<duration.count()*(1e3)<<" nanos\n";

    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}