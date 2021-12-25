// twentyfive.cpp 
// Sea Cucumber

#include <iostream>
#include <vector>
#include "util.h"
#include "geometry/vector2d.h"

using namespace std;

namespace {

typedef Grid<char> Map;
typedef Geometry::Vector2d<int> Vec;

Map MakeMap(const vector< string >& lines)
{
    Map result( lines[0].size(), lines.size() );
    
    for (int x=0;x!=result.width;++x)
    {
        for (int y=0;y!=result.height;++y)
        {
            result.get(x,y)=lines[y][x];
        }
    }

    return result;
}

Map ReadMap()
{
    vector< string > lines;
    string line;
    while (getline(cin, line))
    {
        lines.push_back(line);
    }
    
    return MakeMap(lines);
}

vector<string> ToString(const Map& m)
{
    vector< string > lines;
    lines.reserve(m.height);
    for(int y=0;y!=m.height;++y)
    {
        string line;
        line.reserve(m.width);
        for(int x=0;x!=m.width;++x)
            line.push_back(m.get(x,y));
        lines.push_back(line);
    }
    
    return lines;
}

int StepMap(
    const Map& a, 
    Map& b,
    char c, Vec d)
{
    int count=0;
    
    for(int x=0;x!=a.width;++x)
    {
        for(int y=0;y!=a.height;++y)
        {
            // this spot is occupied by a mover
            if (a.get(x,y)==c)
            {
                // destination spot...
                int x2 = (x+d[0]) % a.width;
                int y2 = (y+d[1]) % b.height;
                if (a.get(x2,y2)=='.')
                {
                    // is free, so the mover LEAVES this spot
                    b.get(x,y)='.';
                }
                else
                {
                    // is occupied, the mover stays put
                    b.get(x,y)=c; // == a.get(x,y);
                }
            }
            // empty spot
            else if (a.get(x,y)=='.')
            {
                // is anything moving to this spot?
                int x2 = (x-d[0]);
                if (x2<0) x2+=a.width;
                int y2 = (y-d[1]);
                if (y2<0) y2+=a.height;
                if (a.get(x2,y2)==c)
                {
                    // 
                    b.get(x,y)=c;
                    count++;
                }
                else 
                {
                    // 
                    b.get(x,y)='.';
                }
            }
            else
            {
                b.get(x,y)=a.get(x,y);
            }
        }
    }
    
    return count;
}

Map StepMap(
    const Map& a, 
    char c, Vec d)
{
    Map b(a.width,a.height);
    StepMap(a,b,c,d);
    return b;
}

Map StepMap(const Map& a)
{
    Map b = StepMap(a,'>',{1,0});
    return StepMap(b,'v',{0,1});
}

int StepMap(Map& a, Map& b)
{
    int east = StepMap(a,b,'>',{1,0});
    int south = StepMap(b,a,'v',{0,1});
    return east+south;
}

void DebugOut(const Map &map)
{
    for (auto l : ToString(map) )
        cout << l << endl;
}

int StepsUntilStopped(Map a)
{
    int steps=1;
    Grid< char > b(a.width,a.height);    
    while (StepMap(a,b))
        steps++;
    return steps;
}

void SelfTest()
{
    auto a = MakeMap({"...>>>>>..."});
    auto b = StepMap(a);
    
    assert(ToString(b)[0]=="...>>>>.>..");
    b = StepMap(b);
    assert(ToString(b)[0]=="...>>>.>.>.");
    
    auto a1 = MakeMap({
        "..........",
        ".>v....v..",
        ".......>..",
        ".........."
    });
    a1 = StepMap(a1);
    bool check = (ToString(a1)==vector<string>{
        "..........",
        ".>........",
        "..v....v>.",
        ".........."
    });
    assert(check);
    
    auto wrap = MakeMap({
        "...>...",
        ".......",
        "......>",
        "v.....>",
        "......>",
        ".......",
        "..vvv.."
    });
    for (int steps=0;steps!=4;++steps)
    {
        wrap = StepMap(wrap);
    }
    check = (ToString(wrap)==vector<string>{
        ">......",
        "..v....",
        "..>.v..",
        ".>.v...",
        "...>...",
        ".......",
        "v......"
    });
    assert(check);
    
    
}

}//namespace

void TwentyFive()
{
    SelfTest();
    
    Grid< char > a = ReadMap();

    cout << "Part 1: " << StepsUntilStopped(a) << endl;
}