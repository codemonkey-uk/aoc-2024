#include <iostream>
#include <deque>
#include <map>
#include <set>

#include "util.h"

using namespace std;

typedef Grid<int> Topo;
typedef Grid<int> Visited;

void DebugPrint(const Topo& map)
{
    for (int row=0;row!=map.height;row++)
    {
        for (int col=0;col!=map.width;col++)
        {
            if (map.get(col,row)>=0)
                cout << map.get(col,row);
            else cout << ".";
        }
        cout << endl;
    }
    cout << endl;
}

struct Pos 
{
    int col; int row;
};

vector<Pos> FindTrailHeads(const Topo& map)
{
    vector<Pos> result;

    Pos p;
    for (p.row=0;p.row!=map.height;p.row++)
    {
        for (p.col=0;p.col!=map.width;p.col++)
        {
            if (map.get(p.col,p.row)==0)
            {
                result.push_back(p);
            }
        }
    }
    return result;
};

bool Valid(const Topo& map, Pos a, Pos b)
{
    if (b.row<0 || b.row>=map.height) return false;
    if (b.col<0 || b.col>=map.width) return false;

    return map.get(a.col,a.row)==map.get(b.col,b.row)-1;
}

void FindPeaks(const Topo& map, Visited& visited, Pos p, vector<Pos>& results)
{
    // do not revisit
    if (visited.get(p.col,p.row)) return;
    visited.get(p.col,p.row) = 1;

    // found a peak
    if (map.get(p.col,p.row) == 9)
    {
        results.push_back(p);
        return;
    }
    
    // check orthogonal neighbours
    Pos next;

    next.row=p.row + 1;
    next.col=p.col;
    if (Valid(map,p,next)) FindPeaks(map,visited,next,results);

    next.row=p.row - 1;
    next.col=p.col;
    if (Valid(map,p,next)) FindPeaks(map,visited,next,results);

    next.row=p.row;
    next.col=p.col + 1;
    if (Valid(map,p,next)) FindPeaks(map,visited,next,results);
    
    next.row=p.row;
    next.col=p.col - 1;
    if (Valid(map,p,next)) FindPeaks(map,visited,next,results);
}

vector<Pos> FindPeaks(const Topo& map, Pos p)
{
    vector<Pos> results;
    Visited visited(map.width, map.height);
    visited.fill(0);

    FindPeaks(map,visited,p,results);
    return results;
}

void Ten()
{
    
    string line;
    if (!std::getline(std::cin, line)) return;

    Topo map(line.size(), line.size());

    int row = 0;
    do
    {
        for(int col=0;col!=line.size();++col)
        {
            map.get(col,row) = line[col] == '.' ? -1 : line[col]-'0';
        }
        row++;
    }while (std::getline(std::cin, line));

    DebugPrint(map);

    vector<Pos> trailHeads = FindTrailHeads(map);
    cout << trailHeads.size() << " trail head(s) found." << endl;

    size_t sum = 0;
    for (int head = 0;head!=trailHeads.size();head++)
    {
        auto peaks = FindPeaks(map,trailHeads[head]);
        cout << peaks.size() << " reachable peaks found." << endl;
        sum += peaks.size();
    }

    //Visited visits(line.size(), line.size());
    //DebugPrint(visits);

    cout << "P1 total: " << sum << endl;
}