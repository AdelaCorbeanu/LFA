#include <bits/stdc++.h>

using namespace std;

class automat {
public:
    vector<vector<pair<int, vector<char>>>> stare;
    vector<int> final;
    int initial;

public:
    automat (const vector<vector<pair<int, vector<char>>>>& stare, vector<int> final, int initial) : stare(
    stare), final(std::move(final)), initial(initial) {}
    vector<vector<pair<int, vector<char>>>> getStare() const {
        return stare;
    }
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
    friend ostream &operator<<(ostream &os, const automat &automat);

    virtual bool verif_cuvant (const string &cuv) = 0;

};


class DFA : public automat {
public:
    DFA (const vector<vector<pair<int, vector<char>>>>& stare, vector<int> final, int initial) : automat(stare, std::move(final), initial) {}

    bool verif_cuvant (const string &cuv) override {
        return false;
    }


    DFA minimizare() {
        vector<vector<pair<int, vector<char>>>> stariDFA = this->stare;
        vector<set<int>> partitii(2);
        vector<int> ce_set(stariDFA.size());

        for (int i = 0; i < stariDFA.size(); ++i) {
            if (getFinal(i)) partitii[1].insert(i), ce_set[i] = 1;
            else partitii[0].insert(i), ce_set[i] = 0;
        }

        bool continua = true;
        while (continua) {

            continua = false;
            int nr_part = partitii.size();

            vector<int> aux(256, -1);
            vector<vector<int>> delta(stariDFA.size(), aux);
            for (int i = 0; i < stariDFA.size(); ++i) {
                for (const auto& vecin: stariDFA[i]) {
                    for (auto ch: vecin.second) {
                        delta[i][ch] = ce_set[vecin.first];
                    }
                }
            }
            auto part_aux = partitii;

            int cnt = 0;
            for (auto& Set : partitii) {
                vector<int> de_sters;
                for (auto i = Set.begin(); i != Set.end(); ++i) {
                    auto j = next(i);

                    while (j != Set.end()) {
                        if (delta[*i] != delta[*j]) {
                            bool exista = false;
                            for (int h = partitii.size(); h < part_aux.size(); ++h) {
                                auto Aux = *part_aux[h].begin();
                                if (delta[Aux] == delta[*j]) {
                                    if (Aux != *j) part_aux[h].insert(*j);
                                    exista = true;
                                }
                            }
                            if (!exista) part_aux.push_back({*j});
                            de_sters.push_back(*j);
                        }
                        j = next(j);
                    }
                }

                for (auto st: de_sters)
                    Set.erase(st);

                auto &it = *(part_aux.begin() + cnt);
                it = Set;
                ++cnt;
            }

            partitii = part_aux;
            for (int i = 0; i < part_aux.size(); ++i) {
                for (auto st : part_aux[i])
                    ce_set[st] = i;
            }

            if (partitii.size() != nr_part) continua = true;
        }

        vector<int> echiv;
        vector<int> raman;
        for (const auto& Set : partitii) {
            raman.push_back(*Set.begin());

            if (Set.size() > 1) {
                auto it = Set.begin();
                for (auto it2 = next(it); it2 != Set.end(); ++it2) {
                    echiv.push_back(*it2);
                    for (auto &st : stariDFA) {
                        for (auto &vecin : st) {
                            if (vecin.first == *it2) vecin.first = *it;
                        }

                        vector<int> de_sters;
                        for (int i = 0; i < st.size(); ++i) {
                            for (int j = i + 1; j < st.size(); ++j) {
                                if (st[i].first == st[j].first) {
                                    for (auto ch : st[j].second)
                                        st[i].second.push_back(ch);
                                    de_sters.push_back(j);
                                }
                            }
                        }

                        for (auto sters : de_sters) {
                            for (int i = sters; i < st.size() - 1; ++i)
                                st[i] = st[i + 1];
                            st.pop_back();
                        }
                    }
                }
            }
        }

        for (auto it : echiv) {
            for (int i = it; i < stariDFA.size() - 1; ++i)
                stariDFA[i] = stariDFA[i + 1];
            stariDFA.pop_back();
        }

        auto aux = stariDFA;
        int nr_stari = 0;
        vector<int> new_final;

        for (auto st : raman) {
            if (getFinal(st)) new_final.push_back(nr_stari);
            for (int i = 0; i < stariDFA.size(); ++i) {
                for (int j = 0; j < stariDFA[i].size(); ++j) {
                    if (stariDFA[i][j].first == st) aux[i][j].first = nr_stari;
                }
            }
            ++nr_stari;
        }

        stariDFA = aux;

        DFA x(stariDFA, new_final, 0);
        return x;
    }

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


    DFA convert_to_DFA () {
        vector<int> new_final;
        vector<vector<pair<int, vector<char>>>> stariDFA(this->stare.size());
        stariDFA = stare;

        map<set<int>, int> Index;

        vector<set<int>> utilizate;
        for (const auto& it : stariDFA) {
            for (const auto& it2 : it) {
                utilizate.push_back({it2.first});
            }
        }

        bool continua = true;
        auto aux = stariDFA;
        size_t nr_stari;

        while (continua) {
            continua = false;
            stariDFA = aux;
            nr_stari = stariDFA.size()-1;
            for (int j = 0; j < stariDFA.size(); ++j) {
                vector<set<int>> litera(256);     //litera[i] = { ... } starile in care ajungem din nodul curent cu litera i

                for (auto &vecin: stariDFA[j]) {
                    for (auto tranz: vecin.second) {
                        litera[tranz].insert(vecin.first);
                    }
                }

                for (int i = 0; i < 256; ++i) {
                    if (!litera[i].empty()) {
                        bool utilizat = false;
                        for (const auto &st: utilizate)
                            if (st == litera[i]) utilizat = true;

                        if (!utilizat && litera[i].size() > 1) {
                            aux.emplace_back();
                            ++nr_stari;
                            bool finala = false;
                            for (auto st: litera[i]) {   //vrem sa stergem muchiile de la aux[j] (starea j) la alea din set (doar cu litera i)
                                if (getFinal(st)) finala = true;

                                for (auto &st2: aux[j]) {
                                    if (st2.first == st) {   //aux[j] e vecin cu un nod din set
                                        int poz = -1;
                                        for (int ch = 0; ch < st2.second.size(); ++ch)  //sterg litera i daca e
                                            if (st2.second[ch] == char(i)) poz = ch;

                                        if (poz != -1) {
                                            for (int ch = poz; ch < st2.second.size() - 1; ++ch)
                                                st2.second[ch] = st2.second[ch + 1];
                                            st2.second.pop_back();
                                        }
                                    }
                                }
                            }
                            if (finala) new_final.push_back((int)nr_stari);
                            aux[nr_stari].push_back({nr_stari, {char(i)}});

                            //vrem sa facem muchie de la aux[j] la starea noua
                            aux[j].push_back({nr_stari, {char(i)}});
                            Index[litera[i]] = (int) nr_stari;
                            utilizate.push_back(litera[i]);

                        } else if (litera[i].size() > 1) {
                            for (auto st: litera[i]) {  //pt fiecare nod din starea noua
                                for (auto & k : aux[j]) {   //pt toti vecinii lui
                                    if (k.first == st) { //daca vecinul asta e in set (vrem sa stergem muchia cu litera i)
                                        int lit = -1;
                                        for (int h = 0;h < k.second.size(); ++h) {    //literele cu care trec in vecinul
                                            if (k.second[h] == char(i)) lit = h;
                                        }

                                        if (lit != -1) {
                                            for (int h = lit; h < k.second.size() - 1; ++h)
                                                k.second[h] = k.second[h + 1];
                                            k.second.pop_back();
                                        }
                                    }
                                }
                            }
                            bool este = false;
                            for (auto &v : aux[j]) {
                                if (v.first == nr_stari) v.second.push_back(char(i)), este = true;
                            }
                            if (!este) aux[j].push_back({nr_stari, {char(i)}});

                            este = false;  //vrem sa ducem muchie la el insusi cu litera i
                            for (auto &v : aux[Index[litera[i]]])
                                if (v.first == Index[litera[i]]) v.second.push_back(char(i)), este = true;
                            if (!este) aux[Index[litera[i]]].push_back({Index[litera[i]], {char(i)}});
                        }
                    }
                }
            }
            if (aux.size() != stariDFA.size()) continua = true;
        }

        stariDFA.clear();
        stariDFA.resize(nr_stari + 1);
        for (int i = 0; i < nr_stari + 1; ++i) {
            for (const auto& st : aux[i]) {
                if (!st.second.empty()) {
                    stariDFA[i].push_back(st);
                    if (getFinal(st.first)) new_final.push_back(st.first);
                }
            }
        }

       DFA x(stariDFA, new_final, 0);
       return x;
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


ostream &operator<<(ostream &os, const automat &automat) {
    int i = 0;
    for (const auto& it : automat.getStare()) {
        os << i << ": ";

        for (const auto& it2 : it) {
            os << it2.first << "(";
            for (auto it3 : it2.second)
                os << it3;
            os << ")  ";
        }

        os << endl;
        i++;
    }

    os << "stari finale: ";
    for (auto it : automat.final) os << it << " ";

    return os;
}


int main()
{
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    NFA automat({{}},{},0);

    ///citeste un lambda-NFA si niste cuvinte si verifica daca sunt acceptate
//    fin >> automat;
//    int nrCuv;
//    fin >> nrCuv;
//
//    for (int i = 0; i < nrCuv; ++i) {
//        string s;
//        fin >> s;
//        fout << automat.verif_cuvant(s);
//    }



      ///citeste un NFA si il converteste la DFA (pe care il afiseaza)
//    fin >> automat;
//    DFA automat2 = automat.convert_to_DFA();
//    fout << automat2;



      ///citeste un DFA si il afiseaza minimizat
//    DFA automat2({{}},{},0);
//    fin >> automat2;
//    fout << automat2.minimizare();

    return 0;
}
