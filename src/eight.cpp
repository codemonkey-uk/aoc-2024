
#include <iostream>
#include <cassert>
#include "util.h"

using namespace std;

int Decode(const string& str)
{
    cerr << str << endl;

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

void Eight()
{
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
        for (auto v : output_value)
            if (Counted( Decode(v) )) total++;
    }
    cout << total << endl;
}