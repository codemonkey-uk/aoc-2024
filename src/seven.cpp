#include <string>
#include <iostream>
#include "util.h"

using namespace std;

int Cost(int d)
{
    int result=0;
    while (d)
    {
        result += d--;
    }
    return result;
}

int Fuel_B(const vector<int>& data, int b)
{
    int f = 0;
    for(auto i : data)
        f += Cost( abs(i-b) );
    return f;
}

int Fuel_A(const vector<int>& data, int a)
{
    int f = 0;
    for(auto i : data)
        f += abs(i-a);
    return f;
}

void Seven()
{
    string line;
    getline(cin,line);
    auto sequence = Split(line, [](auto c){return c==',';});
    vector<int> data;
    data.reserve(sequence.size());

    for(auto s : sequence)
        data.push_back(atoi(s.c_str()));

    sort(data.begin(),data.end());

    int a = data[data.size()/2];
    cout << "align on " << a << endl;

    cout << "fuel (a) " << Fuel_A(data,a) << endl;
    
    // just brute foce this, its not so big
    int b = 0;
    int fb = Fuel_B(data,b);
    for(int n=1;n!=data.size();++n)
    {
        int fn = Fuel_B(data,n);
        if (fn<fb)
        {
            b=n; fb=fn;
        }
    }

    cout << "align on " << b << endl;
    cout << "fuel (b) " << fb << endl;
}