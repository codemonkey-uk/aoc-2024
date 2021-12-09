
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include "util.h"

using namespace std;

void Nine()
{
    vector<string> grid;

    string line;
    while (getline(cin,line))
    {
        grid.push_back(line);
    }

    // basin ids
    int basin_count = 0;
    vector< vector<int> > basin_grid;
    const int h = grid.size();
    basin_grid.resize(h);
    for (int row=0;row!=h;++row)
    {
        basin_grid[row].resize( grid[row].size() );
    }
    
    int risk = 0;
    for (int row=0;row!=h;++row)
    {
        int w = grid[row].size();
        for (int column=0;column!=w;++column)
        {
            int count = 0;
            char c1 = grid[row][column];
            if (column==0 ||  c1<grid[row][column-1]) count++;
            if (column==w-1 ||  c1<grid[row][column+1]) count++;
            if (row==0 ||  c1<grid[row-1][column]) count++;
            if (row==h-1 ||  c1<grid[row+1][column]) count++;
            
            if (count==4)
            {
                risk += 1 + c1-'0';
                basin_grid[row][column] = ++basin_count;
            }
        }
    }
    
    cout << "risk = " << risk << endl;
    cout << "found " << basin_count << " basin" << endl;
    
    int remaining = h*grid[0].size();
    int last_remaining;
    do{
        last_remaining = remaining;
        remaining = 0;
        for (int row=0;row!=h;++row)
        {
            int w = grid[row].size();
            for (int column=0;column!=w;++column)
            {
                int b = basin_grid[row][column];
                char c1 = grid[row][column];
                if (b==0)
                    remaining++;
                if (b>0 && c1!='9')
                {
                    if (column>0 && grid[row][column-1]!='9')
                        basin_grid[row][column-1] = b;
                    if (column<w-1 &&  grid[row][column+1]!='9')
                        basin_grid[row][column+1] = b;
                    if (row>0 && grid[row-1][column]!='9')
                        basin_grid[row-1][column] = b;
                    if (row<h-1 && grid[row+1][column]!='9') 
                        basin_grid[row+1][column] = b;
                }
            }
        }
    }while(remaining && last_remaining!=remaining);
    
    cout << "finished with " << remaining << " remaining" << endl;
    
    map< int, int > basin_sizes;
    for (int row=0;row!=h;++row)
    {
        int w = grid[row].size();
        for (int column=0;column!=w;++column)
        {
            int b = basin_grid[row][column];
            if (b) basin_sizes[ b ]++;
        }
    }
    
    vector< int > sizes;
    for (auto kvp : basin_sizes)
    {
        sizes.push_back(kvp.second);
    }
    
    sort(sizes.rbegin(),sizes.rend());
    sizes.resize(3);
    size_t total = 1;
    for (auto s : sizes)
        total = total * s;
    cout << "answer: " << total << endl;
}