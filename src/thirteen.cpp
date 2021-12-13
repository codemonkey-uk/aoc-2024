// Thirteen.cpp
#include <iostream>
#include <set>

#include "util.h"

using namespace std;

struct Pos 
{
    int x;
    int y;
    
    bool operator==(const Pos& rhs) const {
        return x==rhs.x && y==rhs.y;
    }
    bool operator<(const Pos& rhs) const {
        return x<rhs.x || (x==rhs.x && y<rhs.y);
    }

};

struct Fold 
{
    char axis;
    int p;
};

void Render( const set< Pos >& points, bool debug_=debug )
{
    if (debug_)
    {
        int dx=0,dy=0;
        for (Pos p : points)
        {
            if (p.x>dx) dx = p.x;
            if (p.y>dy) dy = p.y;
        }
        for (int y=0;y<=dy;++y)
        {
            for (int x=0;x<=dx;++x)
            {
                cout << ((find(points.begin(),points.end(),Pos{x,y})!=points.end())
                    ? '#'
                    : '.');
            }
            cout << endl;
        }
    }
    cout << points.size() << endl;
}

set<Pos> DoFold(const set<Pos>& points, Fold f)
{
    set<Pos> result;
    for(Pos p : points)
    {
        if (f.axis=='y')
        {
            if (p.y<f.p)
            {
                result.insert(p);
            }
            else 
            {
                result.insert({
                    p.x,
                    f.p - (p.y-f.p)
                });
            }
        }
        else if (f.axis=='x')
        {
            if (p.x<f.p)
            {
                result.insert(p);
            }
            else 
            {
                result.insert({
                    f.p - (p.x-f.p),
                    p.y
                });
            }
        }
    }
    return result;
}

void Thirteen()
{
    set< Pos > points;
    string line;
    while (getline(cin,line))
    {
        if (line.size()==0)
            break;
            
        auto pair = Split(line,',');
        Pos p;
        p.x = atoi(pair[0].c_str());
        p.y = atoi(pair[1].c_str());
        points.insert(p);
    }
    
    Render( points );
    
    const string prefix="fold along ";
    const int psize = prefix.size();
    while (getline(cin,line))
    {
        string right = line.substr( psize );
        auto pair = Split(right,'=');
        Fold f;
        f.axis = pair[0].front();
        f.p = atoi(pair[1].c_str());
        
        points = DoFold(points,f);
        Render( points );
    }
    Render(points, true);
}