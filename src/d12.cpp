#include <iostream>
#include <map>
#include <set>
#include <iterator>

#include "util.h"

using namespace std;

typedef uint64_t BigInt;

typedef Grid<char> Garden;
typedef Grid<int> Regions;

struct Pos
{
    int col;
    int row;
};

enum Direction {NORTH, EAST, SOUTH, WEST};
Pos Offset[] = {{0,-1},{1,0},{0,1},{-1,0}};

// an edge span
// direction is the direction of the edge away from the inside of the region
struct Span
{
    Pos start;
    Pos end;
    Direction dir;
};

void DebugPrintRegions(const Regions& regions)
{
    for (int row=0;row!=regions.height;row++)
    {
        for (int col=0;col!=regions.width;col++)
        {
            cout << regions.get(col,row);
        }
        cout << endl;
    }
    cout << endl;
}

// FloodFill(map, regions, col, row, region);
void FloodFill(const Garden& map, Regions& regions, int col, int row, int crop, int region)
{
    if (col<0 || col>=map.width) return;
    if (row<0 || row>=map.height) return;
    if (regions.get(col,row)!=0) return;
    if (map.get(col,row)!=crop) return;

    regions.get(col,row) = region;

    FloodFill(map, regions, col-1, row, crop, region);
    FloodFill(map, regions, col+1, row, crop, region);
    FloodFill(map, regions, col, row-1, crop, region);
    FloodFill(map, regions, col, row+1, crop, region);
}

Regions FindRegions(const Garden& map, int* regionsOut)
{
    int region = 1;
    Regions regions(map.width, map.height);
    regions.fill(0);

    // for each square, if its not part of a region, start a new region
    for (int row=0;row!=map.height;row++)
    {
        for (int col=0;col!=map.width;col++)
        {
            if (regions.get(col,row)==0)
            {
                // start a new region
                FloodFill(map, regions, col, row, map.get(col,row), region);
                region++;
                // DebugPrintRegions(regions);
            }
        }
    }

    if (regionsOut) *regionsOut = region;
    return regions;
}

BigInt Area(int r, const Regions& map)
{
    BigInt result=0;
    for (int row=0;row!=map.height;row++)
    {
        for (int col=0;col!=map.width;col++)
        {
            if (map.get(col,row)==r)
            {
                result++;
            }
        }
    }
    return result;
}

BigInt Perimeter(int r, const Regions& map)
{
    BigInt result=0;
    for (int row=0;row!=map.height;row++)
    {
        for (int col=0;col!=map.width;col++)
        {
            if (map.get(col,row)==r)
            {
                if (col==0 || map.get(col-1,row)!=r) result++;
                if (col==map.width-1 || map.get(col+1,row)!=r) result++;
                if (row==0 || map.get(col,row-1)!=r) result++;
                if (row==map.height-1 || map.get(col,row+1)!=r) result++;
            }
        }
    }
    return result;
}

void Insert(Span s, vector<Span>& spans)
{
    // insert a span into a list of spans, merging if possible
    for (auto& span : spans)
    {
        if (span.dir==s.dir)
        {
            if (span.dir == s.dir)
            {
                if ((span.start.col == s.end.col && span.start.row == s.end.row + 1) ||
                    (span.start.col == s.end.col + 1 && span.start.row == s.end.row))
                {
                    span.start = s.start;
                    return;
                }
                if ((span.end.col == s.start.col && span.end.row == s.start.row - 1) ||
                    (span.end.col == s.start.col - 1 && span.end.row == s.start.row))
                {
                    span.end = s.end;
                    return;
                }
            }
        }
    }
    spans.push_back(s);
}

BigInt Sides(int r, const Regions& map)
{
    // count the number of sides of the perimeter of a given region
    vector<Span> spans;

    for (int row=0;row!=map.height;row++)
    {
        for (int col=0;col!=map.width;col++)
        {
            if (map.get(col,row)==r)
            {
                if (col==0 || map.get(col-1,row)!=r) 
                    Insert({{col,row},{col,row},WEST}, spans);

                if (col==map.width-1 || map.get(col+1,row)!=r) 
                    Insert({{col,row},{col,row},EAST}, spans);

                if (row==0 || map.get(col,row-1)!=r) 
                    Insert({{col,row},{col,row},NORTH}, spans);

                if (row==map.height-1 || map.get(col,row+1)!=r) 
                    Insert({{col,row},{col,row},SOUTH}, spans);
            }
        }
    }

    return spans.size();
}

void Twelve()
{
    string line;
    if (!std::getline(std::cin, line)) return;

    Garden map(line.size(), line.size());

    int row = 0;
    do
    {
        for(int col=0;col!=line.size();++col)
        {
            map.get(col,row) = line[col];// == '.' ? -1 : line[col]-'0';
        }
        row++;
    }while (std::getline(std::cin, line));

    int regionCount=0;
    auto regionsMap = FindRegions(map, &regionCount);

    cout << regionCount-1 << endl;
    
    BigInt p1sum=0;
    BigInt p2sum=0;
    for (int r = 1; r!=regionCount; ++r)
    {
        BigInt a = Area(r, regionsMap);
        BigInt p = Perimeter(r, regionsMap);
        BigInt s = Sides(r, regionsMap);
        
        p1sum += a*p;
        p2sum += a*s;
        cout << r << ": "<< a << " tiles " << p << " permiter " << s << " sides " << endl;
    }
    
    cout << "P1: " << p1sum << endl;
    cout << "P2: " << p2sum << endl;
}