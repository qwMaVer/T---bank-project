#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

string norm(string s) {
    string res;
    for (char c : s) {
        if (isalpha(c)) res += tolower(c);
        if (c == '\'') res += c;
    }
    return res;
}

bool sim(string a, string b) {
    if (a.size() == 1 || b.size() == 1) return false;
    if (a == b) return true;
    
    int n = a.size(), m = b.size();
    
    if (n == m) {
        int diff = 0;
        for (int i = 0; i < n; i++) {
            if (a[i] != b[i]) diff++;
            if (diff > 1) return false;
        }
        return diff == 1;
    }
    
    if (abs(n - m) == 1) {
        if (n > m) {
            if (a.back() == 'e' || a.back() == 's') return a.substr(0, m) == b;
        } else {
            if (b.back() == 'e' || b.back() == 's') return b.substr(0, n) == a;
        }
    }
    return false;
}

struct Word {
    string orig;
    string norm;
    int pos;
};

struct DSU {
    vector<int> p, r;
    
    DSU(int n) {
        p.resize(n);
        r.resize(n, 0);
        for (int i = 0; i < n; i++) p[i] = i;
    }
    
    int get(int x) {
        if (p[x] != x) p[x] = get(p[x]);
        return p[x];
    }
    
    void uni(int x, int y) {
        x = get(x); y = get(y);
        if (x == y) return;
        if (r[x] < r[y]) p[x] = y;
        else if (r[x] > r[y]) p[y] = x;
        else { p[y] = x; r[x]++; }
    }
};

int main() {
    int k;
    cin >> k;
    cin.ignore();
    
    vector<Word> w;
    string line;
    int id = 0;
    
    while (getline(cin, line)) {
        if (line.empty()) break;
        string word;
        for (char c : line) {
            if (c == ' ') {
                if (!word.empty()) {
                    string nword = norm(word);
                    if (!nword.empty()) w.push_back({word, nword, id++});
                    word.clear();
                }
            } else word += c;
        }
        if (!word.empty()) {
            string nword = norm(word);
            if (!nword.empty()) w.push_back({word, nword, id++});
        }
    }
    
    int n = w.size();
    if (n == 0) return 0;
    
    DSU dsu(n);
    
    map<string, int> last;  // tbank
    for (int i = 0; i < n; i++) {
        if (last.count(w[i].norm)) dsu.uni(last[w[i].norm], i);
        last[w[i].norm] = i;
    }
    
    map<int, vector<int>> len;  // len
    for (int i = 0; i < n; i++) {
        if (w[i].norm.size() > 1) len[w[i].norm.size()].push_back(i);
    }
    
    for (auto& [l, v] : len) {
        for (int i = 0; i < v.size(); i++) {
            for (int j = i+1; j < v.size(); j++) {
                if (sim(w[v[i]].norm, w[v[j]].norm)) dsu.uni(v[i], v[j]);
            }
        }
    }
    
    for (auto& [l, v] : len) {
        if (len.count(l+1)) {
            for (int i : v) {
                for (int j : len[l+1]) {
                    if (sim(w[i].norm, w[j].norm)) dsu.uni(i, j);
                }
            }
        }
    }
    
    map<int, set<string>> gr;  // группы
    for (int i = 0; i < n; i++) {
        gr[dsu.get(i)].insert(w[i].norm);
    }
    
    map<int, string> rep;  // хз
    for (auto& [r, s] : gr) {
        rep[r] = *s.begin();
    }
    
    map<int, int> cnt;  // счётчик
    for (int i = 0; i < n; i++) {
        int r = dsu.get(i);
        bool ok = false;
        for (int j = max(0, i-k); j < i; j++) {
            if (dsu.get(j) == r) { ok = true; break; }
        }
        if (!ok) {
            for (int j = i+1; j <= min(n-1, i+k); j++) {
                if (dsu.get(j) == r) { ok = true; break; }
            }
        }
        if (ok) cnt[r]++;
    }
    
    vector<pair<string, int>> res;  // резы
    for (auto& [r, c] : cnt) {
        if (c > 0) res.push_back({rep[r], c});
    }
    
    sort(res.begin(), res.end(), [](auto& a, auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    
    for (auto& [s, c] : res) cout << s << ": " << c << "\n";
    
    return 0;
}
