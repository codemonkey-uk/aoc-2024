#include <iostream>
#include <deque>
#include <map>
#include <set>

#include "util.h"

using namespace std;

typedef int64_t BigInt;

struct Antenna
{
    int row;
    int col;

    // order operators
    bool operator==(const Antenna& rhs)const{return row==rhs.row && col==rhs.col;}
    bool operator<(const Antenna& rhs)const{return row<rhs.row || (row==rhs.row && col<rhs.col);}
};

typedef map<char, vector< Antenna > > AntennaMap;

pair<Antenna,Antenna> Antinode(Antenna a, Antenna b)
{
    int abr = b.row - a.row;
    int abc = b.col - a.col;

    pair<Antenna,Antenna> result;
    
    result.first.row = a.row - abr;
    result.first.col = a.col - abc;
    result.second.row = b.row + abr;
    result.second.col = b.col + abc;

    return result;
}

bool InBounds(Antenna a, int width, int height)
{
    return a.row >= 0 && a.row < height && a.col >= 0 && a.row < width;
}

void Eight()
{
    AntennaMap map;

    string line;
    int row = 0;
    int width = 0;
    while (std::getline(std::cin, line))
    {
        if (line.size()>width) width=line.size();
        for (int col=0;col!=line.size();++col)
        {
            char c =line[col];
            if (c!='.')
            {
                map[c].push_back(Antenna{row,col});
            }
        }

        row++;
    }

    int height = row;
    cout << width << " x " << height << endl;

    set< Antenna > antinodes;

    for (auto kvp : map)
    {
        cout << '\'' << kvp.first << '\'' << 'x' << kvp.second.size() << ":" << endl;

        // for each pair
        for (int a=0;a<kvp.second.size();++a)
        {
            for (int b=a+1;b<kvp.second.size();++b)
            {
                auto anti = Antinode(kvp.second[a], kvp.second[b]);

                cout << '\t' << anti.first.col << "," << anti.first.row << " & ";
                cout << anti.second.col << "," << anti.second.row << endl;
                
                if (InBounds(anti.first, width, height))
                    antinodes.insert(anti.first);
                if (InBounds(anti.second, width, height))
                    antinodes.insert(anti.second);
            }    
        }
    }

    int total = 0;
    for (int row=0;row!=width;++row)
    {        
        for (int col=0;col!=height;++col)
        {
            bool b= antinodes.find({row,col})!=antinodes.end();
            cout << (b ? "#" : ".");
            if (b) total++;
        }
        cout << endl;
    }

    
    // 305 - correct
    cout << "P1: " << total << endl;
    
    // 329 - too high -- why???
    // cout << "P1: " << antinodes.size() << endl;
}