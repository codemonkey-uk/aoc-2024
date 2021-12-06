#include <iostream>
#include <vector>
#include "util.h"

using namespace std;

// lanternfish

void Six()
{
    string line;
    getline(cin,line);
    auto sequence = Split(line, [](auto c){return c==',';});
    
    const int lifecycle_d = 9;
    size_t a[lifecycle_d]={0};
    for (const auto& s: sequence)
    {
        int i = atoi(s.c_str());
        assert(i<lifecycle_d);
        a[i]++;
    }
    
    const int max_d = 256;//80;
    size_t b[lifecycle_d]={0};
    for (int d=0;d!=max_d;++d)
    {
        b[8]=a[0];
        b[7]=a[8];
        b[6]=a[7]+a[0];
        b[5]=a[6];
        b[4]=a[5];
        b[3]=a[4];
        b[2]=a[3];
        b[1]=a[2];
        b[0]=a[1];
        copy(b,b+lifecycle_d,a);
    }
    size_t total = 0;
    for(size_t i:a)
        total+=i;
    cout << total << endl;
    
}