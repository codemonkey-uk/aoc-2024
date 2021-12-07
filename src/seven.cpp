#include <string>
#include <iostream>
#include "util.h"

using namespace std;

void Seven()
{
    string line;
    getline(cin,line);
    auto sequence = Split(line, [](auto c){return c==',';});
    vector<int> v;
    v.reserve(sequence.size());

    for(auto s : sequence)
        v.push_back(atoi(s.c_str()));

    sort(v.begin(),v.end());
    int i = v[v.size()/2];
    cout << "align on " << i << endl;
    int f = 0;
    for(auto vi : v)
        f += abs(i-vi);
    cout << "fuel " << f << endl;
}