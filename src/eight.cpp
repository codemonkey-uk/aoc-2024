
#include <iostream>
#include <map>
#include <algorithm>
#include <cassert>
#include "util.h"

using namespace std;

int Decode(const string& str)
{
    // 7 is the only digit that uses three segments
    if (str.size()==3) return 7;
    // 4 is the only digit that uses four segments
    if (str.size()==4) return 4;
    // the only digit that uses two segments is 1
    if (str.size()==2) return 1;
    // 
    if (str.size()==7) return 8;

    return -1;
}

bool Counted(int i)
{
    if (i==1) return true;
    if (i==4) return true;
    if (i==7) return true;
    if (i==8) return true;

    return false;
}
bool contains(const string& s, char c)
{
    return (s.find(c) != std::string::npos);
}

struct Problem
{
    vector< string > patterns;
    vector< string > value;
};

void Eight()
{
    vector<Problem> part_b;

    int total = 0;
    string line;
    while (getline(cin,line))
    {
        auto sequence = Split(line, [](auto c){return c=='|';});
        assert(sequence.size()==2);
        auto patterns = Filter( 
            Split(sequence[0], [](auto c){return c==' ';}),
            [](auto s){return s.empty();}
        );
        assert(patterns.size()==10);
        auto output_value = Filter(
            Split(sequence[1], [](auto c){return c==' ';}),
            [](auto s){return s.empty();}
        );
        assert(output_value.size()==4);

        part_b.push_back({patterns, output_value});
        for (auto v : output_value)
            if (Counted( Decode(v) )) total++;
    }
    cout << "part 1: " << total << endl;

    for(auto& problem : part_b)
    {
        for (auto &p : problem.patterns)
            sort(p.begin(), p.end());
        for (auto &p : problem.value)
            sort(p.begin(), p.end());
    }

    size_t sum = 0;
    for(const auto& problem : part_b)
    {
        map<int, string> solved;
        map<int, vector<string> > unsolved; 
        for (auto v : problem.patterns)
        {
            int i = Decode(v);
            if (i!=-1) solved[i]=v;
            else unsolved[v.size()].push_back(v);
        }
        // 1,4,7,8 are solved

        // 0, 6, 9 : in bucket 6, 
        // 2, 3, 5 : in bucket 5
        assert( unsolved[6].size()==3 );
        assert( unsolved[5].size()==3 );
        
        // only 3 from bucket 5 has both elements same as 1        
        string one = solved[1];
        auto three_itr = find_if(unsolved[5].begin(),unsolved[5].end(),
            [one](const string& c){
                return contains(c,one[0]) && contains(c,one[1]);
            }
        );
        assert(three_itr!=unsolved[5].end());
        solved[3] = *three_itr;
        unsolved[5].erase(three_itr);

        // only 6 from bucket 6 DOESNT have both elements same as 1
        auto six_itr = find_if(unsolved[6].begin(),unsolved[6].end(),
            [one](const string& c){
                return !(contains(c,one[0]) && contains(c,one[1]));
            }
        );
        assert(six_itr!=unsolved[6].end());
        solved[6] = *six_itr;
        unsolved[6].erase(six_itr);

        // 1, 3, 4, 6, 7, 8 are solved

        // 0, 9 : in bucket 6
        // 2, 5 : in bucket 5
        assert( unsolved[6].size()==2 );
        assert( unsolved[5].size()==2 );

        // only 9 from bucket 6 has ALL elements enabled by 3
        string three = solved[3];
        auto nine_itr = find_if(unsolved[6].begin(),unsolved[6].end(),
            [three](const string& s){
                size_t t = count_if(three.begin(), three.end(), [s](auto c){return contains(s,c);});
                return t==three.size();
            }
        );
        assert(nine_itr!=unsolved[6].end());
        solved[9] = *nine_itr;
        unsolved[6].erase(nine_itr);
        assert(unsolved[6].size()==1);
        solved[0] = unsolved[6].back();
        unsolved[6].pop_back();

        // 0, 1, 3, 4, 6, 7, 8, 9 are solved
        // 2, 5 : in bucket 5

        assert( unsolved[6].empty() );
        // only 2 from bucket 5 has an element not pressent in 9
        string nine = solved[9];
        auto two_itr = find_if(unsolved[5].begin(),unsolved[5].end(),
            [nine](const string& s){
                size_t t = count_if(s.begin(), s.end(), [nine](auto c){return !contains(nine,c);});
                return t>0;
            }
        );
        solved[2]=*two_itr;
        unsolved[5].erase(two_itr);
        assert(unsolved[5].size()==1);
        solved[5] = unsolved[5].back();
        unsolved[5].pop_back();

        map< string, int > decode;
        for (auto kvp : solved)
        {
            decode[kvp.second]=kvp.first;
            cout << kvp.first << " " << kvp.second << endl;
        }

        string decoded;
        for (string v : problem.value)
        {
            decoded += ( '0'+decode[v] );
            cout << v << ' ';
        }
        cout << ':' << decoded << endl;

        sum += atoi( decoded.c_str() );
    }
    cout << "total: " << sum << endl;
}