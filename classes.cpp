#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <ostream>

using namespace std;

class automat {
protected:
    vector<vector<pair<int, vector<char>>>> stare;
    vector<int> final;
    int initial;

public:
    automat (const vector<vector<pair<int, vector<char>>>>& stare, vector<int> final, int initial) : stare(
    stare), final(std::move(final)), initial(initial) {}

    int getIndex (int nod1, int nod2) const {
        for (int i = 0; i < stare[nod1].size(); ++i)
            if (stare[nod1][i].first == nod2) return i;
        return -1;
    }

    bool getFinal (int index) const {
        for (auto it : final)
            if (it == index) return true;
        return false;
    }

    friend istream &operator>>(istream &is, automat &automat);

    virtual bool verif_cuvant (const string &cuv) = 0;

};

class NFA : public automat {
public:
    NFA (const vector<vector<pair<int, vector<char>>>>& stare, vector<int> final, int initial) : automat(stare, std::move(final), initial) {}

    bool DFS (int start, string cuv, int poz, vector<int> lambda) {
        for (const auto& vecin : stare[start]) {
            for (auto lit : vecin.second) {
                if (lit == cuv[poz]) {
                    if (poz == cuv.length() - 1) {
                        if (getFinal(vecin.first)) return true;
                    }

                    if (DFS(vecin.first, cuv, poz + 1, {})) return true;
                }

                else if (lit == '0') {
                    if (poz == cuv.length()) {
                        if (getFinal(vecin.first)) return true;
                    }

                    if (lambda.empty()) {
                        lambda.push_back(vecin.first);
                        if (DFS(vecin.first, cuv, poz, lambda)) return true;
                        lambda.pop_back();
                    }

                    else {
                        bool ciclu = false;
                        for (auto it : lambda)
                            if (it == vecin.first) {
                                ciclu = true;
                                break;
                            }

                        if (!ciclu) {
                            lambda.push_back(vecin.first);
                            if (DFS(vecin.first, cuv, poz, lambda)) return true;
                            lambda.pop_back();
                        }
                    }
                }
            }
        }
        return false;
    }

    bool verif_cuvant (const string &cuv) override {
        return DFS(initial, cuv, 0, {});
    }
};

istream &operator>>(istream &is, automat &automat) {
    int n, m;
    is >> n >> m;
    automat.stare.resize(n);

    for (int i = 0; i < m; ++i) {
        int nod1, nod2;
        char l;
        is >> nod1 >> nod2 >> l;
        if (automat.getIndex(nod1, nod2) == -1) automat.stare[nod1].push_back({nod2, {l}});
        else automat.stare[nod1][automat.getIndex(nod1, nod2)].second.push_back(l);
    }

    is >> automat.initial;

    int nrF;
    is >> nrF;
    for (int i = 0; i < nrF; ++i) {
        int f;
        is >> f;
        automat.final.push_back(f);
    }

    return is;
}



int main()
{
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    NFA automat({{}},{},0);
    fin >> automat;
    int nrCuv;
    fin >> nrCuv;

    for (int i = 0; i < nrCuv; ++i) {
        string s;
        fin >> s;
        fout << automat.verif_cuvant(s);
    }
    return 0;
}
